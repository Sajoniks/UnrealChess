// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameState.h"
#include "ChessGameStatics.h"
#include "Chessboard.h"

AChessGameState::AChessGameState(const FObjectInitializer& ObjectInitializer)
{
	//Update castling permissions
	Tiles[FTileCoord{ETileCoord::A1}.ToInt()] = {13};
	Tiles[FTileCoord{ETileCoord::E1}.ToInt()] = {12};
	Tiles[FTileCoord{ETileCoord::H1}.ToInt()] = {14};

	Tiles[FTileCoord{ETileCoord::A8}.ToInt()] = {7};
	Tiles[FTileCoord{ETileCoord::E8}.ToInt()] = {3};
	Tiles[FTileCoord{ETileCoord::H8}.ToInt()] = {11};

	MakeBitMasks();
	MakeHashKeys();

	MakeConverterArray_120To64();
}

void AChessGameState::SetMovingSide_Implementation(EPieceColor NewSide)
{
	Side = NewSide;
}

EPieceColor AChessGameState::GetSide() const
{
	return Side;
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
			//
			//Firstly parse pieces
			TArray<FString> Parsed;
			if (LeftPart.ParseIntoArray(Parsed, TEXT("/"), true))
			{
				EBoardRank Rank = EBoardRank::Eight;

				for (auto&& Row : Parsed)
				{
					EBoardFile File = EBoardFile::A;

					for (auto&& Char : Row)
					{
						//Number of pieces encoded in single character
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

							Tiles[Tile120].SetPiece(Piece);
							Tiles[Tile120].SetPosition(File, Rank);

							File = static_cast<EBoardFile>(static_cast<int32>(File) + 1);
						}
					}

					Rank = static_cast<EBoardRank>(static_cast<int32>(Rank) - 1);
				}

				//Secondly parse state
				if (RightPart.ParseIntoArrayWS(Parsed, nullptr, true))
				{
					//First parsed character is the side which makes move
					Parsed[0].Equals("w") ? SetMovingSide(EPieceColor::White) : SetMovingSide(EPieceColor::Black);

					if (!Parsed[1].Equals("-"))
					{
						for (auto&& Char : Parsed[1])
						{
							switch (Char)
							{
							case 'K': CastlePermission |= static_cast<int32>(ECastlingType::WhiteKing);
								break;
							case 'Q': CastlePermission |= static_cast<int32>(ECastlingType::WhiteQueen);
								break;
							case 'k': CastlePermission |= static_cast<int32>(ECastlingType::BlackKing);
								break;
							case 'q': CastlePermission |= static_cast<int32>(ECastlingType::BlackQueen);
								break;

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


			UpdateListsMaterial();
			GenerateAllMoves();
		}
	}
}

const FChessPiece& AChessGameState::GetPieceAtTile(EBoardFile File, EBoardRank Rank) const
{
	return Tiles[UChessGameStatics::GetTileIndexAt(File, Rank)].GetState().GetChessPiece();
}

bool AChessGameState::IsTileAttacked(EBoardFile File, EBoardRank Rank, EPieceColor MovingSide)
{
	const int32 TileIdx = UChessGameStatics::GetTileIndexAt(File, Rank);

	//Check pawns attacking
	if (MovingSide == EPieceColor::White)
	{
		if (
			Tiles[TileIdx - 11].GetPiece() == GWhitePawn ||
			Tiles[TileIdx - 9].GetPiece() == GWhitePawn
		)
		{
			return true;
		}
	}
	else
	{
		if (
			Tiles[TileIdx + 11].GetPiece() == GBlackPawn ||
			Tiles[TileIdx + 9].GetPiece() == GBlackPawn
		)
		{
			return true;
		}
	}

	//Check knights attacking
	for (auto&& Direction : GWhiteKnight.GetMoveDirections())
	{
		const FChessPiece& Piece = Tiles[TileIdx + Direction].GetPiece();

		if (Piece.IsA(EChessPieceRole::Knight) &&
			Piece.GetColor() == MovingSide
		)
		{
			return true;
		}
	}

	//Check rooks and queens attacking
	for (auto&& Direction : GWhiteRook.GetMoveDirections())
	{
		int32 TileTemp = TileIdx + Direction;
		FChessBoardTile Tile = Tiles[TileTemp];

		while (Tile.IsOnBoard())
		{
			if (!Tile.IsEmpty())
			{
				const FChessPiece& Piece = Tile.GetPiece();

				if (
					(Piece.IsA(EChessPieceRole::Rook) ||
						Piece.IsA(EChessPieceRole::Queen)) &&
					Piece.GetColor() == MovingSide
				)
				{
					return true;
				}

				break;
			}

			TileTemp += Direction;
			Tile = Tiles[TileTemp];
		}
	}

	//Check for bishops and queens attacking
	//Same as for rooks and queens but other directions are being tested
	for (auto&& Direction : GWhiteBishop.GetMoveDirections())
	{
		int32 TileTemp = TileIdx + Direction;
		FChessBoardTile Tile = Tiles[TileTemp];

		while (Tile.IsOnBoard())
		{
			if (!Tile.IsEmpty())
			{
				const FChessPiece& Piece = Tile.GetPiece();
				if (
					(Piece.IsA(EChessPieceRole::Bishop) ||
						Piece.IsA(EChessPieceRole::Queen)) &&
					Piece.GetColor() == MovingSide
				)
				{
					return true;
				}

				break;
			}

			TileTemp += Direction;
			Tile = Tiles[TileTemp];
		}
	}

	//Check for kings attacking
	for (auto&& Direction : GWhiteKing.GetMoveDirections())
	{
		const FChessPiece& Piece = Tiles[TileIdx + Direction].GetPiece();
		if (Piece.IsA(EChessPieceRole::King) && Piece.GetColor() == MovingSide)
		{
			return true;
		}
	}

	//Tile is not attacked
	return false;
}

void AChessGameState::AddQuietMove(const FChessMove& Move)
{
	Moves.Emplace(Move);
}

void AChessGameState::AddCaptureMove(const FChessMove& Move)
{
	Moves.Emplace(Move);
}

void AChessGameState::AddEnPassantMove(const FChessMove& Move)
{
	Moves.Emplace(Move);
}

void AChessGameState::AddWhitePawnCaptureMove(const FTileCoord& From, const FTileCoord& To,
                                              const FChessPiece& Captured)
{
	if (From.GetRank() == EBoardRank::Seven)
	{
		AddCaptureMove({From, To, &Captured, &GWhiteQueen});
		AddCaptureMove({From, To, &Captured, &GWhiteRook});
		AddCaptureMove({From, To, &Captured, &GWhiteBishop});
		AddCaptureMove({From, To, &Captured, &GWhiteKnight});
	}
	else
	{
		AddCaptureMove({From, To, &Captured, nullptr});
	}
}

void AChessGameState::AddWhitePawnMove(const FTileCoord& From, const FTileCoord& To)
{
	if (From.GetRank() == EBoardRank::Seven)
	{
		AddQuietMove({From, To, nullptr, &GWhiteQueen});
		AddQuietMove({From, To, nullptr, &GWhiteRook});
		AddQuietMove({From, To, nullptr, &GWhiteBishop});
		AddQuietMove({From, To, nullptr, &GWhiteKnight});
	}
	else
	{
		AddQuietMove({From, To, nullptr, nullptr});
	}
}

void AChessGameState::AddBlackPawnCaptureMove(const FTileCoord& From, const FTileCoord& To,
                                              const FChessPiece& Captured)
{
	if (From.GetRank() == EBoardRank::Two)
	{
		AddCaptureMove({From, To, &Captured, &GBlackQueen});
		AddCaptureMove({From, To, &Captured, &GBlackRook});
		AddCaptureMove({From, To, &Captured, &GBlackBishop});
		AddCaptureMove({From, To, &Captured, &GBlackKnight});
	}
	else
	{
		AddCaptureMove({From, To, &Captured, nullptr});
	}
}

void AChessGameState::AddBlackPawnMove(const FTileCoord& From, const FTileCoord& To)
{
	if (From.GetRank() == EBoardRank::Two)
	{
		AddQuietMove({From, To, nullptr, &GBlackQueen});
		AddQuietMove({From, To, nullptr, &GBlackRook});
		AddQuietMove({From, To, nullptr, &GBlackBishop});
		AddQuietMove({From, To, nullptr, &GBlackKnight});
	}
	else
	{
		AddQuietMove({From, To, nullptr, nullptr});
	}
}

void AChessGameState::GenerateAllMoves_Implementation()
{
	Moves.Reset();

	if (Side != EPieceColor::NoColor && Side != EPieceColor::Both)
	{
		if (Side == EPieceColor::White)
		{
			GenerateWhitePawnMoves();
			GenerateWhiteCastling();
		}
		else
		{
			GenerateBlackPawnMoves();
			GenerateBlackCastling();
		}


		GenerateSlideMoves();
		GenerateNonSlideMoves();

		UE_LOG(LogGameState, Display, TEXT("Generated %d moves for side %s"), Moves.Num(),
		       *UEnum::GetValueAsString(Side));

		CheckKingState();
	}
}

void AChessGameState::ClearPiece(const FTileCoord& Coord)
{
	check(Coord.IsValid());

	int32 Idx = Coord.ToInt();

	FChessPiece Piece = Tiles[Idx].GetPiece();
	check(Piece != GEmptyChessPiece);

	int32 ColorCode = Piece.GetColorCode();

	HashPiece(Piece, Coord);

	Tiles[Idx].SetPiece(GEmptyChessPiece);
	Material[ColorCode] -= Piece.GetCost();

	if (Piece.IsBigPiece())
	{
		--BigPieces[ColorCode];

		if (Piece.IsMajorPiece())
		{
			--MajorPieces[ColorCode];
		}

		if (Piece.IsMinorPiece())
		{
			--MinorPieces[ColorCode];
		}
	}
	else
	{
		//Update bitboard for color
		ClearBit(Pawns[ColorCode], GetTileAs64(Coord.ToInt()));

		//Update bitboard of combined colors
		ClearBit(Pawns[2], GetTileAs64(Coord.ToInt()));
	}

	int32 PieceCode = Piece.GetCode();
	int32 Count = PieceCount[PieceCode];
	int32 TempCount = -1;

	//TODO
	for (int32 i = 0; i < Count; ++i)
	{
		if (PieceList[PieceCode][i] == Coord)
		{
			TempCount = i;
			break;
		}
	}

	check(TempCount != -1);

	Count = --PieceCount[PieceCode];
	PieceList[PieceCode][TempCount] = PieceList[PieceCode][Count];
}

void AChessGameState::AddPiece(const FTileCoord& Coord, const FChessPiece& Piece)
{
	check(Coord.IsValid());
	check(Piece != GEmptyChessPiece);

	int32 ColorCode = Piece.GetColorCode();

	HashPiece(Piece, Coord);

	Tiles[Coord.ToInt()].SetPiece(Piece);

	if (Piece.IsBigPiece())
	{
		++BigPieces[ColorCode];

		if (Piece.IsMajorPiece())
		{
			++MajorPieces[ColorCode];
		}

		if (Piece.IsMinorPiece())
		{
			++MinorPieces[ColorCode];
		}
	}
	else
	{
		//Set bit to the current's side pawns bitboard
		SetBit(Pawns[ColorCode], GetTileAs64(Coord.ToInt()));

		//And to the combined bitboard
		SetBit(Pawns[2], GetTileAs64(Coord.ToInt()));
	}

	Material[ColorCode] += Piece.GetCode();

	int32 PieceCode = Piece.GetCode();
	int32 Count = PieceCount[PieceCode];

	PieceList[PieceCode][Count] = Coord;
	PieceCount[PieceCode]++;
}

void AChessGameState::MovePiece(const FTileCoord& From, const FTileCoord& To)
{
	check(From.IsValid());
	check(To.IsValid());

	FChessPiece Piece = Tiles[From.ToInt()].GetPiece();
	check(Piece != GEmptyChessPiece);

	int32 FromIdx = From.ToInt();
	int32 ToIdx = To.ToInt();
	int32 ColorCode = Piece.GetColorCode();
	int32 PieceCode = Piece.GetCode();
	int32 Count = PieceCount[PieceCode];

	HashPiece(Piece, From);
	Tiles[FromIdx].SetPiece(GEmptyChessPiece);

	HashPiece(Piece, To);
	Tiles[ToIdx].SetPiece(Piece);

	if (!Piece.IsBigPiece())
	{
		ClearBit(Pawns[ColorCode], GetTileAs64(FromIdx));
		ClearBit(Pawns[2], GetTileAs64(FromIdx));

		SetBit(Pawns[ColorCode], GetTileAs64(ToIdx));
		SetBit(Pawns[2], GetTileAs64(ToIdx));
	}

	bool bFound = false;

	for (int32 i = 0; i < Count; ++i)
	{
		if (PieceList[PieceCode][i] == From)
		{
			PieceList[PieceCode][i] = To;
			bFound = true;
			break;
		}
	}

	//Piece must move!
	check(bFound);
}

//TODO add hashing
void AChessGameState::TakeMove()
{
	FChessMoveRecord Record = History.Last();
	FChessMove Move = Record.Move;

	int32 FromIdx = Move.GetFrom().ToInt();
	int32 ToIdx = Move.GetTo().ToInt();
	
	//Must be valid!
	check(Move.GetFrom().IsValid() && Move.GetTo().IsValid())

	//Revert back values
	CastlePermission = Record.CastlePermission;
	FiftyMoveCounter = Record.FiftyMove;
	EnPassantTile = Record.EnPassantTile;

	//Revert moving side
	SetMovingSide(static_cast<EPieceColor>((int32)Side ^ 1));
	int32 SideCode = (int32)Side;

	//If was en passant move, revert captured pawn
	if (Move.IsEnPassantMove())
	{
		if (Side == EPieceColor::White)
		{
			AddPiece(FTileCoord{ ToIdx - 10 }, GBlackPawn);
		}
		else
		{
			AddPiece(FTileCoord{ ToIdx + 10 }, GWhitePawn);
		}
	}
	else if (Move.IsCastlingMove())
	{
		//If was castling move, revert rook position
		switch(Move.GetTo().GetEnum())
		{
		case ETileCoord::C1:
			MovePiece({ ETileCoord::D1 }, { ETileCoord::A1 });
			break;

		case ETileCoord::C8:
			MovePiece({ ETileCoord::D8 }, { ETileCoord::A8 });
			break;

		case ETileCoord::G1:
			MovePiece({ ETileCoord::F1 }, { ETileCoord::H1 });
			break;

		case ETileCoord::G8:
			MovePiece({ ETileCoord::F8 }, { ETileCoord::H8 });
			break;

		default:

			check(false && "Invalid castling move");
			break;
		}
	}

	//Revert moved piece
	MovePiece(Move.GetTo(), Move.GetFrom());

	//Update king position
	if (Tiles[FromIdx].GetPiece().IsA(EChessPieceRole::King))
	{
		Kings[SideCode] = Move.GetFrom();
	}

	//If piece was captured, return it back to the board
	if (UChessGameStatics::IsCaptureMove(Move))
	{
		FChessPiece Piece = FChessPiece::GetPieceFromCode(Move.GetCapturedPiece());
		AddPiece(Move.GetTo(), Piece);
	}

	//If piece was promoted, remove it and return pawn back
	if (UChessGameStatics::IsPromotionMove(Move))
	{
		FChessPiece Piece = FChessPiece::GetPieceFromCode(Move.GetPromotedPiece());
		EPieceColor PromotedColor = Piece.GetColor();
		
		ClearPiece(Move.GetFrom());
		AddPiece(Move.GetFrom(),
			PromotedColor == EPieceColor::White ? GWhitePawn : GBlackPawn
		);
	}

	//Delete history record
	History.Pop();
}

void AChessGameState::CheckKingState()
{
	if (IsStalemate())
	{
		UE_LOG(LogGameState, Warning, TEXT("Stalemate!"));
		EndGame(EEndChessGameReason::Stalemate);
	}
	else if (IsCheckMate())
	{
		UE_LOG(LogGameState, Warning, TEXT("Checkmate!"));
		EndGame(EEndChessGameReason::Mate);
	}
	else if (IsCheck())
	{
		UE_LOG(LogGameState, Warning, TEXT("Check!"));
		KingCheck.Broadcast(Side);
	}
}

bool AChessGameState::MakeMove(const FChessMove& Move)
{
	const int32 FromIdx = Move.GetFromTileIndex();
	const int32 ToIdx = Move.GetToTileIndex();

	const FChessBoardTile From = Tiles[FromIdx];
	const FChessBoardTile To = Tiles[ToIdx];

	const FChessPiece Piece = Tiles[FromIdx].GetPiece();
	check(Piece != GEmptyChessPiece);

	FChessMoveRecord HistoryRecord {
		Move,
		CastlePermission,
		EnPassantTile.Get(99),
		FiftyMoveCounter,
		PosHashKey
	};

	History.Push(HistoryRecord);


	if (Move.IsEnPassantMove())
	{
		if (Side == EPieceColor::White)
		{
			ClearPiece(Tiles[ToIdx - 10].GetPosition());
		}
		else
		{
			ClearPiece(Tiles[ToIdx + 10].GetPosition());
		}

		UE_LOG(LogGameState, Display, TEXT("En passant move performed"));
	}
	else if (Move.IsCastlingMove())
	{
		switch (To.GetPosition().GetEnum())
		{
		case ETileCoord::C1:
			MovePiece({ETileCoord::A1}, {ETileCoord::D1});
			break;

		case ETileCoord::C8:
			MovePiece({ETileCoord::A8}, {ETileCoord::D8});
			break;

		case ETileCoord::G1:
			MovePiece({ETileCoord::H1}, {ETileCoord::F1});
			break;

		case ETileCoord::G8:
			MovePiece({ETileCoord::H8}, {ETileCoord::F8});
			break;

		default:
			check(false && "Invalid castling move");
		}

		UE_LOG(LogGameState, Display, TEXT("Castling move performed"));
	}

	if (EnPassantTile.IsSet())
	{
		HashEnPassant();
	}

	HashCastle();

	CastlePermission &= From.GetCastlePermission();
	CastlePermission &= To.GetCastlePermission();
	EnPassantTile.Reset();

	HashCastle();

	const FChessPiece CapturedPiece = FChessPiece::GetPieceFromCode(Move.GetCapturedPiece());
	++FiftyMoveCounter;

	if (CapturedPiece != GEmptyChessPiece)
	{
		ClearPiece(To.GetPosition());
		FiftyMoveCounter = 0;
	}

	if (Piece.IsA(EChessPieceRole::Pawn))
	{
		FiftyMoveCounter = 0;
		if (Move.IsPawnStartMove())
		{
			if (Side == EPieceColor::White)
			{
				EnPassantTile.Emplace(From.GetPosition().ToInt() + 10);
			}
			else
			{
				EnPassantTile.Emplace(From.GetPosition().ToInt() - 10);
			}

			HashEnPassant();
		}
	}

	MovePiece(From.GetPosition(), To.GetPosition());

	const FChessPiece PromotedPiece = FChessPiece::GetPieceFromCode(Move.GetPromotedPiece());
	if (PromotedPiece != GEmptyChessPiece)
	{
		ClearPiece(To.GetPosition());
		AddPiece(To.GetPosition(), PromotedPiece);
	}

	if (Piece.IsA(EChessPieceRole::King))
	{
		Kings[static_cast<int32>(Side)] = To.GetPosition();
	}

	int32 SideCode = static_cast<int32>(Side);
	SetMovingSide(static_cast<EPieceColor>(SideCode ^ 1));

	if (IsTileAttacked(Kings[SideCode].GetFile(), Kings[SideCode].GetRank(), Side))
	{
		TakeMove();
		MoveFailed.Broadcast(Side);
		
		UE_LOG(LogGameState, Warning, TEXT("King is attacked, reverting move"));

		return false;
	}

	UE_LOG(LogGameState, Display, TEXT("Now moving side is %s"), *UEnum::GetValueAsString(Side));

	HashSide();

	return true;
}

const TArray<FChessMove>& AChessGameState::GetMoves() const
{
	return Moves;
}

void AChessGameState::BeginPlay()
{
	Super::BeginPlay();
}

void AChessGameState::GenerateWhitePawnMoves()
{
	//Generate moves for white pawns
	int32 PieceCode = GWhitePawn.GetCode();
	int32 Count = PieceCount[PieceCode];

	for (int32 i = 0; i < Count; ++i)
	{
		FTileCoord From = PieceList[PieceCode][i];
		check(From.IsValid());

		int32 FromIdx = From.ToInt();

		//Check for simple moves
		if (Tiles[FromIdx + 10].IsEmpty())
		{
			AddWhitePawnMove(From, Tiles[FromIdx + 10].GetPosition());
			if (From.GetRank() == EBoardRank::Two && Tiles[FromIdx + 20].IsEmpty())
			{
				AddQuietMove(
					{
						From,
						Tiles[FromIdx + 20].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_PawnStartMove
					}
				);
			}
		}

		//Check for capture moves
		FChessBoardTile Tile = Tiles[FromIdx + 9];
		if (!Tile.IsEmpty() && Tile.GetPiece().GetColor() == EPieceColor::Black)
		{
			AddWhitePawnCaptureMove(From, Tile.GetPosition(), Tile.GetPiece());
		}

		Tile = Tiles[FromIdx + 11];
		if (!Tile.IsEmpty() && Tile.GetPiece().GetColor() == EPieceColor::Black)
		{
			AddWhitePawnCaptureMove(From, Tile.GetPosition(), Tile.GetPiece());
		}

		//Check for en passant moves
		if (EnPassantTile.IsSet())
		{
			if (FromIdx + 9 == EnPassantTile.GetValue())
			{
				AddCaptureMove(
					{
						From,
						Tiles[FromIdx + 9].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_EnPassantMove
					}
				);
			}

			if (FromIdx + 11 == EnPassantTile.GetValue())
			{
				AddCaptureMove(
					{
						From,
						Tiles[FromIdx + 11].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_EnPassantMove
					}
				);
			}
		}
	}
}

void AChessGameState::GenerateBlackPawnMoves()
{
	//Generate moves for black pawns
	int32 PieceCode = GBlackPawn.GetCode();
	int32 Count = PieceCount[PieceCode];

	for (int32 i = 0; i < Count; ++i)
	{
		FTileCoord From = PieceList[PieceCode][i];
		check(From.IsValid());

		int32 FromIdx = From.ToInt();

		//Check for simple moves
		if (Tiles[FromIdx - 10].IsEmpty())
		{
			AddBlackPawnMove(From, Tiles[FromIdx - 10].GetPosition());
			if (From.GetRank() == EBoardRank::Seven && Tiles[FromIdx - 20].IsEmpty())
			{
				AddQuietMove(
					{
						From,
						Tiles[FromIdx - 20].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_PawnStartMove
					}
				);
			}
		}
		//Check for capture moves
		FChessBoardTile Tile = Tiles[FromIdx - 9];
		if (!Tile.IsEmpty() && Tile.GetPiece().GetColor() == EPieceColor::White)
		{
			AddBlackPawnCaptureMove(From, Tile.GetPosition(), Tile.GetPiece());
		}

		Tile = Tiles[FromIdx - 11];
		if (!Tile.IsEmpty() && Tile.GetPiece().GetColor() == EPieceColor::White)
		{
			AddBlackPawnCaptureMove(From, Tile.GetPosition(), Tile.GetPiece());
		}

		//Check for en passant moves
		if (EnPassantTile.IsSet())
		{
			if (FromIdx - 9 == EnPassantTile.GetValue())
			{
				AddCaptureMove(
					{
						From,
						Tiles[FromIdx - 9].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_EnPassantMove
					}
				);
			}

			if (FromIdx - 11 == EnPassantTile.GetValue())
			{
				AddCaptureMove(
					{
						From,
						Tiles[FromIdx - 11].GetPosition(),
						nullptr,
						nullptr,
						FChessMove::FLAG_EnPassantMove
					}
				);
			}
		}
	}
}

void AChessGameState::GenerateSlideMoves()
{
	auto&& Pieces = FChessPiece::GetSlidingPiecesByColor(Side);
	for (auto&& Piece : Pieces)
	{
		int32 PieceCode = Piece.GetCode();
		int32 Count = PieceCount[PieceCode];

		for (int32 i = 0; i < Count; ++i)
		{
			FTileCoord From = PieceList[PieceCode][i];
			check(From.IsValid());

			int32 FromIdx = From.ToInt();

			auto&& Dirs = Piece.GetMoveDirections();
			for (auto&& Dir : Dirs)
			{
				FChessBoardTile TTile = Tiles[FromIdx + Dir];

				while (TTile.IsOnBoard())
				{
					if (!TTile.IsEmpty())
					{
						if (TTile.GetPiece().GetColorCode() == (static_cast<int32>(Side) ^ 1))
						{
							AddCaptureMove({From, TTile.GetPosition(), &TTile.GetPiece(), nullptr});
						}
						break;
					}

					AddQuietMove({From, TTile.GetPosition(), nullptr, nullptr});
					TTile = Tiles[TTile.GetPosition().ToInt() + Dir];
				}
			}
		}
	}
}

void AChessGameState::GenerateNonSlideMoves()
{
	auto&& Pieces = FChessPiece::GetNonSlidingPiecesByColor(Side);
	for (auto&& Piece : Pieces)
	{
		int32 PieceCode = Piece.GetCode();
		int32 Count = PieceCount[PieceCode];

		for (int32 i = 0; i < Count; ++i)
		{
			FTileCoord From = PieceList[PieceCode][i];
			check(From.IsValid());

			int32 FromIdx = From.ToInt();

			auto&& Dirs = Piece.GetMoveDirections();
			for (auto&& Dir : Dirs)
			{
				FChessBoardTile TTile = Tiles[FromIdx + Dir];

				if (!TTile.IsOnBoard())
					continue;

				if (!TTile.IsEmpty())
				{
					if (TTile.GetPiece().GetColorCode() == (static_cast<int32>(Side) ^ 1))
					{
						AddCaptureMove({From, TTile.GetPosition(), &TTile.GetPiece(), nullptr});
					}
					continue;
				}

				AddQuietMove({From, TTile.GetPosition(), nullptr, nullptr});
			}
		}
	}
}

void AChessGameState::GenerateWhiteCastling()
{
	if (CastlePermission & static_cast<int32>(ECastlingType::WhiteKing))
	{
		if (
			Tiles[FTileCoord{ETileCoord::F1}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{ETileCoord::G1}.ToInt()].IsEmpty()
		)
		{
			if (
				!IsTileAttacked(EBoardFile::E, EBoardRank::One, EPieceColor::Black) &&
				!IsTileAttacked(EBoardFile::F, EBoardRank::One, EPieceColor::Black)
			)
			{
				FTileCoord From{ETileCoord::E1};
				FTileCoord To{ETileCoord::G1};

				AddQuietMove({From, To, nullptr, nullptr, FChessMove::FLAG_CastlingMove});
			}
		}
	}

	if (CastlePermission & static_cast<int32>(ECastlingType::WhiteQueen))
	{
		if (
			Tiles[FTileCoord{EBoardFile::D, EBoardRank::One}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{EBoardFile::C, EBoardRank::One}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{EBoardFile::B, EBoardRank::One}.ToInt()].IsEmpty()
		)
		{
			if (
				!IsTileAttacked(EBoardFile::E, EBoardRank::One, EPieceColor::Black) &&
				!IsTileAttacked(EBoardFile::D, EBoardRank::One, EPieceColor::Black)
			)
			{
				FTileCoord From{ETileCoord::E1};
				FTileCoord To{ETileCoord::C1};

				AddQuietMove({From, To, nullptr, nullptr, FChessMove::FLAG_CastlingMove});
			}
		}
	}
}

void AChessGameState::GenerateBlackCastling()
{
	if (CastlePermission & static_cast<int32>(ECastlingType::BlackKing))
	{
		if (
			Tiles[FTileCoord{EBoardFile::F, EBoardRank::Eight}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{EBoardFile::G, EBoardRank::Eight}.ToInt()].IsEmpty()
		)
		{
			if (
				!IsTileAttacked(EBoardFile::E, EBoardRank::Eight, EPieceColor::White) &&
				!IsTileAttacked(EBoardFile::F, EBoardRank::Eight, EPieceColor::White)
			)
			{
				FTileCoord From{ETileCoord::E8};
				FTileCoord To{ETileCoord::G8};

				AddQuietMove({From, To, nullptr, nullptr, FChessMove::FLAG_CastlingMove});
			}
		}
	}

	if (CastlePermission & static_cast<int32>(ECastlingType::BlackQueen))
	{
		if (
			Tiles[FTileCoord{EBoardFile::D, EBoardRank::Eight}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{EBoardFile::C, EBoardRank::Eight}.ToInt()].IsEmpty() &&
			Tiles[FTileCoord{EBoardFile::B, EBoardRank::Eight}.ToInt()].IsEmpty()
		)
		{
			if (
				!IsTileAttacked(EBoardFile::E, EBoardRank::Eight, EPieceColor::White) &&
				!IsTileAttacked(EBoardFile::D, EBoardRank::Eight, EPieceColor::White)
			)
			{
				FTileCoord From{ETileCoord::E8};
				FTileCoord To{ETileCoord::C8};

				AddQuietMove({From, To, nullptr, nullptr, FChessMove::FLAG_CastlingMove});
			}
		}
	}
}

void AChessGameState::HashPiece(const FChessPiece& Piece, const FTileCoord& Coord)
{
	PosHashKey ^= PieceHashKeys[Piece.GetCode()][Coord.ToInt()];
}

void AChessGameState::HashCastle()
{
	PosHashKey ^= CastleHashKeys[CastlePermission];
}

void AChessGameState::HashSide()
{
	PosHashKey ^= SideHashKey;
}

void AChessGameState::HashEnPassant()
{
	PosHashKey ^= PieceHashKeys[GEmptyChessPiece.GetCode()][EnPassantTile.Get(99)];
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

			if (Piece.IsMinorPiece())
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
			PieceList[PieceCode][PieceCount[PieceCode]] = Tile.GetPosition();
			++PieceCount[PieceCode];

			if (Piece == GWhiteKing || Piece == GBlackKing)
			{
				Kings[ColorCode] = Tiles[TileIdx].GetPosition();
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

	const int32 MinRank = FTileCoord::GetMinRankIndex();
	const int32 MaxRank = FTileCoord::GetMaxRankIndex();

	const int32 MinFile = FTileCoord::GetMinFileIndex();
	const int32 MaxFile = FTileCoord::GetMaxFileIndex();

	int32 TileAt64Array = 0;

	for (int32 i = MinRank; i <= MaxRank; ++i)
	{
		for (int32 j = MinFile; j <= MaxFile; ++j)
		{
			const EBoardRank CurrentRank = FTileCoord::ToRank(i);
			const EBoardFile CurrentFile = FTileCoord::ToFile(j);

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

void AChessGameState::EndGame(EEndChessGameReason Reason)
{
	bEnded = true;
	
	switch(Reason)
	{
	case EEndChessGameReason::Mate:
		KingCheckMate.Broadcast(Side);
		return;

	case EEndChessGameReason::Stalemate:
		StaleMate.Broadcast();
		return;
	}
}

bool AChessGameState::IsFinished() const
{
	return bEnded;
}

bool AChessGameState::IsCheck()
{
	int32 SideCode = (int32)Side;
	EPieceColor TheirSide = static_cast<EPieceColor>(SideCode ^ 1);
	
	FTileCoord KingTile = Kings[SideCode];
	
	return (IsTileAttacked(KingTile.GetFile(), KingTile.GetRank(), TheirSide));
}

bool AChessGameState::IsCheckMate()
{
	//Very straightforward
	
	if (!IsCheck())
		return false;

	for (auto&& Move : Moves)
	{

		//Make move
		if (MakeMove(Move))
		{
			//Is check away?
			if (!IsCheck())
			{
				//Move away
				TakeMove();

				//No checkmate!
				return false;
			}

			TakeMove();
		}
	}

	//All moves lead to checkmate...
	return true;
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
	uint32 Fold = static_cast<uint32>((TempBitboard & 0xffffffff) ^ (TempBitboard >> 32));
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

void AChessGameState::ClearBit(uint64& BitBoard, int32 Idx)
{
	BitBoard &= ClearMask[Idx];
}

void AChessGameState::SetBit(uint64& BitBoard, int32 Idx)
{
	BitBoard |= SetMask[Idx];
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
		Tiles[Tile].SetPiece(GEmptyChessPiece);
	}

	//Reset all counters to 0
	for (int32 i = 0; i < BigPieces.Num(); ++i)
	{
		BigPieces[i] = 0;
		MajorPieces[i] = 0;
		MinorPieces[i] = 0;
		Material[i] = 0;
	}

	for (int32 i = 0; i < Pawns.Num(); ++i)
	{
		Pawns[i] = 0;
	}

	for (int32 i = 0; i < PieceCount.Num(); ++i)
	{
		PieceCount[i] = 0;
	}

	Kings[0] = FTileCoord{};
	Kings[1] = Kings[0];

	Side = EPieceColor::Both;

	EnPassantTile.Reset();

	FiftyMoveCounter = 0;
	CastlePermission = 0;
	PosHashKey = 0;

	History.Reset();
}

bool AChessGameState::IsStalemate() const
{
	return Moves.Num() == 0;
}
