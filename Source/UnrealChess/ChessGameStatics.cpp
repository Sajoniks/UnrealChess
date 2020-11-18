// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameStatics.h"

int32 UChessGameStatics::GetTileIndexAt(EBoardFile File, EBoardRank Rank)
{
	return static_cast<int32>(File) + 21 + static_cast<int32>(Rank) * 10;
}

//Order of processing
//
//Assume we have 64bit integer
//4		 15                   15                  15                  15 bit
//0000 000000000000000 000000000000000 000000000000000 000000000000000
//
//We will have the following (divided by lines):
//1. 0000 000000000000000 000000000000000 000000000000000 aaaaaaaaaaaaaaaaa
//2. 0000 000000000000000 000000000000000 bbbbbbbbbbbbbbbbb aaaaaaaaaaaaaaaaa
//3. 0000 000000000000000 ccccccccccccccccc bbbbbbbbbbbbbbbbb aaaaaaaaaaaaaaaaa
//4. 0000 ddddddddddddddddd ccccccccccccccccc bbbbbbbbbbbbbbbbb aaaaaaaaaaaaaaaaa
//5. eeee ddddddddddddddddd ccccccccccccccccc bbbbbbbbbbbbbbbbb aaaaaaaaaaaaaaaaa
//
//So we have 64 randomized bits (or uint64 randomized)

uint64 UChessGameStatics::GetRandom64()
{
	return
		(uint64)FMath::Rand() + 
		(uint64)FMath::Rand() << 15 + 
		(uint64)FMath::Rand() << 30 +
		(uint64)FMath::Rand() << 45 +
		((uint64)FMath::Rand() & 0xf) << 60;
};
