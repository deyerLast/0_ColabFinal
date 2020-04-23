// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "RoomTriggerVolume.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class COLABGAME_API ARoomTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
protected:

	virtual void BeginPlay() override;

public:
	//constructor
	ARoomTriggerVolume();

	UFUNCTION()
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UPROPERTY(EditAnywhere)
		float cameraMinX;
	UPROPERTY(EditAnywhere)
		float cameraMaxX;
	UPROPERTY(EditAnywhere)
		float cameraMinZ;
	UPROPERTY(EditAnywhere)
		float cameraMaxZ;
};
