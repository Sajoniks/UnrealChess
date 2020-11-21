// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"

#include "Chess.h"
#include "ChessGameState.h"
#include "Components/ArrowComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AChessboard::AChessboard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileSize = 25;

	BoardMesh = CreateDefaultSubobject<UStaticMeshComponent>("Board");
	SetRootComponent(BoardMesh);
	
	Arrow = CreateDefaultSubobject<UArrowComponent>("Arrow");
	Arrow->SetWorldLocation(BoardMesh->GetSocketLocation("FL"));
	Arrow->AttachToComponent(BoardMesh, FAttachmentTransformRules::KeepRelativeTransform);
}

FVector AChessboard::GetTileCenter(EBoardFile File, EBoardRank Rank) const
{
	const int32 FileIndex = (int32)File;
	const int32 RankIndex = (int32)Rank;

	FVector FL = BoardMesh->GetSocketLocation("FL");

	FVector Pos = FVector{
		-(TileSize * RankIndex + TileSize / 2.f),
		TileSize * FileIndex + TileSize / 2.f,
		0
	};

	Pos = UKismetMathLibrary::TransformLocation(
		GetRootComponent()->GetComponentTransform(),
		Pos
	);

	return Pos + FL;
}

TOptional<FVector> AChessboard::GetTileCenter(float X, float Y)
{
	return TOptional<FVector>();
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

			FChessPiece Piece = GetChessGameState()->GetPieceAtTile(Y, X);

			//TODO
			if (Piece != EmptyChessPiece)
			{
				FRotator R = GetActorRotation();
				FTransform T = FTransform{R, GetTileCenter(Y,X)};
				
				auto Actor = GetWorld()->SpawnActorDeferred<AChess>(
						AChess::StaticClass(), T
					);

				Actor->InitPiece(Piece, this);
				
				Actor->FinishSpawning(T);
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
	FVector FL = BoardMesh->GetSocketLocation("FL");
	FVector FR = BoardMesh->GetSocketLocation("FR");
	FVector BL = BoardMesh->GetSocketLocation("BL");
	FVector BR = BoardMesh->GetSocketLocation("BR");
	
	for (int32 i = 0; i < 8; ++i)
	{
		for (int32 j = 0; j < 8; ++j)
		{
			auto X = static_cast<EBoardRank>(i);
			auto Y = static_cast<EBoardFile>(j);

			bool bAttacked = GetChessGameState()->IsSquareAttacked(Y, X, EPieceColor::Black);

			DrawDebugPoint(GetWorld(), GetTileCenter(Y, X), 10, 
				bAttacked ? 
				FColor::Red : FColor::Emerald
			);
		}
	}

	DrawDebugLine(GetWorld(), FL, BL, FColor::Green);
	DrawDebugLine(GetWorld(), FR, BR, FColor::Green);
	DrawDebugLine(GetWorld(), FL, FR, FColor::Green);
	DrawDebugLine(GetWorld(), BL, BR, FColor::Green);
}

void AChessboard::OnConstruction(const FTransform& Transform)
{

}

// Called every frame
void AChessboard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebug();
}

