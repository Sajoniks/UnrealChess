// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessDefinitions.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ChessMoveRecord.h"

#include "ChessGameState.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FChessPieceCost : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Cost;
};

//TODO Try refactoring some field into player state
//TODO JUST REFACTOR THIS CRAP

class FChessTileState
{
	FChessPiece ChessPiece;

public:

	FChessTileState() = default;

	FORCEINLINE FChessPiece& GetChessPiece() 
	{
		return ChessPiece;
	}

	FORCEINLINE void SetPiece(const FChessPiece& Piece) 
	{
		ChessPiece = Piece;
	}

	FORCEINLINE void ClearTile()
	{
		ChessPiece = EmptyChessPiece;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return ChessPiece == EmptyChessPiece;
	}
};

class FChessBoardTile
{
	//State of tile
	FChessTileState State;

	//Coords
	FTileCoordinate Coords;

public:

	FChessBoardTile() = default;

	FORCEINLINE void Reset()
	{
		State.ClearTile();
		Coords = {};
	}

	FORCEINLINE FChessTileState& GetState()
	{
		return State;
	}

	//Get combined tile position
	FORCEINLINE FTileCoordinate& GetPosition()
	{
		return Coords;
	}

	FORCEINLINE void SetPosition(const FTileCoordinate& Coordinate)
	{
		Coords = Coordinate;
	}

	FORCEINLINE void SetPosition(const EBoardFile& File, const EBoardRank& Rank)
	{
		SetPosition({File, Rank});
	}

	//Converted file coordinate to int32
	FORCEINLINE int32 GetFileAsInt() const { return (int32)Coords.GetFile(); }

	//Converted rank coordinate to int32
	FORCEINLINE int32 GetRankAsInt() const { return (int32)Coords.GetRank(); }

	//Piece code
	FORCEINLINE int32 GetPieceAsInt() { return State.GetChessPiece().GetCode(); }

	//Color code
	FORCEINLINE int32 GetColorAsInt(){ return State.GetChessPiece().GetColorCode(); }

	//Check if tile has given Piece (not empty and equal to it)
	FORCEINLINE bool HasPiece(const FChessPiece& Piece)
	{
		return !IsOffboard() && !IsEmpty() && State.GetChessPiece() == Piece;
	}

	//Check if this tile is not playable
	FORCEINLINE bool IsOffboard() const
	{
		return Coords.IsValid();
	}

	FORCEINLINE bool IsEmpty() const
	{
		return State.IsEmpty();
	}
};

/**
 * 
 */
UCLASS(CustomConstructor)
class UNREALCHESS_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()

	UPROPERTY()
	UDataTable* CostDT;
	
public:

	AChessGameState(const FObjectInitializer& ObjectInitializer);

	void InitBoard(const FString& FEN);

	//UFUNCTION(BlueprintCallable, Category="Get")
	FChessPiece& GetPieceAtTile(EBoardFile File, EBoardRank Rank);

	void BeginPlay() override;
	
private:

	//
	uint64 Bitboard = 0;

	//All tiles
	TStaticArray<FChessBoardTile, 120> Tiles{ };

	//Pawns locations
	TStaticArray<uint64, 3> Pawns{ 0 };

	//King pieces positions
	TStaticArray<int32, 2> Kings{ 0 };

	//The side that needs to make a move
	//TODO replace this enum
	EPieceColor Side = EPieceColor::Both;
	
	//Tile where en passant move is active
	TOptional<int32> EnPassantTile = 0;

	//Counter that detects 50 move (100 half-move), when game is a draw
	int32 FiftyMoveCounter = 0;
	
	//White half-moves count
	int32 Ply = 0;
	
	//Black half-moves count
	int32 HisPly = 0;

	//Total count of each chess piece, including empty tiles
	//13 is a count of chess piece types by each color plus empty tiles count
	TStaticArray<int32, 13> PieceCount{ 0 };

	//Count of all non-pawn pieces
	TStaticArray<int32, 2> BigPieces{ 0 };

	//Count of rooks, queens
	TStaticArray<int32, 2> MajorPieces{ 0 };

	//Count of bishops, knights
	TStaticArray<int32, 2> MinorPieces{ 0 };

	//
	TStaticArray<int32, 2> Material;

	//Hard-coded values that defines piece majority
	TArray<bool, TFixedAllocator<13>> IsBig { false, false, true, true, true, true, true, false, true, true, true, true, true };
	TArray<bool, TFixedAllocator<13>> IsMaj { false, false, false, false, true, true, true, false, false, false, true, true, true };
	TArray<bool, TFixedAllocator<13>> IsMin { false, false, true, true, false, false, false, false, true, true, false, false, false };

	//Hard-coded pieces cost
	TArray<int32, TFixedAllocator<13>> PieceCost { 0, 100, 325, 325, 550, 1000, 50000, 100, 325, 325, 550, 1000, 50000 };
	
	//Hard-coded colors
	TArray<EPieceColor, TFixedAllocator<13>> PieceColor {
		
		EPieceColor::Both,
		
		EPieceColor::White, EPieceColor::White, EPieceColor::White, EPieceColor::White, EPieceColor::White, EPieceColor::White,
		EPieceColor::Black, EPieceColor::Black, EPieceColor::Black, EPieceColor::Black, EPieceColor::Black, EPieceColor::Black
		
	};

	
	
	void UpdateListsMaterial();

	uint64 PosHashKey = 0;

	//Tells which castle is available
	int32 CastlePermission = 0;
	
	//
	TArray<FChessMoveRecord> History;

	/*Some helper arrays to handle 120-tile indexing and 64-tile indexing*/
	/*Too low level, need refactor TODO*/
	
	TStaticArray<int32, 120> Array120To64Converter{ 0 };
	TStaticArray<int32, 64> Array64To120Converter{ 0 };

	void MakeConverterArray_120To64();

	int32 GetTileAs64(int32 Tile120);
	int32 GetTileAs120(int32 Tile64);

	/****************************************************/

	//For faster move generation
	//Piece list
	TStaticArray<TStaticArray<int32, 13>, 10> PieceList{ TStaticArray<int32, 13>{0} };


	//Cached values of ranks and files
	TStaticArray<EBoardFile, 120> BoardFiles;
	TStaticArray<EBoardRank, 120> BoardRanks;

	void MakeFilesRanksArrays();
	
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
	
	TStaticArray<TStaticArray<uint64, 120>, 13> PieceHashKeys{TStaticArray<uint64, 120>{0}};
	TStaticArray<uint64, 16> CastleHashKeys{0};
	uint64 SideHashKey = 0;

	void MakeHashKeys();

	//Takes first bit, starting from the least significant bit, returns its index and sets it to zero
	int32 PopBit();

	//Counts and returns number of non-zero bits in bitboard
	int32 CountBits() const;

	//Clears bit to 0 at given tile
	void ClearBit(int32 Tile);

	//Sets bit to 1 at given tile
	void SetBit(int32 Tile);

	uint64 GeneratePositionHashKey();

	/**/

	void ResetBoard();
};
