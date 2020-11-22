#include "ChessDefinitions.h"

/***********************************Predefined pieces************************************************/

const FChessPiece GEmptyChessPiece{ ETileState::NoPiece};

const FChessPiece GWhitePawn{ ETileState::WhitePawn };
const FChessPiece GBlackPawn{ ETileState::BlackPawn };

const FChessPiece GWhiteKnight{ ETileState::WhiteKnight};
const FChessPiece GBlackKnight{ ETileState::BlackKnight};

const FChessPiece GWhiteBishop{ ETileState::WhiteBishop};
const FChessPiece GBlackBishop{ETileState::BlackBishop};

const FChessPiece GWhiteRook{ETileState::WhiteRook};
const FChessPiece GBlackRook{ETileState::BlackRook};

const FChessPiece GWhiteQueen{ ETileState::WhiteQueen};
const FChessPiece GBlackQueen{ ETileState::BlackQueen};

const FChessPiece GBlackKing{ ETileState::BlackKing };
const FChessPiece GWhiteKing{  ETileState::WhiteKing};

const TArray<int32, TFixedAllocator<13>> FChessPiece::PiecesCost = {
0, 100, 350, 350, 550, 1000, 50000, 100, 350, 350, 550, 1000, 50000
};

const TArray<bool, TFixedAllocator<13>> FChessPiece::BigPieces = {
	0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1
};

const TArray<bool, TFixedAllocator<13>> FChessPiece::MajorPieces = {
	0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0
};

const TArray<bool, TFixedAllocator<13>> FChessPiece::MinorPieces = {
	0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0
};

const TArray<TArray<int32>, TFixedAllocator<13>> FChessPiece::MoveDirections = {
	{},
	{},
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9},
	{-1, -10, 1, 10},
	{-1, -10, 1, 10, -9, -11, 9, 11},
	{-1, -10, 1, 10, -9, -11, 9, 11},
	{},
	{-8, -19, -21, -12, 8, 19, 21, 12},
	{-9, -11, 11, 9},
	{-1, -10, 1, 10},
	{-1, -10, 1, 10, -9, -11, 9, 11},
	{-1, -10, 1, 10, -9, -11, 9, 11}
};

const TArray<FChessPiece> FChessPiece::WSlidingPieces = {
	GWhiteBishop,
	GWhiteRook,
	GWhiteQueen
};

const TArray<FChessPiece> FChessPiece::BSlidingPieces = {
	GBlackBishop,
	GBlackRook,
	GBlackQueen
};


const TArray<FChessPiece> FChessPiece::WNonSlidingPieces = {
	GWhiteKnight,
	GWhiteKing
};

const TArray<FChessPiece> FChessPiece::BNonSlidingPieces = {
	GBlackKnight,
	GBlackKing
};


/************************************************************************************************/

bool FChessPiece::operator==(const FChessPiece& Other) const
{
	if (this == &Other)
		return true;

	return State == Other.State;
}

bool FChessPiece::operator!=(const FChessPiece& Other) const
{
	return !(*this == Other);
}

int32 FChessPiece::GetCode() const
{
	return (int32)State;
}

int32 FChessPiece::GetCost() const
{
	return PiecesCost[GetCode()];
}

ETileState FChessPiece::GetEnum() const
{
	return State;
}

EPieceColor FChessPiece::GetColor() const
{
	if (FMath::IsWithinInclusive(GetCode(), 1, 6))
		return EPieceColor::White;

	if (FMath::IsWithinInclusive(GetCode(), 7, 12))
		return EPieceColor::Black;

	return EPieceColor::NoColor;
}

int32 FChessPiece::GetColorCode() const
{
	return (int32)GetColor();
}

bool FChessPiece::IsBigPiece() const
{
	return BigPieces[GetCode()];
}

bool FChessPiece::IsMajorPiece() const
{
	return MajorPieces[GetCode()];
}

bool FChessPiece::IsMinorPiece() const
{
	return MinorPieces[GetCode()];
}

bool FChessPiece::IsA(EChessPieceRole Role) const
{
	switch(Role)
	{
	case EChessPieceRole::Pawn:
		return State == ETileState::BlackPawn || State == ETileState::WhitePawn;

	case EChessPieceRole::Knight:
		return State == ETileState::BlackKnight || State == ETileState::WhiteKnight;

	case EChessPieceRole::Bishop:
		return State == ETileState::BlackBishop || State == ETileState::WhiteBishop;

	case EChessPieceRole::Rook:
		return State == ETileState::BlackRook || State == ETileState::WhiteRook;

	case EChessPieceRole::Queen:
		return State == ETileState::BlackQueen || State == ETileState::WhiteQueen;

	case EChessPieceRole::King:
		return State == ETileState::BlackKing || State == ETileState::WhiteKing;
		
	default: return false;
	}
}

const TArray<int32>& FChessPiece::GetMoveDirections() const
{
	return MoveDirections[GetCode()];
}

const TArray<FChessPiece>& FChessPiece::GetSlidingPiecesByColor(EPieceColor Color)
{
	check(Color == EPieceColor::White || Color == EPieceColor::Black);

	return Color == EPieceColor::White ? WSlidingPieces : BSlidingPieces;
}

const TArray<FChessPiece>& FChessPiece::GetNonSlidingPiecesByColor(EPieceColor Color)
{
	check(Color == EPieceColor::White || Color == EPieceColor::Black);

	return Color == EPieceColor::White ? WNonSlidingPieces : BNonSlidingPieces;
}

const FChessPiece& FChessPiece::GetPieceFromChar(TCHAR Char)
{
	switch (Char)
	{
	case 'P':
		return GWhitePawn;
	case 'N':
		return GWhiteKnight;
	case 'B':
		return GWhiteBishop;
	case 'R':
		return GWhiteRook;
	case 'Q':
		return GWhiteQueen;
	case 'K':
		return GWhiteKing;
		
	case 'p':
		return GBlackPawn;
	case 'n':
		return GBlackKnight;
	case 'b':
		return GBlackBishop;
	case 'r':
		return GBlackRook;
	case 'q':
		return GBlackQueen;
	case 'k':
		return GBlackKing;

	default:
		return GEmptyChessPiece;
	}
}


ETileCoord FTileCoordinate::GetEnum() const
{
	return Coordinate;
}

EBoardFile FTileCoordinate::GetFile() const
{
	return File;
}

EBoardRank FTileCoordinate::GetRank() const
{
	return Rank;
}

bool FTileCoordinate::IsValid() const
{
	return Coordinate != ETileCoord::NoTile && Rank != EBoardRank::None && File != EBoardFile::None;
}

int32 FTileCoordinate::ToInt() const
{
	return (int32)Coordinate;
}
