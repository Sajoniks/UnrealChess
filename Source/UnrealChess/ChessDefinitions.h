// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataTableEditor/Private/SRowEditor.h"

#include "ChessDefinitions.generated.h"

UENUM(BlueprintType)
//This enum represents which piece occupies the tile
enum class ETileState : uint8
{
	NoPiece,
	
	WhitePawn,
	WhiteKnight,
	WhiteBishop,
	WhiteRook,
	WhiteQueen,
	WhiteKing,

	BlackPawn,
	BlackKnight,
	BlackBishop,
	BlackRook,
	BlackQueen,
	BlackKing	
};

UENUM(BlueprintType)
enum class EChessPieceRole : uint8
{
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
};

UENUM(BlueprintType)
enum class EChessPieceColor : uint8
{
	Black,
	White
};


UENUM(BlueprintType)
//First coordinate of the board tile
enum class EBoardFile : uint8
{
	A, B, C, D, E, F, G, H, None
};

UENUM(BlueprintType)
//Second coordinate of the board tile
enum class EBoardRank : uint8
{
	One, Two, Three, Four, Five, Six, Seven, Eight, None
};

UENUM(BlueprintType)
//This enum represents chess pieces color group
enum class EPieceColor : uint8
{
	White,
	Black,
	Both,
	NoColor
};


class UNREALCHESS_API FChessPiece
{
	bool bIsBig;
	bool bIsMajor;
	bool bIsMinor;

	int32 Cost;
	EPieceColor Color;

	ETileState State;

public:

	FChessPiece() = default;
	
	FChessPiece(EPieceColor Color, ETileState State, bool bIsBig, bool bIsMaj, bool bIsMin, int32 Cost):
		bIsBig(bIsBig), bIsMajor(bIsMaj), bIsMinor(bIsMin), Cost(Cost), Color(Color), State(State) {}

	bool operator==(const FChessPiece& Other) const;
	bool operator!=(const FChessPiece& Other) const;

	/**Piece code
	 * @return Integer in range [1-12] (1-white pawn, 2-white knight, ...), or 0, if invalid
	 */
	FORCEINLINE int32 GetCode() const;

	//Chess piece value
	FORCEINLINE int32 GetCost() const;

	//Chess state as enum
	FORCEINLINE ETileState GetEnum() const;

	//Chess piece color enum
	FORCEINLINE EPieceColor GetColor() const;

	/**Color code of the piece
	 * @return Integer in range [0-1], if chess is valid, else [2-3] if invalid
	 */
	FORCEINLINE int32 GetColorCode() const;

	FORCEINLINE bool IsBigPiece() const;
	FORCEINLINE bool IsMajorPiece() const;
	FORCEINLINE bool IsMinorPiece() const;
	FORCEINLINE bool IsA(EChessPieceRole Role) const;

	static const FChessPiece& GetPieceFromChar(TCHAR Char);
};

/****Predefined pieces constants****/
/****************************/
extern const FChessPiece EmptyChessPiece;

extern const FChessPiece WhitePawn;
extern const FChessPiece BlackPawn;

extern const FChessPiece WhiteKnight;
extern const FChessPiece BlackKnight;

extern const FChessPiece WhiteBishop;
extern const FChessPiece BlackBishop;

extern const FChessPiece WhiteRook;
extern const FChessPiece BlackRook;

extern const FChessPiece WhiteQueen;
extern const FChessPiece BlackQueen;

extern const FChessPiece BlackKing;
extern const FChessPiece WhiteKing;
/****************************/


UENUM()
//This enum represents tile coordinate
enum class ETileCoord : uint8
{
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8, NoTile
};

class UNREALCHESS_API FTileCoordinate
{
	ETileCoord Coordinate;

	EBoardRank Rank;
	EBoardFile File;

public:

	FTileCoordinate():
		Coordinate(ETileCoord::NoTile), Rank(EBoardRank::None), File(EBoardFile::None) {}

	FTileCoordinate(ETileCoord Combined):
		Coordinate(Combined), Rank(), File()
	{
		int32 Index = (int32)Combined;

		if (FMath::IsWithinInclusive(Index, 21, 28))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 31, 38))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 41, 48))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 51, 58))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 61, 68))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 71, 78))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 81, 88))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}

		if (FMath::IsWithinInclusive(Index, 91, 98))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index);
		}
	}

	FTileCoordinate(EBoardFile File, EBoardRank Rank):
	Coordinate(), Rank(Rank), File(File)
	{
		Coordinate = static_cast<ETileCoord>(21 + (int32)Rank * 10 + (int32)File);
	}

	FORCEINLINE ETileCoord GetEnum() const;
	FORCEINLINE EBoardFile GetFile() const;
	FORCEINLINE EBoardRank GetRank() const;

	FORCEINLINE bool IsValid() const;

	FORCEINLINE int32 ToInt() const;

	static EBoardRank ToRank(int32 Int)
	{
		return static_cast<EBoardRank>(Int);
	}

	static EBoardFile ToFile(int32 Int)
	{
		return static_cast<EBoardFile>(Int);
	}

	static int32 GetMinRankIndex()
	{
		return (int32)EBoardRank::One;
	}

	static int32 GetMaxRankIndex()
	{
		return (int32)EBoardRank::Eight;
	}

	static int32 GetMinFileIndex()
	{
		return(int32)EBoardFile::A;
	}

	static int32 GetMaxFileIndex()
	{
		return (int32)EBoardFile::H;
	}
};

UENUM()
//This enum represents castling type
enum class ECastlingType : uint8
{
	WhiteKing = 1,
	WhiteQueen = 2,
	BlackKing = 4,
	BlackQueen = 8
};
//Binary values of this enum are
//1 0 0 0
//0 1 0 0
//0 0 1 0
//0 0 0 1
//This enum lets us know which castling is available by binary addition
//Example
//0 1 1 1
//White Queen, Black King and Black Queen castling is available