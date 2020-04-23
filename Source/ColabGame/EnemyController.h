// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyObject.h"
#include "EnemyController.generated.h"

/**
 *
 */
UCLASS()
class COLABGAME_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;

	virtual void Possess(APawn* InPawn) override;
	virtual void UnPossess() override;


	UPROPERTY(BlueprintReadOnly, Category = "AI")
		AEnemyObject* PawnAsEnemy;



};
