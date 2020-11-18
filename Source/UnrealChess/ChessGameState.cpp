// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameState.h"

#include "ChessGameStatics.h"

AChessGameState::AChessGameState(const FObjectInitializer& ObjectInitializer)
{
	MakeConverterArray_120To64();
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

			Array64To120Converter[TileAt64Array]	= Tile;
			Array120To64Converter[Tile]				= TileAt64Array;

			++TileAt64Array;
		}
	}
}
