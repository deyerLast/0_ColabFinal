// Alex Avello
#include "RoomTriggerVolume.h"
#include "EngineGlobals.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"
#include "ColabGameCharacter.h"

#define print(text) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::Green, text)
#define printFString(text, fstring) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT(text), fstring))

ARoomTriggerVolume::ARoomTriggerVolume()
{
	OnActorBeginOverlap.AddDynamic(this, &ARoomTriggerVolume::OnOverlapBegin);
}

void ARoomTriggerVolume::BeginPlay()
{
	Super::BeginPlay();
	//DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Orange, true, -1, 0, 5);
}

void ARoomTriggerVolume::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
	if (OtherActor && (OtherActor != this))
	{
		AColabGameCharacter* player = Cast<AColabGameCharacter>(OtherActor);
		if (player)
		{
			//print("Overlap Begin");
			player->minX = cameraMinX;
			player->maxX = cameraMaxX;
			player->minZ = cameraMinZ;
			player->maxZ = cameraMaxZ;
			player->transition = true;
		}
	}
}


