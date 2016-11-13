// Fill out your copyright notice in the Description page of Project Settings.

#include "RollingPlatformer.h"
#include "BallPawn.h"
#include "GravityPad.h"


// Sets default values
AGravityPad::AGravityPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create the static mesh
	GravityMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Gravity Mesh"));
	GravityMesh->SetCollisionProfileName(FName("BlockAll")); // Becomes a solid object
	GravityMesh->SetMobility(EComponentMobility::Static); // The pad is not movable and set to static so it can be baked into shadow maps etc...
	RootComponent = GravityMesh; // The root component should be the static mesh

	this->Tags.Add("GravityPad"); // Adds the player tag to the actor itself
}

// Called when the game starts or when spawned
void AGravityPad::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGravityPad::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}




