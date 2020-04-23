
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteComponent.h"
//This class contains all of the attributes of the enemy class and a couple boolean check functions for the controller class 
//This will be the only class representing enemies at it was made with reusability in mind
//Each attribute(sprite,health, attack damage...) can be changed within the editor when the bluprint is made 
#pragma once

#include "Components/BoxComponent.h"
#include "PaperSpriteComponent.h"
#include "ColabGameCharacter.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyObject.generated.h"

UCLASS()
class COLABGAME_API AEnemyObject : public APawn 
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UPaperSpriteComponent* EnemySprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UBoxComponent* EnemyBox;


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AttDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Walkspeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AttCool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Runspeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AttAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Yawspeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AttRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float SightDist;

	void Attack();
	bool ShouldAttack(FVector TargLoc, FVector OurLoc);
	bool InRange(FVector TargLoc, FVector OurLoc);


	UFUNCTION(BlueprintCallable)
		void OnHitComp();

	bool canAttack;
	FTimerHandle attackTimer;

	void resetAttack();


};
