// Fill out your copyright notice in the Description page of Project Settings.

#include "RollingPlatformer.h"
#include "GravityPad.h"
#include "BallPawn.h"


// Sets default values
ABallPawn::ABallPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
// Creating the static mesh to hold the sphere ball shape
	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sphere Static Mesh"));
	// Finding the sphere static mesh model
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Game/Assets/StaticMeshes/Player/Player_Sphere.Player_Sphere"));
	// If the sphere mesh was found, set the static mesh to be it
	if (SphereAsset.Succeeded())
	{
		Sphere->SetStaticMesh(SphereAsset.Object);
	}
	// Setting up the Sphere to be affected by physics
	Sphere->SetSimulatePhysics(true);
	Sphere->SetAngularDamping(1.0f);
	Sphere->SetLinearDamping(0.5f);
	Sphere->BodyInstance.bOverrideMass = true;
	Sphere->BodyInstance.SetMassOverride(1.0f);
	Sphere->BodyInstance.MaxAngularVelocity = 800.0f;
	// Setting up the Sphere's Collision Profile
	Sphere->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Sphere->SetNotifyRigidBodyCollision(true); // Notifies if the Sphere collides with something
	RootComponent = Sphere; // Setting the Sphere as the root component for the player to control

	// Creating the camera spring arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Boom"));
	//CameraBoom->bDoCollisionTest = false;
	CameraBoom->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CameraBoom->bDoCollisionTest = true;
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bAbsoluteRotation = true; // The rotation of the CameraBoom is seperate to the Sphere
	CameraBoom->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f)); // Sets the height of the camera relative to the root
	CameraBoom->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f)); // Sets the rotation of the camera relative to the root
	
	// Creating the Camera
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Main Camera"));
	MainCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName);

	// Creating the Facing Arrow
	FacingArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Facing Arrow"));
	FacingArrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	FacingArrow->bAbsoluteRotation = true;

	// Creating the overlap sphere collision
	SphereOverlap = CreateDefaultSubobject<USphereComponent>(TEXT("Overlap Collision"));
	SphereOverlap->bGenerateOverlapEvents = true;
	SphereOverlap->SetCollisionProfileName(FName("OverlapAll")); // The collision is disabled by default until they enter an artificial gravity region
	SphereOverlap->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SphereOverlap->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	SphereOverlap->SetRelativeScale3D(FVector(1.7f, 1.7f, 1.7f));

	SphereOverlap->OnComponentBeginOverlap.AddDynamic(this, &ABallPawn::OnBeginOverlap);
	SphereOverlap->OnComponentEndOverlap.AddDynamic(this, &ABallPawn::OnEndOverlap);

	// Receive input from the lowest player available
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Setting up Default Variable Values
	RollImpulse = 500.0f;
	JumpImpulse = 300.0f;
	GravityForces = 0;
	bCanJump = true;
	bHasJumped = false;
	bIsBoosting = false;

	// Assigning the overlap events to the overlap collision

	// Adds a tag to let the game know if it's the player or not
	this->Tags.Add("Player"); // Adds the player tag to the actor itself
	ForwardVector = FacingArrow->GetForwardVector();
	RightVector = FacingArrow->GetRightVector();
}

// Called when the game starts or when spawned
void ABallPawn::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABallPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the forward vector array has one or more vectors in it
	// We need to calculate the average vector on the ball and normalise it so we can move in that direction
	if (ForwardVectorArray.Num() >= 1)
	{
			for (int i = 0; i < ForwardVectorArray.Num(); i++)
			{
				ForwardVector += ForwardVectorArray[i]; // Get sum of all vectors
			}
			ForwardVector /= ForwardVectorArray.Num(); // Get the average of forward vectors
			ForwardVector.Normalize(); // Normalise the vector for movement

			// Get the right vector using the maing camera as it changes with perspective
			RightVector = MainCamera->GetRightVector();
			RightVector.Normalize(); // Normalise that vector incase it isn't already
		}
	}
	else
	{
		// If the array is empty, then we use the facing arrow to get our movement directions
		ForwardVector = FacingArrow->GetForwardVector();
		ForwardVector.Z = 0.0f;
		RightVector = FacingArrow->GetRightVector();
	}
}

// Called to bind functionality to input
void ABallPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
	// Bind movement axis to their functions
	InputComponent->BindAxis("MoveForward", this, &ABallPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABallPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABallPawn::Turn);
	InputComponent->BindAxis("Tilt", this, &ABallPawn::Tilt);
	// Bind input actions to their functions using their wanted states
	InputComponent->BindAction("Jump", IE_Pressed, this, &ABallPawn::Jump);
	InputComponent->BindAction("Boost", IE_Pressed, this, &ABallPawn::OnBeginBoost);
	InputComponent->BindAction("Boost", IE_Released, this, &ABallPawn::OnEndBoost);
}

// Move the pawn forwards / back by adding a force that is equal to the current forward vector * the roll impulse scaled by the axis input value -1 to 1
void ABallPawn::MoveForward(float AxisValue)
{
	const FVector force = FVector(AxisValue * ForwardVector) * RollImpulse;
	Sphere->AddForce(force); // Add force to the sphere
}

// Move the pawn to the right / left by getting the current right vector and roll impulse and again scaling the axis input of -1 to 1
void ABallPawn::MoveRight(float AxisValue)
{
	const FVector force = FVector(AxisValue * RightVector) * RollImpulse;
	Sphere->AddForce(force); // Add force to the sphere
}

// Turn the camera by using the input axis value
// Rotating a camera boom and not the camera itself so collision and offset remains the same
void ABallPawn::Turn(float AxisValue)
{
	// Rotating the camera
	FRotator PlayerRotation = CameraBoom->GetComponentRotation();
	PlayerRotation.Yaw += AxisValue;
	CameraBoom->SetRelativeRotation(PlayerRotation);
	// Rotating the arrow so the ball moves the way it is facing
	PlayerRotation = FacingArrow->GetComponentRotation();
	PlayerRotation.Yaw += AxisValue;
	FacingArrow->SetRelativeRotation(PlayerRotation); // Once the facing arrow rotation is calculated, set its rotation
}

// Tilt the camera (pitch) using the inputted axis value
// The amount that the camera can tilt should be limited
void ABallPawn::Tilt(float AxisValue)
{
	float TiltLimit = 45.0f;
	FRotator PlayerRotation = CameraBoom->GetComponentRotation();
	if (PlayerRotation.Pitch >= -TiltLimit && PlayerRotation.Pitch <= TiltLimit) // If the current pitch is between the negative and positive limits
	{
		PlayerRotation.Pitch += AxisValue; // Add the axis value to the pitch (tilt)
		if (PlayerRotation.Pitch > TiltLimit)
		{
			PlayerRotation.Pitch = TiltLimit; // If it is now over the limit, set it to the limit
		}
		else if (PlayerRotation.Pitch < -TiltLimit)
		{
			PlayerRotation.Pitch = -TiltLimit; // Set to negative limit if below the negative limit
		}
	}
	CameraBoom->SetRelativeRotation(PlayerRotation); // Set the camera rotation to the new rotation
}

// If the player can jump or can double jump and has only jumped once then let them jump
void ABallPawn::Jump()
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(FacingArrow->GetUpVector() * JumpImpulse); // Add the jump impulse to the current up vector of the sphere
		Sphere->AddImpulse(Impulse);												// Use the facing arrow as the sphere iself rotates around using physics so will not have the actual up vector
		if (bHasJumped)
		{
			bCanJump = false; // Once the ball has jumped, until it lands it cannot jump again
		}
		else
		{
			bHasJumped = true; // The ball has now jumped and as in the air, the ball can double jump so bCanJump still remains true
		}

	}

}

// Notifies if the player hits something, this should allow the player to jump again
void ABallPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	// If the ball has hit a gravity pad, then the physics needs to be changed
	if (!Sphere->IsGravityEnabled() && Hit.IsValidBlockingHit() && Other->ActorHasTag("GravityPad"))
	{
		bCanJump = false; // The ball cannot jump on gravity pads to remove issues
		SetGravity(false); // Gravity should no longer affect the ball as it will be changing directions
		GravityVector = (NormalImpulse); // The normal impulse becomes the gravity vector once we ensure that it is normalised
		GravityVector.Normalize();
		GravityVector *= -1.0f; // Invert the gravity vector
		Sphere->AddForce(GravityVector *= (980.0f)); // Add a force of Earth gravity in the new gravity vector direction
		GenerateCrossProduct();
	}
	else if (Sphere->IsGravityEnabled() && Hit.IsValidBlockingHit()) // If a gravity pad was not hit
	{
		bCanJump = true; // Assume the ball can now jump
		bHasJumped = true;
		SetGravity(true); // The gravity is set to true as we are not currently using a gravity pad
	}

}

// Increase the speed and jump of the ball if the player holds the boost button
void ABallPawn::OnBeginBoost()
{
	RollImpulse = 750.0f;
	JumpImpulse = 350.0f;
	Sphere->BodyInstance.MaxAngularVelocity = 1600.0f; // As the player can now go faster, the max angular velocity must be adjusted to suit
	bIsBoosting = true;
}

// When the player stops boosting, the roll and jump impulses are returned to normal
void ABallPawn::OnEndBoost()
{
	RollImpulse = 500.0f;
	JumpImpulse = 300.0f;
	Sphere->BodyInstance.MaxAngularVelocity = 800.0f;
	bIsBoosting = false;
}

// Add impulse to the ball using the passed vector
void ABallPawn::AddImpulse(FVector VectorImpulse)
{
	Sphere->AddImpulse(VectorImpulse);
}

// Set the roll impulse of the ball
void ABallPawn::SetRollImpulse(float Impulse)
{
	RollImpulse = Impulse;
}

// Set the new jump impulse
void ABallPawn::SetJumpImpulse(float Impulse)
{
	JumpImpulse = Impulse;
}

// Sets whether the world gravity should affect the ball or not
void ABallPawn::SetGravity(bool bIsEnabled)
{
	Sphere->SetEnableGravity(bIsEnabled);
}

// Adds force to the sphere using the vector as a direction to push the force in
void ABallPawn::AddForce(FVector Force)
{
	Sphere->AddForce(Force);
}

void ABallPawn::AddGravityVector()
{
	GravityForces++;
	// Only disable the gravity and jumping on the first (and subsequently the last) gravity pad as the others in between will still have them disabled
	if (GravityForces == 1)
	{
		Sphere->SetEnableGravity(false);
		bCanJump = false;
	}
}

// Remove a gravity vector from the ones currently affecting the ball
void ABallPawn::RemoveGravityVector()
{
	GravityForces--;
	if (GravityForces <= 0)
	{
		Sphere->SetEnableGravity(true);
	}
}

// Remove all gravity vectors and use normal gravity again
void ABallPawn::EmptyGravityVector()
{
	GravityForces = 0;
	Sphere->SetEnableGravity(true);
}

// Add a forward vector to the array using the passed vector
void ABallPawn::AddForwardVector(FVector ForwardVector)
{
	ForwardVectorArray.Add(ForwardVector);
}

// If the passed vector exists in the forward array, then remove it
void ABallPawn::RemoveForwardVector(FVector ForwardVector)
{
	if (ForwardVectorArray.Contains(ForwardVector))
	{
		ForwardVectorArray.RemoveSingle(ForwardVector);
	}
}

// Depending on the passed value, the overlap collision is turned on or off
void ABallPawn::UpdateGravityOverlap(bool EnableCollision)
{
	if (EnableCollision)SphereOverlap->SetCollisionProfileName(FName("OverlapAll"));
	else SphereOverlap->SetCollisionProfileName(FName("NoCollision"));
}

// When the overlap sphere interacts with another object, it is checked whether it is a gravity pad or not, adds gravity direction if it is
void ABallPawn::OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("GravityPad"))
	{
		AddGravityVector();
	}
}

// When the overlap collision ends, it is checked wether it overlapped with a gravity pad
// If a gravity pad was overlapped to end then it needs to be removed from the amount of gravity vectors playing on the ball
void ABallPawn::OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("GravityPad"))
	{
		RemoveGravityVector();	
	}
}

// The forward vector during periods of artificial gravity is calculated from the current gravity vector
// The vector is normalized along with the right vector of the camera and an origin of 0,0,0
// This creates a triangle that can have its cross product generated to get a perpendicular vector for the forward vector
void ABallPawn::GenerateCrossProduct()
{
	// The gravity vector is assigned to a local variable and normalized
	FVector GVector = GravityVector;
	GVector.Normalize();

	// The right vector of the camera is assigned to a local variable and normalized
	FVector RVector = MainCamera->GetRightVector();
	RVector.Normalize();

	// An origin vector is created and normalized (possibly unnecessary)
	FVector NVector = FVector(0.0f, 0.0f, 0.0f);
	NVector.Normalize();

	// The sides of the triangle are created from the origin point to the other vectors, as they are normalized, they dont need re-normalizing
	FVector Side1 = NVector - GVector;
	FVector Side2 = RVector - GVector;
	FVector Forward;

	// The forward vector is then calculated from the cross product of the vectors.
	// The vector then needs to be normalised as it can be possible to have negative numbers otherwise
	ForwardVector = Forward.CrossProduct(Side1, Side2) * -1.0f;
	ForwardVector.Normalize();
}

// Return the current roll impulse
float ABallPawn::GetRollImpulse()
{
	return RollImpulse;
}

// Return the current jump impulse
float ABallPawn::GetJumpImpulse()
{
	return JumpImpulse;
}

// Return whether or not the ball is boosting
bool ABallPawn::GetIsBoosting()
{
	return bIsBoosting;
}






