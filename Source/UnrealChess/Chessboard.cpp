// Fill out your copyright notice in the Description page of Project Settings.

#include "Chessboard.h"
#include "MoveRules.h"
#include "Templates/UnrealTemplate.h"

UChessboard::UChessboard(const FObjectInitializer & ObjectInitializer)
{
	ReloadBoard(EGenerationType::DEFAULT);
}

void UChessboard::ReloadBoard(EGenerationType GenerationType)
{
	switch (GenerationType)
	{
		case EGenerationType::DEFAULT:
		{
			FString DefaultBoard[8][8] =
			{
				{ { "R" },{ "H" },{ "B" },{ "Q" },{ "K" },{ "B" },{ "H" },{ "R" } },
				{ { "P" },{ "P" },{ "P" },{ "P" },{ "P" },{ "P" },{ "P" },{ "P" } },
				{ { "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" } },
				{ { "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" } },
				{ { "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" } },
				{ { "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" },{ "" } },
				{ { "p" },{ "p" },{ "p" },{ "p" },{ "p" },{ "p" },{ "p" },{ "p" } },
				{ { "r" },{ "h" },{ "b" },{ "q" },{ "k" },{ "b" },{ "h" },{ "r" } },
			};
			ConvertStringToBitboards(DefaultBoard, WP, WH, WB, WR, WK, WQ, BP, BH, BB, BR, BK, BQ);
		}
			break;

		case EGenerationType::RANDOM:
		{

		}
			break;

		case EGenerationType::CHESS960:
		{

		}
			break;
	}
}

void UChessboard::ConvertStringToBitboards(const FString(&Board)[8][8], int64 & WP, int64 & WH, int64 & WB, int64 & WR, int64 & WK, int64 & WQ, int64 & BP, int64 & BH, int64 & BB, int64 & BR, int64 & BK, int64 & BQ)
{
	
}

int64 UChessboard::ConvertStringToBit(const FString & String)
{
	int64 Base = 1;
	int64 Result = 0;

	for (int i = String.Len() - 1; i > 0; --i)
	{
		Result += Base*String[i];
		Base *= 2;
	}

	Result += (String[0] == '0' ? Base : -Base);

	return Result;
}

