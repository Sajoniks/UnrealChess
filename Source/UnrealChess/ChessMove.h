#pragma once

#include "CoreMinimal.h"
#include "TileCoordinate.h"

#include "ChessMove.generated.h"

class FChessPiece;

USTRUCT(BlueprintType)
struct UNREALCHESS_API FChessMove
{
	GENERATED_BODY()
	
private:

	int32 Move = 0;
	int32 Score = 0;
	int32 Castling = 0;
	
public:

	static const int32 FLAG_CastlingMove = 0x1000000;
	static const int32 FLAG_EnPassantMove = 0x40000;
	static const int32 FLAG_PawnStartMove = 0x80000;

	FChessMove() = default;

	FChessMove(
		const FTileCoord& From,
		const FTileCoord& To,
		const FChessPiece* Captured,
		const FChessPiece* Promoted,
		int32 Flags = 0
	);

	int32 GetFromTileIndex()		const { return Move & 0x7F; }
	FTileCoord GetFrom()	const { return { (ETileCoord)GetFromTileIndex() }; }

	int32 GetToTileIndex()		const { return (Move >> 7) & 0x7F; }
	FTileCoord GetTo()		const { return { (ETileCoord)GetToTileIndex() }; }

	int32 GetCapturedPiece()		const { return (Move >> 14) & 0xF; }
	int32 GetPromotedPiece()		const { return (Move >> 20) & 0xF; }
	
	bool IsEnPassantMove()		const { return Move & 0x40000; }
	bool IsPawnStartMove()		const { return Move & 0x80000; }
	bool IsCastlingMove()			const { return Move & 0x1000000; }

	int32 Raw()					const { return Move; }
};