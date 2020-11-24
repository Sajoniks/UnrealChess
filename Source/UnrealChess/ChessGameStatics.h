// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ChessMove.h"
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Get")
	static bool IsEnPassantMove(const FChessMove& Move);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Get")
	static bool IsPawnStartMove(const FChessMove& Move);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Get")
	static bool IsCastlingMove(const FChessMove& Move);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Get")
	static bool IsCaptureMove(const FChessMove& Move);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Get")
	static bool IsPromotionMove(const FChessMove& Move);
	
	UFUNCTION(BlueprintCallable, Category="Utility")
	/**Get 120-tile array index from given rank and file
	 * @param Rank Board rank
	 * @param File Board file
	 * @return Index of playable tile in 120-tiles array
	 */
	static int32 GetTileIndexAt(EBoardFile File, EBoardRank Rank);

	UFUNCTION(BlueprintCallable, Category = "Utility")
	/**Converts File and Rank to 64-size array index
	 * @param File Board file
	 * @param Rank Board rank
	 * @return Integer in range 0<=i<64
	 */
	static int32 GetTileIndexAt_64(EBoardFile File, EBoardRank Rank);

	/**Makes random unsigned 64 bit integer
	 * @return Unsigned 64 bit integer
	 */
	static uint64 GetRandom64();

	/**Parses board position from the string
	 * @note InString must be typed as "<BoardFileBoardRank>"
	 * @param InString String to parse
	 * @out Tuple with two coords - file and rank
	 */
	static TPair<EBoardFile, EBoardRank> ParsePositionFromString(const FString& InString);
};
