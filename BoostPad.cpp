// Fill out your copyright notice in the Description page of Project Settings.

#include "RollingPlatformer.h"
#include "BallPawn.h"
#include "BoostPad.h"


// Sets default values
ABoostPad::ABoostPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RampMesh"));
	RootComponent = PadMesh;
	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	CollisionBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionBox->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CollisionBox->bGenerateOverlapEvents = true;
	CollisionBox->SetCollisionProfileName(FName("OverlapAll"));
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABoostPad::OnBeginOverlap);
	SpeedImpulse = 500000.0f;
}

// Called when the game starts or when spawned
void ABoostPad::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABoostPad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// The player should be given a boost impulse in the direction they are moving if they overlap with the boost pad
void ABoostPad::OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		ABallPawn* PlayerPawn = Cast<ABallPawn>(OtherActor);
		if (PlayerPawn)
		{
			FVector Impulse = GetImpulse();
			PlayerPawn->AddImpulse(Impulse);
		}

	}
}

// Gets the impulse vector 
FVector ABoostPad::GetImpulse()
{
	return (GetActorRightVector() * SpeedImpulse);
}




