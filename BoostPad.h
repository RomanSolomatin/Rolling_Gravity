// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "BoostPad.generated.h"

UCLASS()
class ROLLINGPLATFORMER_API ABoostPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoostPad();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleDefaultsOnly, Category = "BoostPad")
		UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere, Category = "BoostPad")
		UStaticMeshComponent* PadMesh;

	UPROPERTY(EditAnywhere, Category = "BoostPad")
		float SpeedImpulse;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	FVector GetImpulse();

	
};
