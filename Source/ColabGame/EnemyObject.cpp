//This class contains all of the attributes of the enemy class and a couple boolean check functions for the controller class 
//This will be the only class representing enemies at it was made with reusability in mind
//Each attribute(sprite,health, attack damage...) can be changed within the editor when the bluprint is made 

#include "EnemyObject.h"

#include "ColabGameCharacter.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

// Sets default values
AEnemyObject::AEnemyObject()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//These are the default attributes that every enemy will have 
	//All of these attributes can be adjusted inside the editor based on the enemy you want it to be
	Health = 20;

	PrimaryActorTick.bCanEverTick = true;
	//These are the default attributes that every enemy will have 
	//All of these attributes can be adjusted inside the editor based on the enemy you want it to be
	Health = 200;

	AttDamage = 10;
	AttRange = 50;
	Walkspeed = 60;
	Runspeed = 40;
	AttCool = 3;
	AttAngle = 30;
	SightDist = 500;
	EnemySprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("EnemySprite"));
	//Checks for RootComponent, if none exists create one

	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("EnemyBase"));
	}
	//Attach Sprite to root compnent 
	EnemySprite->SetupAttachment(RootComponent);

	EnemyBox = CreateDefaultSubobject<UBoxComponent>(TEXT("EnemyBox"));
	RootComponent = EnemyBox;
	//Attach Sprite to root compnent 
	EnemySprite->SetupAttachment(RootComponent);
	canAttack = true;
	//Have controller take over as soon as an enemy appears in the world
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void AEnemyObject::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEnemyObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemyObject::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//Take health from target, right now there is just a message printed to the output log 
void AEnemyObject::Attack()
{
	UE_LOG(LogTemp, Warning, TEXT("Consider yourself attacked. You have %d less life."), AttDamage);
	if (canAttack) {
		canAttack = false;
		UE_LOG(LogTemp, Warning, TEXT("Consider yourself attacked. You have %d less life."), AttDamage);
		GetWorld()->GetTimerManager().SetTimer(attackTimer, this, &AEnemyObject::resetAttack, 10.0f, false);
	}

}

//Check if target is in enemy attack range, after this check the enemy will attack(true) or continue moving(false)
bool AEnemyObject::ShouldAttack(FVector TargLoc, FVector OurLoc) {
	float RotDistance = FVector::DotProduct(TargLoc, OurLoc);
	if (RotDistance >= FMath::Cos(FMath::DegreesToRadians(AttAngle))) {
		float Distance = FVector::DistSquaredXY(TargLoc, OurLoc);
		if (Distance <= AttRange * AttRange) {
			return true;
		}
	}
	return false;
}

//Check if target is in the range of the enemy sight distance, after this check the enemy will move towards target(true) or remain still(false)
bool AEnemyObject::InRange(FVector TargLoc, FVector OurLoc) {
	float Distance = FVector::DistSquaredXY(TargLoc, OurLoc);
	if (Distance <= SightDist * SightDist) {
		return true;
	}
	return false;

}

void AEnemyObject::OnHitComp() {
	Health = Health - 10;
	//UE_LOG(LogTemp, Warning, TEXT("Ive been hit %f"),Health);
}

void AEnemyObject::resetAttack()
{
	canAttack = true;
	GetWorldTimerManager().ClearTimer(attackTimer);

}
