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
	void Turn(float AxisValue); // Turn camera
	void Tilt(float Axisvalue); // Pitch camera
	// Actions for the player to do
	void Jump(); // Jump
	void OnBeginBoost(); // Open boost to increase speed
	void OnEndBoost(); // Stop boost and return speed to normal

	void AddImpulse(FVector VectorImpulse); // Add the passed impulse to the ball using physics
	void SetRollImpulse(float Impulse); // Set the value of RollImpulse
	void SetJumpImpulse(float Impulse); // Set the value of JumpImpulse
	void SetGravity(bool bIsEnabled); // Set whether UE4 gravity is enabled on the pawn or not
	void AddForce(FVector Force); // Add a force to the ball using the passed vector
	void AddGravityVector(); // Add a gravity vector
	void RemoveGravityVector(); // Remove a gravity vector
	void AddForwardVector(FVector ForwardVector); // Add the passed vector to the forward vector array
	void RemoveForwardVector(FVector ForwardVector); // Remove the passed vector from the forward vector array, if it exists
	void EmptyGravityVector(); // Empty all of the gravity vectors acting on the ball
	void UpdateGravityOverlap(bool EnableCollision); // Update the gravity pad overlap
	void GenerateCrossProduct(); // Generate cross product for gravity vector
	
	float GetJumpImpulse(); // Return the current value of JumpImpulse
	float GetRollImpulse(); // Return the current value of RollImpulse
	bool GetIsBoosting(); // Return whether or not the pawn is boosting
	
	// Notify when the pawn hits something
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	// Override what happens when the pawn enters an overlap collision
	UFUNCTION()
		void OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Override what happens when the pawn ends an overlap collision
	UFUNCTION()
		void OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// Sphere mesh component
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UStaticMeshComponent* Sphere;

	// Spring arm component for camera boom - offset and collision
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		USpringArmComponent* CameraBoom;

	// Sphere component to activate overlap collision
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		USphereComponent* SphereOverlap;	

	// Camera component
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UCameraComponent* MainCamera;

protected:

	// Number of gravity forces acting on the ball
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		int GravityForces;

	// The current Gravity Vector acting on the ball if not using UE4 gravity
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector GravityVector;

	// Resizable array to store all the forward vectors
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		TArray<FVector> ForwardVectorArray;

	// Current forward vector
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector ForwardVector;

	// Current Right vector
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		FVector RightVector;

	// The impulse used to cause the ball to roll
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		float RollImpulse;

	// The impulse used to cause the ball to jump
	UPROPERTY(EditAnywhere, Category = "Sphere")
		float JumpImpulse;

	// Whether the ball can jump or not
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		bool bCanJump;

	// Whether the ball has jumped or not (different so we can double jump)
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		bool bHasJumped;

	// Whether the ball is currently boosting or not
	UPROPERTY(BlueprintReadOnly, Category = "Sphere")
		bool bIsBoosting;

	// If the ball is rotating or not
	UPROPERTY(BlueprintReadOnly, Category = "Sphere")
		bool bIsRotating;

	// Facing arrow that will not rotate with the ball so we always have a reference
	// Ball rotates when using physics so up,forward,right vectors are useless on sphere itself, which is why we have an arrow
	UPROPERTY(VisibleDefaultsOnly, Category = "Sphere")
		UArrowComponent* FacingArrow;

	
};
