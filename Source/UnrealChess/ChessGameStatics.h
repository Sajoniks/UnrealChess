// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ChessDefinitions.h"

#include "ChessGameStatics.generated.h"

/**
 * 
 */
UCLASS()
class UNREALCHESS_API UChessGameStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Utility")
	/**Get 120-tile array index from given rank and file
	 * @param Rank Board rank
	 * @param File Board file
	 * @out Index of playable tile in 120-tiles array
	 */
	static int32 GetTileIndexAt(EBoardFile File, EBoardRank Rank);
};
