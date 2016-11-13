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

	if (ForwardVectorArray.Num() >= 1)
	{
		//	for (int i = 0; i < ForwardVectorArray.Num(); i++)
		//	{
		//		ForwardVector += ForwardVectorArray[i];
		//	}
		//	ForwardVector /= ForwardVectorArray.Num();
		//	ForwardVector.Normalize();

		//	RightVector = MainCamera->GetRightVector();
		//	RightVector.Normalize();
		//}
	}
	else
	{
		ForwardVector = FacingArrow->GetForwardVector();
		ForwardVector.Z = 0.0f;
		RightVector = FacingArrow->GetRightVector();
	}
}

// Called to bind functionality to input
void ABallPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &ABallPawn::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABallPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABallPawn::Turn);
	InputComponent->BindAxis("Tilt", this, &ABallPawn::Tilt);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ABallPawn::Jump);
	InputComponent->BindAction("Boost", IE_Pressed, this, &ABallPawn::OnBeginBoost);
	InputComponent->BindAction("Boost", IE_Released, this, &ABallPawn::OnEndBoost);
}

void ABallPawn::MoveForward(float AxisValue)
{
	const FVector force = FVector(AxisValue * ForwardVector) * RollImpulse;
	Sphere->AddForce(force);
}

void ABallPawn::MoveRight(float AxisValue)
{
	const FVector force = FVector(AxisValue * RightVector) * RollImpulse;
	Sphere->AddForce(force);
}

void ABallPawn::Turn(float AxisValue)
{
	// Rotating the camera
	FRotator PlayerRotation = CameraBoom->GetComponentRotation();
	PlayerRotation.Yaw += AxisValue;
	CameraBoom->SetRelativeRotation(PlayerRotation);
	// Rotating the arrow so the ball moves the way it is facing
	PlayerRotation = FacingArrow->GetComponentRotation();
	PlayerRotation.Yaw += AxisValue;
	FacingArrow->SetRelativeRotation(PlayerRotation);
}

void ABallPawn::Tilt(float AxisValue)
{
	FRotator PlayerRotation = CameraBoom->GetComponentRotation();
	if (PlayerRotation.Pitch >= -45.0f && PlayerRotation.Pitch <= 45.0f)
	{
		PlayerRotation.Pitch += AxisValue;
		if (PlayerRotation.Pitch > 45.0f)
		{
			PlayerRotation.Pitch = 45.0f;
		}
		else if (PlayerRotation.Pitch < -45.0f)
		{
			PlayerRotation.Pitch = -45.0f;
		}
	}
	CameraBoom->SetRelativeRotation(PlayerRotation);
}

// If the player can jump or can double jump and has only jumped once then let them jump
void ABallPawn::Jump()
{
	if (bCanJump)
	{
		const FVector Impulse = FVector(FacingArrow->GetUpVector() * JumpImpulse);
		Sphere->AddImpulse(Impulse);
		if (bHasJumped)
		{
			bCanJump = false;
		}
		else
		{
			bHasJumped = true;
		}

	}

}

// Notifies if the player hits something, this should allow the player to jump again
void ABallPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	
	if (!Sphere->IsGravityEnabled() && Hit.IsValidBlockingHit() && Other->ActorHasTag("GravityPad"))
	{
		bCanJump = false;
		SetGravity(false);
		GravityVector = (NormalImpulse);
		GravityVector.Normalize();
		GravityVector *= -1.0f;
		Sphere->AddForce(GravityVector *= (980.0f));
		GenerateCrossProduct();
	}
	else if (Sphere->IsGravityEnabled() && Hit.IsValidBlockingHit())
	{
		bCanJump = true;
		bHasJumped = true;
		SetGravity(true);
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


void ABallPawn::AddImpulse(FVector VectorImpulse)
{
	Sphere->AddImpulse(VectorImpulse);
}

void ABallPawn::SetRollImpulse(float Impulse)
{
	RollImpulse = Impulse;
}

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

void ABallPawn::RemoveGravityVector()
{
	GravityForces--;
	if (GravityForces <= 0)
	{
		Sphere->SetEnableGravity(true);
	}
}

void ABallPawn::EmptyGravityVector()
{
	GravityForces = 0;
	Sphere->SetEnableGravity(true);
}

void ABallPawn::AddForwardVector(FVector ForwardVector)
{
	ForwardVectorArray.Add(ForwardVector);
}

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

float ABallPawn::GetRollImpulse()
{
	return RollImpulse;
}

float ABallPawn::GetJumpImpulse()
{
	return JumpImpulse;
}

bool ABallPawn::GetIsBoosting()
{
	return bIsBoosting;
}






