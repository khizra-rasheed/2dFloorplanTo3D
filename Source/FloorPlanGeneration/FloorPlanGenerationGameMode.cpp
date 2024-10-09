// Copyright Epic Games, Inc. All Rights Reserved.

#include "FloorPlanGenerationGameMode.h"
#include "FloorPlanGenerationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFloorPlanGenerationGameMode::AFloorPlanGenerationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
