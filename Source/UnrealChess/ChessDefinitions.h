// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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
	Both
};

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