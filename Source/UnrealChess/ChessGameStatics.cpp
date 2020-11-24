// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameStatics.h"

#include "Kismet/GameplayStatics.h"

bool UChessGameStatics::IsEnPassantMove(const FChessMove& Move)
{
	return Move.IsEnPassantMove();
}

bool UChessGameStatics::IsPawnStartMove(const FChessMove& Move)
{
	return Move.IsPawnStartMove();
}

bool UChessGameStatics::IsCastlingMove(const FChessMove& Move)
{
	return Move.IsCastlingMove();
}

bool UChessGameStatics::IsCaptureMove(const FChessMove& Move)
{
	return Move.GetCapturedPiece() != GEmptyChessPiece.GetCode();
}

bool UChessGameStatics::IsPromotionMove(const FChessMove& Move)
{
	return Move.GetPromotedPiece() != GEmptyChessPiece.GetCode();
}

int32 UChessGameStatics::GetTileIndexAt(EBoardFile File, EBoardRank Rank)
{
	return static_cast<int32>(File) + 21 + static_cast<int32>(Rank) * 10;
}

int32 UChessGameStatics::GetTileIndexAt_64(EBoardFile File, EBoardRank Rank)
{
	return (int32)Rank * 8 + (int32)File;
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
		((uint64)FMath::Rand() << 15) + 
		((uint64)FMath::Rand() << 30) +
		((uint64)FMath::Rand() << 45) +
		(((uint64)FMath::Rand() & 0xf) << 60);
}

TPair<EBoardFile, EBoardRank> UChessGameStatics::ParsePositionFromString(const FString& InString)
{
	return MakeTuple(static_cast<EBoardFile>(InString[0] - L'a'), static_cast<EBoardRank>(InString[1] - L'1'));
};
