// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess.h"
#include "UObject/ConstructorHelpers.h"
#include "ChessDefinitions.h"

// Sets default values
AChess::AChess()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PieceBase = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	PieceTop = CreateDefaultSubobject<UStaticMeshComponent>("Top piece");

	SetRootComponent(PieceBase);
	PieceTop->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	PieceTop->SetRelativeRotation(FRotator{0, -90, 0});
	
	ConstructorHelpers::FObjectFinder<UDataTable> MeshFinder{ TEXT("DataTable'/Game/Blueprint/DT_ChessMeshAssets.DT_ChessMeshAssets'") };
	ConstructorHelpers::FObjectFinder<UDataTable> MaterialFinder{ TEXT("DataTable'/Game/Blueprint/DT_ChessMaterialAssets.DT_ChessMaterialAssets'") };

	if (MeshFinder.Succeeded() && MaterialFinder.Succeeded())
	{
		MeshPaths = MeshFinder.Object;
		MaterialPaths = MaterialFinder.Object;

		const auto Row = MeshPaths->FindRow<FChessMeshPath>("Base", "", false);
		if (!Row)
		{
			UE_LOG(LogActor, Error, TEXT("Failed to set mesh for chess piece."))
		}
		else
		{
			PieceBase->SetStaticMesh(Row->Mesh);
		}
	}
	else
	{
		UE_LOG(LogActor, Error, TEXT("Failed to found assets for chess piece. Check references in chess class."));
	}
	
	ChessColor = EChessPieceColor::White;
	ChessRole = EChessPieceRole::Pawn;
}

void AChess::OnConstruction(const FTransform& Transform)
{
	UpdateMesh();
}

bool AChess::IsWhite() const
{
	return ChessColor == EChessPieceColor::White;
}

bool AChess::IsBlack() const
{
	return !IsWhite();
}

AChessboard* AChess::GetOwningBoard() const
{
	return OwningChessboard;
}

EChessPieceRole AChess::GetPieceRole() const
{
	return ChessRole;
}

void AChess::InitPiece(const FChessPiece& Piece, const FTileCoord& Coord, AChessboard* Board)
{
	this->Location = Coord;
	
	switch(Piece.GetColor())
	{
	case EPieceColor::White:
		ChessColor = EChessPieceColor::White;
		break;

	case EPieceColor::Black:
		ChessColor = EChessPieceColor::Black;
		break;

	default:
		UE_LOG(LogActor, Warning, TEXT("Invalid parameter in chess piece initalization: %s"), *UEnum::GetValueAsString(Piece.GetColor()));
		return;
	}

	switch(Piece.GetEnum())
	{
	case ETileState::WhitePawn: case ETileState::BlackPawn:
		ChessRole = EChessPieceRole::Pawn;
		break;

	case ETileState::WhiteKnight: case ETileState::BlackKnight:
		ChessRole = EChessPieceRole::Knight;
		break;

	case ETileState::WhiteBishop: case ETileState::BlackBishop:
		ChessRole = EChessPieceRole::Bishop;
		break;

	case ETileState::WhiteRook: case ETileState::BlackRook:
		ChessRole = EChessPieceRole::Rook;
		break;

	case ETileState::WhiteQueen: case ETileState::BlackQueen:
		ChessRole = EChessPieceRole::Queen;
		break;

	case ETileState::WhiteKing: case ETileState::BlackKing:
		ChessRole = EChessPieceRole::King;
		break;
		
	default:

		UE_LOG(LogActor, Warning, TEXT("Invalid parameter in chess piece initalization: %s"), *UEnum::GetValueAsString(Piece.GetEnum()));
		return;
	}

	//Rotate piece towards other side
	if (ChessColor == EChessPieceColor::Black)
		AddActorWorldRotation(FRotator{ 0, 180, 0 });

	OwningChessboard = Board;
	PieceCost = Piece.GetCost();

	UpdateMesh();
}

const FTileCoord& AChess::GetBoardLocation() const
{
	return Location;
}

void AChess::SetBoardLocation(const FTileCoord& NewLocation)
{
	this->Location = NewLocation;
}

// Called when the game starts or when spawned
void AChess::BeginPlay()
{
	Super::BeginPlay();
}

void AChess::UpdateMesh()
{
	if (MaterialPaths && MeshPaths)
	{
		//Find mesh in DT
		FName RoleName = UEnum::GetValueAsName(ChessRole);
		const auto Row = MeshPaths->FindRow<FChessMeshPath>(RoleName, "", false);
		if (Row)
			PieceTop->SetStaticMesh(Row->Mesh);
		
		//Find material in DT
		FName MaterialName = UEnum::GetValueAsName(ChessColor);
		const auto MatRow = MaterialPaths->FindRow<FChessMaterialPath>(MaterialName, "", false);

		if (MatRow)
		{
			PieceBase->SetMaterial(0, MatRow->Material);
			PieceTop->SetMaterial(0, MatRow->Material);
		}
	}
}

// Called every frame
void AChess::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

