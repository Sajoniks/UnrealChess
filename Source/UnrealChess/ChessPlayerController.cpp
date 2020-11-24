// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessPlayerController.h"
#include "Chessboard.h"

void AChessPlayerController::SetSide_Implementation(EPieceColor NewSide)
{
	Side = NewSide;
}

EPieceColor AChessPlayerController::GetSide() const
{
	return Side;
}

bool AChessPlayerController::Server_NotifyPlayerMoved_Validate(AChessboard* Board, int32 FromIdx, int32 ToIdx)
{
	//99 is offboard index
	return Board && FromIdx != 99 && ToIdx != 99;
}

void AChessPlayerController::Server_NotifyPlayerMoved_Implementation(AChessboard* Board, int32 FromIdx, int32 ToIdx)
{
	Board->Multicast_Move(FromIdx, ToIdx);
}
