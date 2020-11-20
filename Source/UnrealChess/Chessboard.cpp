// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"

#include "Chess.h"
#include "ChessGameState.h"
#include "DrawDebugHelpers.h"

// Sets default values
AChessboard::AChessboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileSize = 25;

	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>("Board");
}

FVector AChessboard::GetTileCenter(EBoardFile File, EBoardRank Rank) const
{
	const int32 FileIndex = (int32)File;
	const int32 RankIndex = (int32)Rank;

	return CornerLocation + FVector{
		-(TileSize * RankIndex + TileSize/2.f),
		TileSize * FileIndex + TileSize/2.f,
		0
	};
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < 8; ++i)
	{
		for (int32 j = 0; j < 8; ++j)
		{
			auto X = static_cast<EBoardRank>(i);
			auto Y = static_cast<EBoardFile>(j);

			ETileState State = GetChessGameState()->GetPieceAtTile(Y, X);

			if (State != ETileState::NoPiece)
			{
				FTransform T = FTransform{ GetTileCenter(Y, X) };
				
				auto Actor = GetWorld()->SpawnActorDeferred<AChess>(
						AChess::StaticClass(), T
					);

				Actor->InitFromState(State);

				Actor->FinishSpawning(T, true);
			}
		}
	}
}

AChessGameState* AChessboard::GetChessGameState() const
{
	return GetWorld() != nullptr ? Cast<AChessGameState>(GetWorld()->GetGameState()) : nullptr;
}

void AChessboard::DrawDebug()
{
	for (int32 i = 0; i < 8; ++i)
	{
		for (int32 j = 0; j < 8; ++j)
		{
			auto X = static_cast<EBoardRank>(i);
			auto Y = static_cast<EBoardFile>(j);
			
			DrawDebugPoint(GetWorld(), GetTileCenter(Y, X), 10, FColor::Red);
		}
	}
	
	DrawDebugLine(GetWorld(), CornerLocation, CornerLocation + FVector{ 0, 8.f * TileSize, 0 }, FColor::Green);
	DrawDebugLine(GetWorld(), CornerLocation, CornerLocation + FVector{ -(8.f * TileSize), 0, 0 }, FColor::Green);
	DrawDebugLine(GetWorld(), CornerLocation + FVector{ 0, 8.f * TileSize, 0 }, CornerLocation + FVector{ -(8.f * TileSize), 8.f * TileSize, 0 }, FColor::Green);
	DrawDebugLine(GetWorld(), CornerLocation + FVector{ -(8.f * TileSize), 0, 0 }, CornerLocation + FVector{ -(8.f * TileSize), 8.f * TileSize, 0 }, FColor::Green);
}

void AChessboard::OnConstruction(const FTransform& Transform)
{
	CornerLocation = GetActorLocation() + FVector{ 100, -100, 0 };
}

// Called every frame
void AChessboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebug();
}

