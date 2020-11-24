// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ChessDefinitions.h"
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChessPlayerController.generated.h"

class AChessboard;
/**
 * 
 */
UCLASS(Blueprintable)
class UNREALCHESS_API AChessPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

	//Side of the player
	UPROPERTY(VisibleAnywhere, Category="Chess")
	EPieceColor Side;

public:
	//Getter, setter
	//
	UFUNCTION(BlueprintCallable, Category="Get")
	EPieceColor GetSide() const;

	UFUNCTION(Client, Reliable, BlueprintCallable, Category="Set")
	void SetSide(EPieceColor NewSide);

	//Board doesn't have owner, so we ask player controller to notify server about move
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_NotifyPlayerMoved(AChessboard* Board, int32 FromIdx, int32 ToIdx);
};
