
#include "TileCoordinate.h"

FTileCoord::FTileCoord(ETileCoord Combined):
	Coordinate(Combined), Rank(), File()
{
	if (Combined == ETileCoord::NoTile)
	{
		Rank = EBoardRank::None;
		File = EBoardFile::None;
	}
	else
	{
		int32 Index = (int32)Combined;

		if (FMath::IsWithinInclusive(Index, 21, 28))
		{
			Rank = EBoardRank::One;
			File = static_cast<EBoardFile>(Index - 21);
		}

		if (FMath::IsWithinInclusive(Index, 31, 38))
		{
			Rank = EBoardRank::Two;
			File = static_cast<EBoardFile>(Index - 31);
		}

		if (FMath::IsWithinInclusive(Index, 41, 48))
		{
			Rank = EBoardRank::Three;
			File = static_cast<EBoardFile>(Index - 41);
		}

		if (FMath::IsWithinInclusive(Index, 51, 58))
		{
			Rank = EBoardRank::Four;
			File = static_cast<EBoardFile>(Index - 51);
		}

		if (FMath::IsWithinInclusive(Index, 61, 68))
		{
			Rank = EBoardRank::Five;
			File = static_cast<EBoardFile>(Index - 61);
		}

		if (FMath::IsWithinInclusive(Index, 71, 78))
		{
			Rank = EBoardRank::Six;
			File = static_cast<EBoardFile>(Index - 71);
		}

		if (FMath::IsWithinInclusive(Index, 81, 88))
		{
			Rank = EBoardRank::Seven;
			File = static_cast<EBoardFile>(Index - 81);
		}

		if (FMath::IsWithinInclusive(Index, 91, 98))
		{
			Rank = EBoardRank::Eight;
			File = static_cast<EBoardFile>(Index - 91);
		}
	}
}

FTileCoord::FTileCoord(EBoardFile File, EBoardRank Rank):
	Coordinate(), Rank(Rank), File(File)
{
	Coordinate = static_cast<ETileCoord>(21 + (int32)Rank * 10 + (int32)File);
}

EBoardRank FTileCoord::ToRank(int32 Int)
{
	return static_cast<EBoardRank>(Int);
}

EBoardFile FTileCoord::ToFile(int32 Int)
{
	return static_cast<EBoardFile>(Int);
}

int32 FTileCoord::GetMinRankIndex()
{
	return (int32)EBoardRank::One;
}

int32 FTileCoord::GetMaxRankIndex()
{
	return (int32)EBoardRank::Eight;
}

int32 FTileCoord::GetMinFileIndex()
{
	return (int32)EBoardFile::A;
}

int32 FTileCoord::GetMaxFileIndex()
{
	return (int32)EBoardFile::H;
}
