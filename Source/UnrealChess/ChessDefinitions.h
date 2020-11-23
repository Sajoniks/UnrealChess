// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

/*Basic wrapper over ETileState enum*/
class UNREALCHESS_API FChessPiece
{
	//Fast and lightweight chess piece data structure

	//Is a big piece (non-pawn)
	const static TArray<bool, TFixedAllocator<13>> BigPieces;

	//Is a major piece (rook or queen)
	const static TArray<bool, TFixedAllocator<13>> MajorPieces;

	//Is a minor piece (knight or bishop)
	const static TArray<bool, TFixedAllocator<13>> MinorPieces;

	//Pieces cost
	const static TArray<int32, TFixedAllocator<13>> PiecesCost;

	//Pieces move directions
	const static TArray<TArray<int32>, TFixedAllocator<13>> MoveDirections;

	//State
	ETileState State;

	//Array of white sliding pieces (rook, queen, bishop)
	static const TArray<FChessPiece> WSlidingPieces;

	//Array of black sliding pieces (rook, queen, bishop)
	static const TArray<FChessPiece> BSlidingPieces;

	//Array of white non-sliding pieces (knight, king)
	static const TArray<FChessPiece> WNonSlidingPieces;

	//Array of black non-sliding pieces (knight, king)
	static const TArray<FChessPiece> BNonSlidingPieces;

public:

	FChessPiece() = default;
	
	FChessPiece(ETileState State):
		State(State) {}

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
	
	FORCEINLINE const TArray<int32>& GetMoveDirections() const;
	
	static const TArray<FChessPiece>& GetSlidingPiecesByColor(EPieceColor Color);
	static const TArray<FChessPiece>& GetNonSlidingPiecesByColor(EPieceColor Color);
	
	static const FChessPiece& GetPieceFromChar(TCHAR Char);
	static const FChessPiece& GetPieceFromCode(int32 Code);
};

/****Predefined pieces constants****/
/****************************/
extern const FChessPiece GEmptyChessPiece;

extern const FChessPiece GWhitePawn;
extern const FChessPiece GBlackPawn;

extern const FChessPiece GWhiteKnight;
extern const FChessPiece GBlackKnight;

extern const FChessPiece GWhiteBishop;
extern const FChessPiece GBlackBishop;

extern const FChessPiece GWhiteRook;
extern const FChessPiece GBlackRook;

extern const FChessPiece GWhiteQueen;
extern const FChessPiece GBlackQueen;

extern const FChessPiece GBlackKing;
extern const FChessPiece GWhiteKing;
/****************************/

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