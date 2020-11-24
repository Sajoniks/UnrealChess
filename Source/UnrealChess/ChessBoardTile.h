#pragma once

#include "CoreMinimal.h"
#include "ChessDefinitions.h"
#include "TileCoordinate.h"

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
	FTileCoord Coords;

	//Castle permission value
	int32 CastlePermission = 15;

public:

	FChessBoardTile() = default;
	FChessBoardTile(int32 Value) :
		State(), Coords(ETileCoord::NoTile), CastlePermission(Value)
	{}

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
	FORCEINLINE const FTileCoord& GetPosition() const
	{
		return Coords;
	}

	FORCEINLINE void SetPosition(const FTileCoord& Coordinate)
	{
		Coords = Coordinate;
	}

	FORCEINLINE void SetPosition(const EBoardFile& File, const EBoardRank& Rank)
	{
		SetPosition({ File, Rank });
	}

	FORCEINLINE int32 GetCastlePermission() const
	{
		return CastlePermission;
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