// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "ColabGameCharacter.generated.h"

class UTextRenderComponent;

/**
 * This class is the default character for ColabGame, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class AColabGameCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/* Scene Capture Component that acts as the minimap */
	UPROPERTY(VisibleAnywhere)
	class USceneCaptureComponent2D* MinimapCaptureComponent;

	virtual void Tick(float DeltaSeconds) override;

protected:
	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)//JD
		class UPaperFlipbook* AttackAnimation;

	void OnAttack();     //JD
	void ResetAttack();//JD

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);
	/* Called for crouch and uncrouch*/
	void Duck();
	void UnDuck();

	void UpdateCharacter();
	void UpdateCamera(float DeltaTime);

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool transitionPositiveX = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool transitionPositiveZ = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool startTransition = false;

public:
	AColabGameCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minX = -10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxX = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minZ = -10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxZ = 10;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool transition = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float transitionSpeed = 3000;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector transitionVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool crouching = false;

	bool isAttacking;
	FTimerHandle AttackFlip;
};
