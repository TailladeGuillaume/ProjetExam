// Copyright Epic Games, Inc. All Rights Reserved.

#include "EXAM_PROJETGameMode.h"
#include "EXAM_PROJETCharacter.h"
#include "UObject/ConstructorHelpers.h"

AEXAM_PROJETGameMode::AEXAM_PROJETGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
