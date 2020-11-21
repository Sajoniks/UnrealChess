// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameState.h"

#include "ChessGameStatics.h"

//TODO
//Think about InitBoard location
AChessGameState::AChessGameState(const FObjectInitializer& ObjectInitializer)
{
	MakeBitMasks();
	MakeHashKeys();

	MakeConverterArray_120To64();
	MakeFilesRanksArrays();
	
	InitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
	UpdateListsMaterial();
}

void AChessGameState::InitBoard(const FString& FEN)
{
	if (!FEN.IsEmpty())
	{
		ResetBoard();

		FString LeftPart, RightPart;
		const FString Str = FEN.TrimStartAndEnd();

		if (Str.Split(" ", &LeftPart, &RightPart))
		{
			//Left part is a board description
			//Right part is a state description

			//Remove whitespace
			//RightPart.TrimStartInline();

			//Firstly parse pieces
			TArray<FString> Parsed;
			if (LeftPart.ParseIntoArray(Parsed, TEXT("/"), true))
			{
				EBoardRank Rank = EBoardRank::Eight;
				
				for (auto&& Row : Parsed)
				{
					EBoardFile File = EBoardFile::A;
					
					for (auto&& Char : Row)
					{//Number of pieces encoded in single character
						int32 Count = 1;

						FChessPiece Piece = FChessPiece::GetPieceFromChar(Char);

						switch (Char)
						{
							//Piece count
						case'1': case '2': case '3': case '4': case '5': case '6': case '7': case '8':
							//Get numeric value
							//Example
							//0 ascii is 48, char ascii is 56 (number 8)
							//56-48 = 8
							Count = Char - L'0';
							break;
						}

						for (int32 i = 0; i < Count; ++i)
						{
							int32 Tile64 = UChessGameStatics::GetTileIndexAt_64(File, Rank);
							int32 Tile120 = GetTileAs120(Tile64);

							Tiles[Tile120].GetState().SetPiece(Piece);
							Tiles[Tile120].SetPosition(File, Rank);

							File = static_cast<EBoardFile>((int32)File + 1);
						}
					}

					Rank = static_cast<EBoardRank>((int32)Rank - 1);
				}

				//Secondly parse state
				if (RightPart.ParseIntoArrayWS(Parsed, nullptr, true))
				{
					//First parsed character is the side which makes move
					Side = Parsed[0].Equals("w") ? EPieceColor::White : EPieceColor::Black;

					if (!Parsed[1].Equals("-"))
					{
						for (auto&& Char : Parsed[1])
						{
							switch(Char)
							{
								case 'K': CastlePermission |= static_cast<int32>(ECastlingType::WhiteKing); break;
								case 'Q': CastlePermission |= static_cast<int32>(ECastlingType::WhiteQueen); break;
								case 'k': CastlePermission |= static_cast<int32>(ECastlingType::BlackKing); break;
								case 'q': CastlePermission |= static_cast<int32>(ECastlingType::BlackQueen); break;

								default:
									break;
							}
						}
					}

					if (!Parsed[2].Equals("-"))
					{
						auto Tile = UChessGameStatics::ParsePositionFromString(Parsed[2]);
						EnPassantTile.Emplace(UChessGameStatics::GetTileIndexAt(Tile.Key, Tile.Value));
					}

					PosHashKey = GeneratePositionHashKey();

				}
			}

			UE_LOG(LogGameState, Display, TEXT("Game state chess board was initialized."));
			UE_LOG(LogGameState, Display, TEXT("Provided FEN: %s"), *FEN);
		}
	}
}

FChessPiece& AChessGameState::GetPieceAtTile(EBoardFile File, EBoardRank Rank)
{
	return Tiles[UChessGameStatics::GetTileIndexAt(File, Rank)].GetState().GetChessPiece();
}


void AChessGameState::BeginPlay()
{
	
}

void AChessGameState::UpdateListsMaterial()
{
	for (auto&& Tile : Tiles)
	{
		//If tile is on board and has a chess piece
		if (Tile.IsOnBoard() && !Tile.IsEmpty())
		{
			const FChessPiece& Piece = Tile.GetPiece();
			int32 ColorCode = Piece.GetColorCode();

			//Increment counters by chess piece type
			if (Piece.IsBigPiece())
			{
				++BigPieces[ColorCode];
			}

			if (Piece.IsMajorPiece())
			{
				++MajorPieces[ColorCode];
			}

			if (Piece.isMinorPiece())
			{
				++MinorPieces[ColorCode];
			}

			int32 PieceCode = Piece.GetCode();
			int32 TileIdx = Tile.GetPosition().ToInt();
			
			Material[ColorCode] += Piece.GetCost();

			//How it works
			//Assume we have first white pawn on the A1 tile
			//So in pseudo-code this will look like
			//
			//CountOf(WhitePawn) = 0;
			//PieceList[WhitePawn][0] = A1;
			//CountOf(WhitePawn) += 1;
			//
			//Next piece is a white pawn on the A2 tile
			//So we have
			//
			//CountOf(WhitePawn) = 1
			//PieceList[WhitePawn][1] = A2
			//CountOf(WhitePawn) += 1;
			//
			PieceList[PieceCode][PieceCount[PieceCode]] =	 TileIdx;
			++PieceCount[PieceCode];

			if (Piece == WhiteKing || Piece == BlackKing)
			{
				Kings[ColorCode] = TileIdx;
			}
		}
	}

	UE_LOG(LogGameState, Display, TEXT("Calculated material:\n%d for whites;\n%d for blacks"),
			Material[0],
			Material[1]
		);
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

	const int32 MinRank = FTileCoordinate::GetMinRankIndex();
	const int32 MaxRank = FTileCoordinate::GetMaxRankIndex();

	const int32 MinFile = FTileCoordinate::GetMinFileIndex();
	const int32 MaxFile = FTileCoordinate::GetMaxFileIndex();

	int32 TileAt64Array = 0;

	for (int32 i = MinRank; i <= MaxRank; ++i)
	{
		for (int32 j = MinFile; j <= MaxFile; ++j)
		{
			const EBoardRank CurrentRank = FTileCoordinate::ToRank(i);
			const EBoardFile CurrentFile = FTileCoordinate::ToFile(j);

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

void AChessGameState::MakeFilesRanksArrays()
{
	for (int32 i = 0; i < 120; ++i)
	{
		BoardFiles[i] = EBoardFile::None;
		BoardRanks[i] = EBoardRank::None;
	}

	const int32 MinRank = FTileCoordinate::GetMinRankIndex();
	const int32 MaxRank = FTileCoordinate::GetMaxRankIndex();

	const int32 MinFile = FTileCoordinate::GetMinFileIndex();
	const int32 MaxFile = FTileCoordinate::GetMaxFileIndex();

	for (int32 i = MinRank; i <= MaxRank; ++i)
	{
		for (int32 j = MinFile; j <= MaxFile; ++j)
		{
			const EBoardRank CurrentRank = FTileCoordinate::ToRank(i);
			const EBoardFile CurrentFile = FTileCoordinate::ToFile(j);

			const int32 Tile = UChessGameStatics::GetTileIndexAt(CurrentFile, CurrentRank);
			BoardFiles[Tile] = CurrentFile;
			BoardRanks[Tile] = CurrentRank;
		}
	}
}

void AChessGameState::MakeBitMasks()
{
	for (int32 i = 0; i < 64; ++i)
	{
		SetMask[i] |= uint64(1) << i;
		ClearMask[i] = ~SetMask[i];
	}
}

void AChessGameState::MakeHashKeys()
{
	//Just assign to all array values random value

	for (int32 i = 0; i < PieceHashKeys.Num(); ++i)
	{
		for (int32 j = 0; j < PieceHashKeys[i].Num(); ++j)
		{
			PieceHashKeys[i][j] = UChessGameStatics::GetRandom64();
		}
	}

	SideHashKey = UChessGameStatics::GetRandom64();

	for (int32 i = 0; i < CastleHashKeys.Num(); ++i)
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
		if (!Tiles[i].IsOnBoard() && !Tiles[i].IsEmpty())
		{
			ResultKey ^= PieceHashKeys[Tiles[i].GetPieceAsInt()][i];
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

void AChessGameState::ResetBoard()
{
	//Reset all tiles to NoTile
	for (int32 i = 0; i < Tiles.Num(); ++i)
	{
		Tiles[i].Reset();
	}

	//Playable tiles set to NoPiece
	for (int32 i = 0; i < 64; ++i)
	{
		const int32 Tile = GetTileAs120(i);
		Tiles[Tile].GetState().SetPiece(EmptyChessPiece);
	}

	//Reset all counters to 0
	for (int32 i = 0; i < 3; ++i)
	{
		BigPieces[i] = 0;
		MajorPieces[i] = 0;
		MinorPieces[i] = 0;

		Pawns[i] = 0;
	}

	for (int32 i = 0; i < PieceCount.Num(); ++i)
	{
		PieceCount[i] = 0;
	}

	Kings[0] = static_cast<int32>(ETileCoord::NoTile);
	Kings[1] = Kings[0];

	Side = EPieceColor::Both;

	EnPassantTile.Reset();

	FiftyMoveCounter = 0;
	Ply = 0;
	HisPly = 0;

	CastlePermission = 0;
	PosHashKey = 0;

	History.Reset();
}
