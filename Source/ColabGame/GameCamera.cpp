// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCamera.h"
#include "Kismet/GameplayStatics.h"

AGameCamera::AGameCamera()
{

}

void AGameCamera::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* player = UGameplayStatics::GetPlayerController(this, 0);
	FViewTargetTransitionParams params;
	player->SetViewTarget(this, params);
}




