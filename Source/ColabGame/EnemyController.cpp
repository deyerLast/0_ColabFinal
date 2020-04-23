// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyController.h"
#include "EnemyObject.h"
#include "ColabGameCharacter.h"
#include "EnemyController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PaperSpriteComponent.h"
#include "PaperSprite.h"
#include "ColabGameCharacter.h"
#include "EnemyObject.h"

//This is what makes the enemy move, rotate, and attack
//This is called every "tick"/ frame in order to stay up to date with the player characters movements
void AEnemyController::Tick(float DeltaTime) {
	FVector MyLoc = PawnAsEnemy->GetActorLocation();
	FVector TargetLoc = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FVector DistanceToTarg = TargetLoc - MyLoc;
	FRotator DesiredRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc) + FRotator(0.0f, 0.0f, 0.0f);
	DesiredRot.Pitch = 0;
	DesiredRot.Roll = 0;
	PawnAsEnemy->SetActorRotation(DesiredRot);
	FVector DesiredMovement = (FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(PawnAsEnemy->Walkspeed, PawnAsEnemy->Runspeed), PawnAsEnemy->GetPendingMovementInputVector().X)) * DeltaTime * PawnAsEnemy->GetActorForwardVector();
	FVector DesiredLoc = MyLoc + DesiredMovement;
	PawnAsEnemy->SetActorLocation(DesiredLoc);
	FVector DirectionToTarget = (GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - MyLoc).GetSafeNormal2D();

	float DistSqXY = FVector::DistSquaredXY(GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(), MyLoc);
	if (DistSqXY <= (PawnAsEnemy->SightDist * PawnAsEnemy->SightDist)) {
		//UE_LOG(LogTemp, Warning, TEXT("I see you"));
		PawnAsEnemy->SetActorLocation(DesiredLoc);
	}
	//FVector DirectionToTarget = (GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation() - MyLoc).GetSafeNormal2D();
	if (DistSqXY <= (PawnAsEnemy->AttRange * PawnAsEnemy->AttRange)) {
		PawnAsEnemy->Attack();
	}

}

//To be called as soon as pawn appears in world-Posses is included in the Controller class 
//Right click on the word "Posses" and hit "Go to definition" to see the controller class definition
void AEnemyController::Possess(APawn* InPawn) {
	Super::Possess(InPawn);
	//Casts object as enemy in order to access enemy functions 
	PawnAsEnemy = Cast<AEnemyObject>(GetPawn());

	//Finds target(player chracter) and caches an object as that type in order to access player chracter functions (Health functions)
	ACharacter* Target = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	AColabGameCharacter* MyTarg = Cast<AColabGameCharacter>(Target);
}


//To be Called upon death of pawn-UnPosses is included in the Controller class 
void AEnemyController::UnPossess() {
	Super::UnPossess();
	PawnAsEnemy = nullptr;
}

