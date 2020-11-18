// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessDefinitions.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChessMoveRecord.h"

#include "ChessGameState.generated.h"

//TODO Try refactoring some field into player state
//TODO JUST REFACTOR THIS CRAP

/**
 * 
 */
UCLASS(CustomConstructor)
class UNREALCHESS_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()

	AChessGameState(const FObjectInitializer& ObjectInitializer);

	//
	uint64 Bitboard;

	//All tiles
	TStaticArray<int32, 120> Tiles{ 0 };

	//Pawns locations
	TStaticArray<uint64, 3> Pawns{ 0 };

	//King pieces positions
	TStaticArray<int32, 2> Kings{ 0 };

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
	TStaticArray<int32, 13> PieceCount{ 0 };

	//Count of all non-pawn pieces
	TStaticArray<int32, 3> BigPieces{ 0 };

	//Count of rooks, queens
	TStaticArray<int32, 3> MajorPieces{ 0 };

	//Count of bishops, knights
	TStaticArray<int32, 3> MinorPieces{ 0 };

	uint64 PosHashKey;

	//Tells which castle is available
	int32 CastlePermission;
	
	//
	TArray<FChessMoveRecord> History;

	/*Some helper arrays to handle 120-tile indexing and 64-tile indexing*/

	TStaticArray<int32, 120> Array120To64Converter{ 0 };
	TStaticArray<int32, 64> Array64To120Converter{ 0 };

	void MakeConverterArray_120To64();

	/****************************************************/

	//For faster move generation
	//TODO add piece list
	//

	/**/

	//See definition on www.chessprogrammingwiki.com
	TArray<int32> BitTable = {
		63, 10, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
		51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
		26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
		58, 20, 37, 17, 36, 8
	};

	TStaticArray<uint64, 64> SetMask{ 0 };
	TStaticArray<uint64, 64> ClearMask{ 0 };

	void MakeBitMasks();
	
	TStaticArray<TStaticArray<uint64, 120>, 13> PieceHashKeys;
	TStaticArray<uint64, 16> CastleHashKeys;
	uint64 SideHashKey;

	void MakeHashKeys();

	//Takes first bit, starting from the least significant bit, returns its index and sets it to zero
	int32 PopBit();

	//Counts and returns number of non-zero bits in bitboard
	int32 CountBits() const;

	//Clears bit to 0 at given tile
	void ClearBit(int32 Tile);

	//Sets bit to 1 at given tile
	void SetBit(int32 Tile);
};
