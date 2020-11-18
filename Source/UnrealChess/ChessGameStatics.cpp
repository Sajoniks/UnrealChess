// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameStatics.h"

int32 UChessGameStatics::GetTileIndexAt(EBoardFile File, EBoardRank Rank)
{
	return static_cast<int32>(File) + 21 + static_cast<int32>(Rank) * 10;
};
