// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ColabGameGameMode.h"
#include "ColabGameCharacter.h"

AColabGameGameMode::AColabGameGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = AColabGameCharacter::StaticClass();
}
