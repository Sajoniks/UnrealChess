// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile.h"

#include "Chessboard.h"
#include "Chess.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Tile");
	SetRootComponent(TileMesh);

	ConstructorHelpers::FObjectFinder<UDataTable> DTFinder(TEXT("DataTable'/Game/Blueprint/DT_TileMaterialAssets.DT_TileMaterialAssets'"));
	if (DTFinder.Succeeded())
	{
		TileMaterials = DTFinder.Object;
	}

	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("StaticMesh'/Game/Models/SM_Tile.SM_Tile'"));
	TileMesh->SetStaticMesh(MeshFinder.Object);
}

void ATile::SetType(ETileType Type)
{
	check(TileMaterials)
	UMaterialInstance* Mat = nullptr;

	switch(Type)
	{
		
	case ETileType::Move: 
		Mat = TileMaterials->FindRow<FChessMaterialPath>("Move", "")->Material;
		TileMesh->SetMaterial(0, Mat);

		GetRootComponent()->SetVisibility(true, true);
		break;
		
	case ETileType::Capture:
		Mat = TileMaterials->FindRow<FChessMaterialPath>("Capture", "")->Material;
		TileMesh->SetMaterial(0, Mat);
		
		GetRootComponent()->SetVisibility(true, true);
		break;
		
	case ETileType::NoMove:
		GetRootComponent()->SetVisibility(false, true);
	}
}

void ATile::SetLocation(const FTileCoordinate& Coordinate)
{
	this->Coord = Coordinate;
}

void ATile::SetPiece(AChess* NewChess)
{
	this->Chess = NewChess;
	
	if (Chess)
		Chess->SetBoardLocation(GetLocation());
}

const FTileCoordinate& ATile::GetLocation() const
{
	return Coord;
}

AChess* ATile::GetPiece() const
{
	return Chess;
}

AChessboard* ATile::GetOwningBoard() const
{
	return Cast<AChessboard>(GetOwner());
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

