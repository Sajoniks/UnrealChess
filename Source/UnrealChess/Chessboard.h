// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Chessboard.generated.h"

UENUM(BlueprintType)
enum class EGenerationType : uint8
{
	CHESS960,
	DEFAULT,
	RANDOM
};

/**
 *  Class that represent chess board as a set of twelve bitboards (one for each piece) and some generation algorithms
 */
UCLASS(CustomConstructor, BlueprintType)
class UNREALCHESS_API UChessboard : public UObject
{
	GENERATED_BODY()
	
	//White piece - W prefix
	//Black - B

	//White pieces bitboards

	int64 WP, WB, WR, WK, WQ, WH;
	
	//Black pieces bitboards

	int64 BP, BB, BR, BK, BQ, BH;

public:

	UChessboard(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Action")
	void ReloadBoard(EGenerationType GenerationType);

	static void ConvertStringToBitboards(
		const FString (&Board)[8][8],
		int64& WP,
		int64& WH,
		int64& WB,
		int64& WR,
		int64& WK,
		int64& WQ,
		int64& BP,
		int64& BH,
		int64& BB,
		int64& BR,
		int64& BK,
		int64& BQ
	);

	//Converts given binary string to int64
	static int64 ConvertStringToBit(
		const FString& String
	);
};
