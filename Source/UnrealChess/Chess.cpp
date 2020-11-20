// Fill out your copyright notice in the Description page of Project Settings.


#include "Chess.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AChess::AChess()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PieceBase = CreateDefaultSubobject<UStaticMeshComponent>("Base");
	PieceTop = CreateDefaultSubobject<UStaticMeshComponent>("Top piece");

	SetRootComponent(PieceBase);
	PieceTop->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

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

	UpdateMesh();
}

void AChess::OnConstruction(const FTransform& Transform)
{
	UpdateMesh();
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

