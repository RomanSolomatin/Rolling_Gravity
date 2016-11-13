// Fill out your copyright notice in the Description page of Project Settings.

#include "RollingPlatformer.h"
#include "LevitationPad.h"


// Sets default values
ALevitationPad::ALevitationPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LevitationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Levitation Pad Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LevitationPad(TEXT("/Game/Assets/StaticMeshes/PlatformDevices/Levitation_Pad/Levitation_Pad.Levitation_Pad"));
	// If the sphere mesh was found, set the static mesh to be it
	if (LevitationPad.Succeeded())
	{
		LevitationMesh->SetStaticMesh(LevitationPad.Object);
	}
	LevitationMesh->SetSimulatePhysics(false);
	LevitationMesh->SetNotifyRigidBodyCollision(true); // Notifies if the Sphere collides with something
	LevitationMesh->SetCollisionProfileName(FName("BlockAll"));

	// Setting Up Collision
	LevitationMesh->OnComponentHit.AddDynamic(this, &ALevitationPad::OnHit);

	RootComponent = LevitationMesh;

	// Setting up the default values for variables
	Time = 2.0f;
	ReturnSpeed = 0.0f;
}

// Called when the game starts or when spawned
void ALevitationPad::BeginPlay()
{
	Super::BeginPlay();
	
	// The pad needs to be returned to its original position and rotation so the information is gained when the object is constructed
	// If it is created in the constructor then the actor has no location or rotation to store returning (0,0,0) for both
	PadStartLoc = GetActorLocation();
	PadStartRot = GetActorRotation();

	// Because the time to fall can be set at different occurences, this value must be stored so it can be reset
	FallTime = Time;
}

// Called every frame
void ALevitationPad::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	switch (LevitationState)
	{
		// Idle state means that the pad stays hovering in the air
	case ELPState::LP_Idle:
		break;
		// When the player has landed on the pad, then it is activated and begins to dim, eventually falling after enough time
	case ELPState::LP_Activated:
		Time -= DeltaTime;
		if (Time < 0.0f)
		{
			// The pad has run out of power and so becomes a physics object so it can fall realistically
			LevitationState = ELPState::LP_Falling;
			LevitationMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
			LevitationMesh->SetSimulatePhysics(true);
			Time = 1.0f;
		}
		break;
	case ELPState::LP_Falling:
		Time -= DeltaTime;
		if (Time <= 0.0f)
		{
			LevitationMesh->SetVisibility(false);
			LevitationState = ELPState::LP_Rising;
			LevitationMesh->SetSimulatePhysics(false);
			LevitationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Time = 10.0f;
		}
		break;
		// The pad rises back to its original position and rotation, this can be done using lerps between the rotation and location vectors
		// Time is the alpha
	case ELPState::LP_Rising:
		Time -= DeltaTime;
		if (Time <= 0.0f)
		{
			LevitationMesh->SetVisibility(true);
			LevitationMesh->SetCollisionProfileName(FName("BlockAll"));
			LevitationState = ELPState::LP_Idle;
			SetActorLocation(PadStartLoc);
			SetActorRotation(PadStartRot);
			Time = FallTime;			
		}
		break;
	}

}

// If the player lands on the pad, then it begins to dim and eventually fall

void ALevitationPad::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (LevitationState == ELPState::LP_Idle)
	{
		LevitationState = ELPState::LP_Activated;
	}
}

void ALevitationPad::SetRiseLocation()
{
	//FVector NewLoc;
	//NewLoc.Dist(GetActorLocation(), PadStartLoc);
	//NewLoc.Normalize();
	//NewLoc *= ReturnSpeed;
	//FTransform currentTransform;
	//FVector currentLoc = currentTransform.GetLocation() + NewLoc;
	//currentTransform.SetLocation(currentLoc);
	//SetActorLocation(currentTransform.GetLocation);
}
