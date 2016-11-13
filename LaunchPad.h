// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LaunchPad.generated.h"

UCLASS()
class ROLLINGPLATFORMER_API ALaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaunchPad();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, Category = "LaunchPad")
		UStaticMeshComponent* PadMesh;

	UPROPERTY(EditAnywhere, Category = "LaunchPad")
		float LaunchImpulse;

	UPROPERTY(VisibleDefaultsOnly, Category = "LaunchPad")
		UArrowComponent* LaunchArrow;

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	
protected:
	FVector GetImpulse();
};
