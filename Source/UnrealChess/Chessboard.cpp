// Fill out your copyright notice in the Description page of Project Settings.


#include "Chessboard.h"

#include "Chess.h"
#include "ChessGameState.h"
#include "ChessGameStatics.h"
#include "ChessPlayerController.h"
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
	const int32 RankIndex = FTileCoord::GetMaxRankIndex() - (int32)Rank;

	return GetTileCenter(RankIndex, FileIndex);
}

FVector AChessboard::GetTileCenter(int32 X, int32 Y) const
{
	FVector FL = BoardMesh->GetSocketLocation("FL");

	FVector Pos = FVector{
		-(TileSize * X + TileSize / 2.f),
		TileSize * Y + TileSize / 2.f,
		0
	};

	Pos = UKismetMathLibrary::TransformLocation(
		GetRootComponent()->GetComponentTransform(),
		Pos
	);

	return Pos + FL;
}

void AChessboard::AddSelection(AChess* Chess)
{
	SelectedChess = Chess;

	for (auto&& Tile : Tiles)
		Tile->SetType(ETileType::NoMove);

	if (Chess)
	{
		for (auto&& Move : GetChessGameState()->GetMoves())
		{
			if (Chess->GetBoardLocation() == Move.GetFrom())
			{
				ATile** Tile = Tiles.FindByPredicate([&Move](ATile* Tile)
				{
					return Tile->GetLocation() == Move.GetTo();
				});

				if (Tile)
				{
					if (Move.GetCapturedPiece() != GEmptyChessPiece.GetCode())
						(*Tile)->SetType(ETileType::Capture);
					else
						(*Tile)->SetType(ETileType::Move);
				}
			}
		}
	}
}

void AChessboard::Multicast_Move_Implementation(int32 From, int32 To)
{	
	for (auto&& Move : GetChessGameState()->GetMoves())
	{
		if (Move.GetFromTileIndex() == From && Move.GetToTileIndex() == To)
		{
			if (GetChessGameState()->MakeMove(Move))
			{
				int32 FromIdx = GetChessGameState()->GetTileAs64(From);
				int32 ToIdx = GetChessGameState()->GetTileAs64(To);

				ATile* FromTile = Tiles[FromIdx];
				ATile* ToTile = Tiles[ToIdx];

				if (HasAuthority())
				{
					UE_LOG(LogGameState, Display, TEXT("Move performed on server"));
				}
				else
				{
					UE_LOG(LogGameState, Display, TEXT("Move performed on client"));

					//Update visuals
					if (UChessGameStatics::IsCastlingMove(Move))
					{
						//TODO	
					}
					else if (UChessGameStatics::IsEnPassantMove(Move))
					{
						int32 EnPas;
						
						if (GetChessGameState()->GetSide() == EPieceColor::White)
						{
							EnPas = To + 10;
						}
						else
						{
							EnPas = To - 10;
						}
						
						EnPas = GetChessGameState()->GetTileAs64(EnPas);
						ATile* EnPasTile = Tiles[EnPas];
						
						OnMove(FromTile, ToTile, EnPasTile, Move);
					}
					else
					{
						OnMove(FromTile, ToTile, nullptr, Move);
					}
				}

				ToTile->SetPiece(FromTile->GetPiece());
				FromTile->SetPiece(nullptr);
				
				GetChessGameState()->GenerateAllMoves();
				break;
			}
		}
	}
}


void AChessboard::OnTileClicked(ATile* Tile, AChessPlayerController* Controller)
{
	if (SelectedChess)
	{
		EPieceColor MovingSide = GetChessGameState()->GetSide();
		EPieceColor PlayerSide = Controller->GetSide();


		//Whites can't move blacks 
		if (MovingSide == PlayerSide)
		{
			FTileCoord From = SelectedChess->GetBoardLocation();
			FTileCoord To = Tile->GetLocation();
			
			//Run move logic on server
			//Controller is a dirty trick
			Controller->Server_NotifyPlayerMoved(this, From.ToInt(), To.ToInt());
		}

		AddSelection(nullptr);
	}
}

// Called when the game starts or when spawned
void AChessboard::BeginPlay()
{
	Super::BeginPlay();

	GetChessGameState()->InitBoard(FEN);
	Tiles.Reserve(64);

	for (int32 i = 0; i < 8; ++i)
	{
		for (int32 j = 0; j < 8; ++j)
		{
			auto X = static_cast<EBoardRank>(i);
			auto Y = static_cast<EBoardFile>(j);

			Tiles.Emplace(GetWorld()->SpawnActor<ATile>());
			Tiles.Last()->SetActorLocation(GetTileCenter(Y, X));
			Tiles.Last()->SetLocation(FTileCoord{ Y,X });
			Tiles.Last()->SetType(ETileType::NoMove);
			Tiles.Last()->SetOwner(this);
		}
	}
	
	for (int32 i = FTileCoord::GetMaxRankIndex(); i >= FTileCoord::GetMinRankIndex(); --i)
	{
		for (int32 j = FTileCoord::GetMinFileIndex(); j <= FTileCoord::GetMaxFileIndex(); ++j)
		{
			auto X = static_cast<EBoardRank>(i);
			auto Y = static_cast<EBoardFile>(j);

			FChessPiece Piece = GetChessGameState()->GetPieceAtTile(Y, X);

			if (Piece != GEmptyChessPiece)
			{
				FRotator R = GetActorRotation();
				FTransform T = FTransform{R, GetTileCenter(Y,X)};
				
				auto Actor = GetWorld()->SpawnActorDeferred<AChess>(
						AChess::StaticClass(), T
					);

				Actor->InitPiece(Piece, FTileCoord{Y,X}, this);
				
				Actor->FinishSpawning(T);

				Tiles[GetChessGameState()->GetTileAs64(Actor->GetBoardLocation().ToInt())]->SetPiece(Actor);
			}
		}
	}

	GetChessGameState()->GenerateAllMoves();
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

