// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GravityPad.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EGPadState : uint8
{
	GP_Idle 	UMETA(DisplayName = "Idle"),
	GP_Delay 	UMETA(DisplayName = "Delay"),
	GP_Rotating	UMETA(DisplayName = "Rotating")
};

UCLASS()
class ROLLINGPLATFORMER_API AGravityPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGravityPad();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleDefaultsOnly, Category = "GravityPad")
		UStaticMeshComponent* GravityMesh;

protected:
	
};
