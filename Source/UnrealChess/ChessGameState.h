// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChessGameState.generated.h"

struct FMoveRecord
{
	
};


/**
 * 
 */
UCLASS(CustomConstructor)
class UNREALCHESS_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()

	AChessGameState(const FObjectInitializer& ObjectInitializer);

	//
	TStaticArray<int32, 120> Tiles;
	
	//
	TArray<FMoveRecord> History;
};
