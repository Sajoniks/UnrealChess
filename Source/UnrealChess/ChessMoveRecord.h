// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
struct UNREALCHESS_API FChessMoveRecord
{
	//
	int32 MoveTile;

	//
	int32 CastlePermission;

	//	
	int32 EnPassantTile;

	//
	int32 FiftyMove;

	//
	uint64 PosHashKey;
};
