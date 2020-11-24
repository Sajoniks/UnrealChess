#pragma once

#include "CoreMinimal.h"
#include "ChessDefinitions.h"

#include "TileCoordinate.generated.h"

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

USTRUCT()
struct UNREALCHESS_API FTileCoord
{
	GENERATED_BODY()
	
private:
	ETileCoord Coordinate;

	EBoardRank Rank;
	EBoardFile File;

public:

	FTileCoord() :
		Coordinate(ETileCoord::NoTile), Rank(EBoardRank::None), File(EBoardFile::None) {}

	FTileCoord(ETileCoord Combined);

	FTileCoord(EBoardFile File, EBoardRank Rank);

	FORCEINLINE ETileCoord GetEnum() const;
	FORCEINLINE EBoardFile GetFile() const;
	FORCEINLINE EBoardRank GetRank() const;

	FORCEINLINE bool IsValid() const;
	FORCEINLINE int32 ToInt() const;
	FORCEINLINE bool operator==(const FTileCoord& Other) const;

	static EBoardRank ToRank(int32 Int);
	static EBoardFile ToFile(int32 Int);
	static int32 GetMinRankIndex();
	static int32 GetMaxRankIndex();
	static int32 GetMinFileIndex();
	static int32 GetMaxFileIndex();
};

ETileCoord FTileCoord::GetEnum() const
{
	return Coordinate;
}

EBoardFile FTileCoord::GetFile() const
{
	return File;
}

EBoardRank FTileCoord::GetRank() const
{
	return Rank;
}

bool FTileCoord::IsValid() const
{
	return Coordinate != ETileCoord::NoTile && Rank != EBoardRank::None && File != EBoardFile::None;
}

int32 FTileCoord::ToInt() const
{
	return (int32)Coordinate;
}

bool FTileCoord::operator==(const FTileCoord& Other) const
{
	if (this == &Other)
		return true;

	return Coordinate == Other.Coordinate;
}