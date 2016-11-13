// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BallPawn.generated.h"

UCLASS()
class ROLLINGPLATFORMER_API ABallPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABallPawn();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Movement controls for the player - adds force on direction
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	// Camera controls for the player - turns the forward vector and camera
	void Turn(float AxisValue);
	void Tilt(float Axisvalue);
	// Actions for the player to do
	void Jump();
	void OnBeginBoost();
	void OnEndBoost();

	void AddImpulse(FVector VectorImpulse);
	void SetRollImpulse(float Impulse);
	void SetJumpImpulse(float Impulse);
	void SetGravity(bool bIsEnabled);
	void AddForce(FVector Force);
	void AddGravityVector();
	void RemoveGravityVector();
	void AddForwardVector(FVector ForwardVector);
	void RemoveForwardVector(FVector ForwardVector);
	void EmptyGravityVector();
	void UpdateGravityOverlap(bool EnableCollision);
	void GenerateCrossProduct();
	
	float GetJumpImpulse();
	float GetRollImpulse();
	bool GetIsBoosting();
	
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UStaticMeshComponent* Sphere;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		USphereComponent* SphereOverlap;	

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UCameraComponent* MainCamera;

protected:

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		int GravityForces;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector GravityVector;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		TArray<FVector> ForwardVectorArray;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector ForwardVector;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector RightVector;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		float RollImpulse;

	UPROPERTY(EditAnywhere, Category = "Sphere")
		float JumpImpulse;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		bool bCanJump;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		bool bHasJumped;

	UPROPERTY(BlueprintReadOnly, Category = "Sphere")
		bool bIsBoosting;

	UPROPERTY(BlueprintReadOnly, Category = "Sphere")
		bool bIsRotating;

	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UArrowComponent* FacingArrow;

	
};
