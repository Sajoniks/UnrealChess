// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChessMove.h"

/**
 * 
 */
struct UNREALCHESS_API FChessMoveRecord
{	
	//
	FChessMove Move;

	//
	int32 CastlePermission;

	//	
	int32 EnPassantTile;

	//
	int32 FiftyMove;

	//
	uint64 PosHashKey;
};
