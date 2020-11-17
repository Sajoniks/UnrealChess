// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/*
	* Static class that represents move rules in chess
*/
class UNREALCHESS_API MoveRules
{
public:
	 
	static FString GetPossibleMovesW(
		FString History,
		int64 WP,
		int64 WH,
		int64 WB,
		int64 WR,
		int64 WK,
		int64 WQ,
		int64 BP,
		int64 BH,
		int64 BB,
		int64 BR,
		int64 BK,
		int64 BQ
	);
};
