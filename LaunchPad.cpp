// Fill out your copyright notice in the Description page of Project Settings.

#include "RollingPlatformer.h"
#include "BallPawn.h"
#include "LaunchPad.h"


// Sets default values
ALaunchPad::ALaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PadMesh"));
	PadMesh->SetCollisionProfileName(FName("BlockAll"));
	PadMesh->SetNotifyRigidBodyCollision(true);
	RootComponent = PadMesh;

	LaunchArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Launch Arrow"));
	LaunchArrow->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	LaunchArrow->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	LaunchImpulse = 75000.0f;
}

// Called when the game starts or when spawned
void ALaunchPad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaunchPad::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void ALaunchPad::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, OtherActor, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (OtherActor->ActorHasTag("Player"))
	{
		ABallPawn* PlayerBall = Cast<ABallPawn>(OtherActor);
		if (PlayerBall)
		{
			FVector LaunchImpulse = GetImpulse();
			PlayerBall->AddImpulse(LaunchImpulse);
		}
	}

}

FVector ALaunchPad::GetImpulse()
{
	return (LaunchArrow->GetForwardVector() * LaunchImpulse);
}

