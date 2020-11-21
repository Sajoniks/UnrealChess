#include "ChessDefinitions.h"

/***********************************Predefined pieces************************************************/

const FChessPiece EmptyChessPiece{};

const FChessPiece WhitePawn{ EPieceColor::White, ETileState::WhitePawn, false, false, false, 100 };
const FChessPiece BlackPawn{ EPieceColor::Black, ETileState::BlackPawn, false, false, false, 100 };

const FChessPiece WhiteKnight{ EPieceColor::White, ETileState::WhiteKnight, false, false, false, 350 };
const FChessPiece BlackKnight{ EPieceColor::Black, ETileState::BlackKnight, false, false, false, 350};

const FChessPiece WhiteBishop{ EPieceColor::White, ETileState::WhiteBishop, false, false, false, 350 };
const FChessPiece BlackBishop{ EPieceColor::Black, ETileState::BlackBishop, false, false, false, 350};

const FChessPiece WhiteRook{ EPieceColor::White, ETileState::WhiteRook, false, false, false, 550};
const FChessPiece BlackRook{ EPieceColor::Black, ETileState::BlackRook, false, false, false, 550 };

const FChessPiece WhiteQueen{ EPieceColor::White, ETileState::WhiteQueen, false, false, false, 1000};
const FChessPiece BlackQueen{ EPieceColor::Black, ETileState::BlackQueen, false, false, false, 1000 };

const FChessPiece BlackKing{ EPieceColor::Black, ETileState::BlackKing, false, false, false, 50000 };
const FChessPiece WhiteKing{ EPieceColor::White, ETileState::WhiteKing, false, false, false, 50000 };

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
	return Cost;
}

ETileState FChessPiece::GetEnum() const
{
	return State;
}

EPieceColor FChessPiece::GetColor() const
{
	return Color;
}

int32 FChessPiece::GetColorCode() const
{
	return (int32)Color;
}

bool FChessPiece::IsBigPiece() const
{
	return bIsBig;
}

bool FChessPiece::IsMajorPiece() const
{
	return bIsMajor;
}

bool FChessPiece::IsMinorPiece() const
{
	return bIsMinor;
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

const FChessPiece& FChessPiece::GetPieceFromChar(TCHAR Char)
{
	switch (Char)
	{
	case 'P':
		return WhitePawn;
	case 'N':
		return WhiteKnight;
	case 'B':
		return WhiteBishop;
	case 'R':
		return WhiteRook;
	case 'Q':
		return WhiteQueen;
	case 'K':
		return WhiteKing;
		
	case 'p':
		return BlackPawn;
	case 'n':
		return BlackKnight;
	case 'b':
		return BlackBishop;
	case 'r':
		return BlackRook;
	case 'q':
		return BlackQueen;
	case 'k':
		return BlackKing;

	default:
		return EmptyChessPiece;
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
