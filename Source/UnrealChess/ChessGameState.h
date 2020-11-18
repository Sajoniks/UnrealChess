// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessDefinitions.h"
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

	//All tiles
	TStaticArray<int32, 120> Tiles;

	//Pawns locations
	TStaticArray<uint64, 3> Pawns;

	//King pieces positions
	TStaticArray<int32, 2> Kings;

	//The side that needs to make a move
	//TODO replace this enum
	EPieceColor Side;
	
	//Tile where en passant move is active
	int32 EnPassantTile;

	//Counter that detects 50 move (100 half-move), when game is a draw
	int32 FiftyMoveCounter;
	
	//White half-moves count
	int32 Ply;
	
	//Black half-moves count
	int32 HisPly;

	//13 is a count of chess piece types by each color plus empty tiles count
	//Total count of each chess piece, including empty tiles
	TStaticArray<int32, 13> PieceCount;

	//Count of all non-pawn pieces
	TStaticArray<int32, 3> BigPieces;

	//Count of rooks, queens
	TStaticArray<int32, 3> MajorPieces;

	//Count of bishops, knights
	TStaticArray<int32, 3> MinorPieces;

	uint64 PosHashKey;
	
	//
	TArray<FMoveRecord> History;
};
