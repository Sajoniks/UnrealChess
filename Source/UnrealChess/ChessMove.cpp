#pragma once

#include "ChessMove.h"

FChessMove::FChessMove(const FTileCoord& From, const FTileCoord& To, const FChessPiece* Captured,
                       const FChessPiece* Promoted, int32 Flags)
{
	int32 PromotedCode = (Promoted) ? Promoted->GetCode() : 0;
	int32 CapturedCode = (Captured) ? Captured->GetCode() : 0;

	Move = (
		(From.ToInt()) |
		(To.ToInt() << 7) |
		(CapturedCode << 14) |
		(PromotedCode << 20) |
		(Flags)
	);
}

int32 FChessMove::GetFromTileIndex() const
{
	return Move & 0x7F;
}

FTileCoord FChessMove::GetFrom() const
{
	return {(ETileCoord)GetFromTileIndex()};
}

int32 FChessMove::GetToTileIndex() const
{
	return (Move >> 7) & 0x7F;
}

FTileCoord FChessMove::GetTo() const
{
	return {(ETileCoord)GetToTileIndex()};
}
