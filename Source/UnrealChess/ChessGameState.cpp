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
			RightPart.TrimStartInline();

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

						//Piece encoded in character
						ETileState State = ETileState::NoPiece;

						switch (Char)
						{
							//White pieces
						case 'K': State = ETileState::WhiteKing; break;
						case 'Q': State = ETileState::WhiteQueen; break;
						case 'R': State = ETileState::WhiteRook; break;
						case 'B': State = ETileState::WhiteBishop; break;
						case 'N': State = ETileState::WhiteKnight; break;
						case 'P': State = ETileState::WhitePawn; break;

							//Black pieces
						case 'k': State = ETileState::BlackKing; break;
						case 'q': State = ETileState::BlackQueen; break;
						case 'r': State = ETileState::BlackRook; break;
						case 'b': State = ETileState::BlackBishop; break;
						case 'n': State = ETileState::BlackKnight; break;
						case 'p': State = ETileState::BlackPawn; break;

							//Piece count
						case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
							//Get numeric value
							//Example
							//0 ascii is 48, char ascii is 56 (number 8)
							//56-48 = 8
							Count = Char - L'0';
							break;

						default:
							UE_LOG(LogGameState, Error, TEXT("Invalid FEN provided, board generation is impossible"));
							return;
						}

						for (int32 i = 0; i < Count; ++i)
						{
							int32 Tile64 = (int32)Rank * 8 + (int32)File;
							int32 Tile120 = GetTileAs120(Tile64);

							if (State != ETileState::NoPiece)
							{
								//TODO
								Tiles[Tile120] = (int32)State;
							}

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
		}
	}
}

void AChessGameState::BeginPlay()
{
	InitBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

int32& AChessGameState::GetTileAs64(int32 Tile120)
{
	return Array120To64Converter[Tile120];
}

int32& AChessGameState::GetTileAs120(int32 Tile64)
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

void AChessGameState::ResetBoard()
{
	//Reset all tiles to NoTile
	for (int i = 0; i < Tiles.Num(); ++i)
	{
		Tiles[i] = static_cast<int32>(ETileCoord::NoTile);
	}

	//Playable tiles set to NoPiece
	for (int i = 0; i < 64; ++i)
	{
		GetTileAs120(i) = static_cast<int32>(ETileState::NoPiece);
	}

	//Reset all counters to 0
	for (int i = 0; i < 3; ++i)
	{
		BigPieces[i] = 0;
		MajorPieces[i] = 0;
		MinorPieces[i] = 0;

		Pawns[i] = 0;
	}

	for (int i = 0; i < PieceCount.Num(); ++i)
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
