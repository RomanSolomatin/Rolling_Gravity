// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LevitationPad.generated.h"

UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class ELPState : uint8
{
	LP_Idle 	UMETA(DisplayName = "Idle"),
	LP_Activated 	UMETA(DisplayName = "Activated"),
	LP_Falling	UMETA(DisplayName = "Falling"),
	LP_Rising UMETA(DisplayName = "Rising")
};

UCLASS()
class ROLLINGPLATFORMER_API ALevitationPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevitationPad();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void SetRiseLocation();

	UPROPERTY(EditAnywhere, Category = "LevitationPad")
		float Time;

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		float FallTime;

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		float ReturnSpeed;

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		FVector PadStartLoc;

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		FRotator PadStartRot;

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		UStaticMeshComponent* LevitationMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "LevitationPad")
		ELPState LevitationState;
	
};
