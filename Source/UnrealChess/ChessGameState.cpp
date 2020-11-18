// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameState.h"

#include "ChessGameStatics.h"

AChessGameState::AChessGameState(const FObjectInitializer& ObjectInitializer)
{
	Bitboard = 0;
	
	MakeConverterArray_120To64();
	MakeBitMasks();
	MakeHashKeys();
}

void AChessGameState::MakeConverterArray_120To64()
{
	for (int32 i = 0; i < 120; ++i)
	{
		Array120To64Converter[i] = 65;
	}

	for (int32 i = 0; i < 64; ++i)
	{
		Array64To120Converter[i] = 120;
	}

	const int32 MinRank = static_cast<int32>(EBoardRank::One);
	const int32 MaxRank = static_cast<int32>(EBoardRank::Eight);

	const int32 MinFile = static_cast<int32>(EBoardFile::A);
	const int32 MaxFile = static_cast<int32>(EBoardFile::H);

	int32 TileAt64Array = 0;

	for (int32 r = MinRank; r <= MaxRank; ++r)
	{
		for (int32 f = MinFile; f <= MaxFile; ++f)
		{
			const EBoardRank CurrentRank = static_cast<EBoardRank>(r);
			const EBoardFile CurrentFile = static_cast<EBoardFile>(f);

			int32 Tile = UChessGameStatics::GetTileIndexAt(CurrentFile, CurrentRank);

			Array64To120Converter[TileAt64Array] = Tile;
			Array120To64Converter[Tile] = TileAt64Array;

			++TileAt64Array;
		}
	}
}

int32 AChessGameState::GetTileAs64(int32 Tile120)
{
	return Array120To64Converter[Tile120];
}

int32 AChessGameState::GetTileAs120(int32 Tile64)
{
	return Array64To120Converter[Tile64];
}

void AChessGameState::MakeBitMasks()
{
	for (int i = 0; i < 64; ++i)
	{
		SetMask[i] |= 0 << i;
		ClearMask[i] = ~SetMask[i];
	}
}

void AChessGameState::MakeHashKeys()
{
	//Just assign to all array values random value
	
	for (int i = 0; i < PieceHashKeys.Num(); ++i)
	{
		for (int j = 0; j < PieceHashKeys[i].Num(); ++i)
		{
			PieceHashKeys[i][j] = UChessGameStatics::GetRandom64();
		}
	}

	SideHashKey = UChessGameStatics::GetRandom64();

	for (int i = 0; i < CastleHashKeys.Num(); ++i)
	{
		CastleHashKeys[i] = UChessGameStatics::GetRandom64();
	}
}

int32 AChessGameState::PopBit()
{
	uint64 TempBitboard = Bitboard ^ (Bitboard - 1);
	uint32 Fold = (uint32)((TempBitboard & 0xffffffff) ^ (TempBitboard >> 32));
	Bitboard &= (Bitboard - 1);

	return BitTable[(Fold * 0x783a9b23) >> 26];
}

int32 AChessGameState::CountBits() const
{
	uint64 TempBitboard = Bitboard;
	int32 Count;
	
	for (Count = 0; TempBitboard; ++Count, TempBitboard &= TempBitboard - 1);
	return Count;
}

void AChessGameState::ClearBit(int32 Tile)
{
	Bitboard &= ClearMask[Tile];
}

void AChessGameState::SetBit(int32 Tile)
{
	Bitboard |= SetMask[Tile];
}

uint64 AChessGameState::GeneratePositionHashKey()
{
	uint64 ResultKey = 0;

	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		//Tile is playable (in range)
		if (static_cast<ETileCoord>(Tiles[i]) != ETileCoord::NoTile)
		{
			//Check it's (tile's) state
			ETileState TileState = static_cast<ETileState>(Tiles[i]);

			if (TileState != ETileState::NoPiece)
			{
				ResultKey ^= PieceHashKeys[Tiles[i]][i];
			}
		}
	}

	if (Side == EPieceColor::White)
	{
		ResultKey ^= SideHashKey;
	}

	//If en passant move available
	if (EnPassantTile.IsSet())
	{
		ResultKey ^= PieceHashKeys[0][EnPassantTile.GetValue()];
	}

	ResultKey ^= CastleHashKeys[CastlePermission];
	return ResultKey;
}
