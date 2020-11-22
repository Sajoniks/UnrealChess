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

	FORCEINLINE const FChessPiece& GetChessPiece() const
	{
		return ChessPiece;
	}

	FORCEINLINE void SetPiece(const FChessPiece& Piece) 
	{
		ChessPiece = Piece;
	}

	FORCEINLINE void ClearTile()
	{
		ChessPiece = GEmptyChessPiece;
	}

	FORCEINLINE bool IsEmpty() const
	{
		return ChessPiece == GEmptyChessPiece;
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

	FORCEINLINE const FChessTileState& GetState() const
	{
		return State;
	}

	FORCEINLINE void SetPiece(const FChessPiece& Piece)
	{
		State.SetPiece(Piece);
	}

	//Get combined tile position
	FORCEINLINE const FTileCoordinate& GetPosition() const
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
	FORCEINLINE int32 GetPieceAsInt() const { return State.GetChessPiece().GetCode(); }

	//Color code
	FORCEINLINE int32 GetColorAsInt() const { return State.GetChessPiece().GetColorCode(); }

	//Check if tile has given Piece (not empty and equal to it)
	FORCEINLINE bool HasPiece(const FChessPiece& Piece) const
	{
		return !IsOnBoard() && !IsEmpty() && State.GetChessPiece() == Piece;
	}

	FORCEINLINE const FChessPiece& GetPiece() const
	{
		return State.GetChessPiece();
	}

	//Check if this tile is not playable
	FORCEINLINE bool IsOnBoard() const
	{
		return Coords.IsValid();
	}

	FORCEINLINE bool IsEmpty() const
	{
		return State.IsEmpty();
	}
};

class UNREALCHESS_API FChessMove
{
	int32 Move = 0;
	int32 Score = 0;

public:

	static const int32 FLAG_CastlingMove = 0x1000000;
	static const int32 FLAG_EnPassantMove = 0x40000;
	static const int32 FLAG_PawnStartMove = 0x80000;
	//static const int32 FLAG_PromotionMove = 0x1000000;

	FChessMove(
		const FTileCoordinate& From, 
		const FTileCoordinate& To, 
		const FChessPiece* Captured, 
		const FChessPiece* Promoted,
		int32 Flags = 0
	)
	{
		int32 PromotedCode = (Promoted) ? Promoted->GetCode() : 0;
		int32 CapturedCode = (Captured) ? Captured->GetCode() : 0;
		
		Move = (
				 From.ToInt() | 
				(To.ToInt() << 7) | 
				(CapturedCode << 14) | 
				(PromotedCode << 20) |
				(Flags)
			);
	}

	int32 GetFromTileIndex()	const { return Move & 0x3F; }
	int32 GetToTileIndex()	const { return (Move >> 7) & 0x3F; }
	int32 GetCapturedPiece()	const { return (Move >> 14) & 0xF; }
	int32 GetPromotedPiece()	const { return (Move >> 20) & 0xF; }

	bool IsEnPassantMove()	const { return Move & 0x40000; }
	bool IsPawnStartMove()	const { return Move & 0x80000; }
	bool IsCastlingMove()		const { return Move & 0x1000000; }
};

/**
 * 
 */
UCLASS(CustomConstructor)
class UNREALCHESS_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AChessGameState(const FObjectInitializer& ObjectInitializer);

	void InitBoard(const FString& FEN);

	//
	const FChessPiece& GetPieceAtTile(EBoardFile File, EBoardRank Rank) const;

	//
	bool IsTileAttacked(EBoardFile File, EBoardRank Rank, EPieceColor Side);
	
	//
	void AddQuietMove(const FChessMove& Move);
	void AddCaptureMove(const FChessMove& Move);
	void AddEnPassantMove(const FChessMove& Move);

	//
	void AddWhitePawnCaptureMove(const FTileCoordinate& From, const FTileCoordinate& To, const FChessPiece& Captured);
	void AddWhitePawnMove(const FTileCoordinate& From, const FTileCoordinate& To);
	void AddBlackPawnCaptureMove(const FTileCoordinate& From, const FTileCoordinate& To, const FChessPiece& Captured);
	void AddBlackPawnMove(const FTileCoordinate& From, const FTileCoordinate& To);

	void GenerateAllMoves();
	
	void BeginPlay() override;
	
private:

	void GenerateWhitePawnMoves();
	void GenerateBlackPawnMoves();
	void GenerateSlideMoves();
	void GenerateNonSlideMoves();
	void GenerateWhiteCastling();
	void GenerateBlackCastling();
	
	//
	uint64 Bitboard = 0;

	//All tiles
	TStaticArray<FChessBoardTile, 120> Tiles{ };

	//Pawns locations
	TStaticArray<uint64, 3> Pawns{ 0 };

	//King pieces positions
	TStaticArray<int32, 2> Kings{ 0 };

	//The side that needs to make a move
	EPieceColor Side = EPieceColor::Both;
	
	//Tile where en passant move is active
	TOptional<int32> EnPassantTile = 0;

	//Counter that detects 50 move (100 half-move), when game is a draw
	int32 FiftyMoveCounter = 0;
	
	//TODO move fields to the player state
	
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

	//Total values of pieces by color
	TStaticArray<int32, 2> Material;

	void UpdateListsMaterial();

	uint64 PosHashKey = 0;

	//Tells which castle is available
	int32 CastlePermission = 0;
	
	//
	TArray<FChessMoveRecord> History;

	//
	TArray<FChessMove> Moves;

	/*Some helper arrays to handle 120-tile indexing and 64-tile indexing*/
	/*Too low level, need refactor TODO*/
	
	TStaticArray<int32, 120> Array120To64Converter{ 0 };
	TStaticArray<int32, 64> Array64To120Converter{ 0 };

	void MakeConverterArray_120To64();

	int32 GetTileAs64(int32 Tile120);
	int32 GetTileAs120(int32 Tile64);

	/****************************************************/

	//Move directions for pieces
	//TODO
	TArray<int32, TFixedAllocator<8>> KnightDir = {-8, -19, -21, -12, 8, 10, 21, 12};
	TArray<int32, TFixedAllocator<4>> RookDir = { -1, -10, 1, 10 };
	TArray<int32, TFixedAllocator<4>> BishopDir = { -9, -11, 11, 9 };
	TArray<int32, TFixedAllocator<8>> KingDir = { -1, -10, 1, 10, -9, -11, 11, 9 };
	
	//For faster move generation
	//Piece list
	TStaticArray<TStaticArray<FTileCoordinate, 10>, 13> PieceList{ };

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

/* Game Move Encoding */
//Use 28 bits	
/*
0000 0000 0000 0000 0000 0111 1111 -> From				0x3F
0000 0000 0000 0011 1111 1000 0000 -> To					>> 7, 0x3F
0000 0000 0011 1100 0000 0000 0000 -> Captured piece	>> 14, 0XF
0000 0000 0100 0000 0000 0000 0000 -> Is En Passant		0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn start			0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece	>> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Is Castling		0x1000000
 */
