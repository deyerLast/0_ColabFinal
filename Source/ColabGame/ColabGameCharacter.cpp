// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ColabGameCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printFString(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// AColabGameCharacter

AColabGameCharacter::AColabGameCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->bAbsoluteRotation = true;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->RelativeRotation = FRotator(0.0f, -90.0f, 0.0f);
	
	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 3186.0f;
	SideViewCameraComponent->AspectRatio = 16.0f / 9.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Create a capture component alongside our camera
	MinimapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MinimapCapture"));
	MinimapCaptureComponent->ProjectionType = ECameraProjectionMode::Orthographic;
	MinimapCaptureComponent->OrthoWidth = 16160.0;
	MinimapCaptureComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->bAbsoluteRotation = true;
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->JumpZVelocity = 1700.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;
	GetCharacterMovement()->MaxFlySpeed = 1000.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void AColabGameCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	// Are we moving or standing still?
	UPaperFlipbook* DesiredAnimation = (PlayerSpeedSqr > 0.0f) ? RunningAnimation : IdleAnimation;
	if (isAttacking == true) {
		GetSprite()->SetFlipbook(AttackAnimation);
	}
	else if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void AColabGameCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Orange, true, -1, 0, 5);
	
	UpdateCharacter();	
	UpdateCamera(DeltaSeconds);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AColabGameCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &AColabGameCharacter::MoveRight);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AColabGameCharacter::Duck);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AColabGameCharacter::UnDuck);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AColabGameCharacter::OnAttack);
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AColabGameCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AColabGameCharacter::TouchStopped);
}
void AColabGameCharacter::OnAttack() {
	isAttacking = true;
	GetWorld()->GetTimerManager().SetTimer(AttackFlip, this, &AColabGameCharacter::ResetAttack, 0.5);
}
void AColabGameCharacter::ResetAttack() {
	isAttacking = false;
}

void AColabGameCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	if(!transition)AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void AColabGameCharacter::Duck ()
{
	crouching = true;
}

void AColabGameCharacter::UnDuck ()
{
	crouching = false;
}

void AColabGameCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	if(!transition)Jump();
}

void AColabGameCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void AColabGameCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
	
}

void AColabGameCharacter::UpdateCamera(float DeltaSeconds)
{
	FVector curLocation = SideViewCameraComponent->GetComponentLocation();

	if (transition)
	{
		if (startTransition)
		{
			//remember velocity when crossing rooms
			transitionVelocity = GetCharacterMovement()->Velocity;
			//stop all movement and gravity!
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->GravityScale = 0.0f;
			
			transitionPositiveX = curLocation.X < minX;
			transitionPositiveZ = curLocation.Z < minZ;
			startTransition = false;
		}

		float t = transitionSpeed * DeltaSeconds;
		bool metX = false;
		bool metZ = false;

		if (transitionPositiveX)
		{
			if (curLocation.X < minX)
				curLocation.X += t;
			else
				metX = true;
		}
		else
		{
			if (curLocation.X > maxX)
				curLocation.X -= t;
			else
				metX = true;
		}

		if (transitionPositiveZ)
		{
			if (curLocation.Z < minZ)
				curLocation.Z += t;
			else
				metZ = true;
		}
		else
		{
			if (curLocation.Z > maxZ)
				curLocation.Z -= t;
			else
				metZ = true;
		}

		if (metX && metZ)
		{
			transition = false;
			//set gravity and velocity back to normal
			GetCharacterMovement()->GravityScale = 2.0f;
			GetCharacterMovement()->Velocity = transitionVelocity;
		}
			
	}
	else
	{
		startTransition = true;

		FVector ourLocation = GetActorLocation();
		SideViewCameraComponent->SetWorldLocation(ourLocation);
		curLocation = ourLocation;

		if (curLocation.X > maxX)
			curLocation.X = maxX;

		if (curLocation.X < minX)
			curLocation.X = minX;

		if (curLocation.Z > maxZ)
			curLocation.Z = maxZ;

		if (curLocation.Z < minZ)
			curLocation.Z = minZ;
	}

	curLocation.Y = 500;
	FVector minimapCapturePosition = curLocation;
	minimapCapturePosition.Y = 1000;
	SideViewCameraComponent->SetWorldLocation(curLocation);
	MinimapCaptureComponent->SetWorldLocation(minimapCapturePosition);
	MinimapCaptureComponent->SetWorldRotation(FRotator(0.0f,-90.0f,0.0f));
}
