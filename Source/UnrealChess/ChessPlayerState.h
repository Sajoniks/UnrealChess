// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ChessPlayerState.generated.h"

/**
 * TODO
 */
UCLASS()
class UNREALCHESS_API AChessPlayerState : public APlayerState
{
	GENERATED_BODY()

	//Moves
	int32 Ply;

	//
	int32 Score;
	
};
