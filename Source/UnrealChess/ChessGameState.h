// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessDefinitions.h"
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TileCoordinate.h"
#include "ChessMove.h"
#include "ChessMoveRecord.h"
#include "ChessBoardTile.h"

#include "ChessGameState.generated.h"

class AChessboard;
class UDataTable;

UCLASS(CustomConstructor)
class UNREALCHESS_API AChessGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AChessGameState(const FObjectInitializer& ObjectInitializer);

	//Set moving side
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Set")
	void SetMovingSide(EPieceColor NewSide);

	//Get moving side
	UFUNCTION(BlueprintCallable, Category="Get")
	EPieceColor GetSide() const;

	//Init board layout from the FEN string
	void InitBoard(const FString& FEN);

	//Get chess piece at index
	const FChessPiece& GetPieceAtTile(EBoardFile File, EBoardRank Rank) const;

	//Is tile attacked at index
	bool IsTileAttacked(EBoardFile File, EBoardRank Rank, EPieceColor Side);
	
	//Add basic moves
	void AddQuietMove(const FChessMove& Move);
	void AddCaptureMove(const FChessMove& Move);
	void AddEnPassantMove(const FChessMove& Move);

	//Add pawn moves because they are tricky
	void AddWhitePawnCaptureMove(const FTileCoord& From, const FTileCoord& To, const FChessPiece& Captured);
	void AddWhitePawnMove(const FTileCoord& From, const FTileCoord& To);
	void AddBlackPawnCaptureMove(const FTileCoord& From, const FTileCoord& To, const FChessPiece& Captured);
	void AddBlackPawnMove(const FTileCoord& From, const FTileCoord& To);

	//All pieces move generation
	UFUNCTION(NetMulticast, Reliable)
	void GenerateAllMoves();
	
	//Move related functions
	void ClearPiece(const FTileCoord& Coord);
	void AddPiece(const FTileCoord& Coord, const FChessPiece& Piece);
	void MovePiece(const FTileCoord& From, const FTileCoord& To);

	//Main move functions
	//
	//Revert move (Undo)
	void TakeMove();
	//Make move (Do)
	bool MakeMove(const FChessMove& Move);

	//Restart
	void ResetBoard();

	//Get current moves for moving side
	const TArray<FChessMove>& GetMoves() const;

	//
	void BeginPlay() override;

	//Index converters
	//
	int32 GetTileAs64(int32 Tile120);
	int32 GetTileAs120(int32 Tile64);

	//Tile where en passant move is active
	TOptional<int32> EnPassantTile = 0;
	
private:

	//All tiles
	TStaticArray<FChessBoardTile, 120> Tiles{ };

	//Moves
	TArray<FChessMove> Moves;

	//For faster move generation
	//Piece list
	TStaticArray<TStaticArray<FTileCoord, 10>, 13> PieceList{ };
	
	//Pawns locations
	TStaticArray<uint64, 3> Pawns{ 0 };

	//King pieces positions
	TStaticArray<FTileCoord, 2> Kings{ };

	//The side that needs to make a move
	EPieceColor Side = EPieceColor::Both;

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

	//Tells which castle is available
	int32 CastlePermission = 0;
	
	//Move generator helpers
	//
	void GenerateWhitePawnMoves();
	void GenerateBlackPawnMoves();
	void GenerateSlideMoves();
	void GenerateNonSlideMoves();
	void GenerateWhiteCastling();
	void GenerateBlackCastling();

	//Calculate total material
	void UpdateListsMaterial();
	
	//Hashing related functions
	//Will be used in history
	//
	void HashPiece(const FChessPiece& Piece, const FTileCoord& Coord);
	void HashCastle();
	void HashSide();
	void HashEnPassant();

	TStaticArray<TStaticArray<uint64, 120>, 13> PieceHashKeys{ TStaticArray<uint64, 120>{0} };
	TStaticArray<uint64, 16> CastleHashKeys{ 0 };
	uint64 SideHashKey = 0;
	uint64 PosHashKey = 0;

	void MakeHashKeys();
	uint64 GeneratePositionHashKey();

	//Converter arrays
	//
	TStaticArray<int32, 120> Array120To64Converter{ 0 };
	TStaticArray<int32, 64> Array64To120Converter{ 0 };

	//Init converter arrays
	void MakeConverterArray_120To64();

	//Main bitboard
	uint64 Bitboard = 0;

	//Masks
	//
	TStaticArray<uint64, 64> SetMask{ 0 };
	TStaticArray<uint64, 64> ClearMask{ 0 };

	void MakeBitMasks();

	//Takes first bit, starting from the least significant bit, returns its index and sets it to zero
	int32 PopBit();

	//Counts and returns number of non-zero bits in bitboard
	int32 CountBits() const;

	//Clears bit to 0 at given tile
	void ClearBit(uint64& BitBoard, int32 Idx);

	//Sets bit to 1 at given tile
	void SetBit(uint64& BitBoard, int32 Idx);

	
	/*****************Going to the WIP section*****************/

	//Counter that detects 50 move (100 half-move), when game is a draw
	int32 FiftyMoveCounter = 0;
	
	//
	TArray<FChessMoveRecord> History;
	
	//See definition on www.chessprogrammingwiki.com
	TArray<int32> BitTable = {
		63, 10, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34, 61, 29, 2,
		51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35, 62, 31, 40, 4, 49, 5, 52,
		26, 60, 6, 23, 44, 46, 27, 56, 16, 7, 39, 48, 24, 59, 14, 12, 55, 38, 28,
		58, 20, 37, 17, 36, 8
	};
	
	/****************************************************/
};

/* Game Move Encoding */
//Use 28 bits	
/*
0000 0000 0000 0000 0000 0111 1111 -> From				0x7F
0000 0000 0000 0011 1111 1000 0000 -> To					>> 7, 0x7F
0000 0000 0011 1100 0000 0000 0000 -> Captured piece	>> 14, 0XF
0000 0000 0100 0000 0000 0000 0000 -> Is En Passant		0x40000
0000 0000 1000 0000 0000 0000 0000 -> Pawn start			0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece	>> 20, 0xF
0001 0000 0000 0000 0000 0000 0000 -> Is Castling		0x1000000
 */
