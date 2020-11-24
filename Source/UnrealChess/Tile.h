// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileCoordinate.h"
#include "Tile.generated.h"

class UDataTable;

UENUM(BlueprintType)
enum class ETileType : uint8
{
	Move,
	Capture,
	NoMove
};

class AChessboard;
class AChess;

UCLASS()
class UNREALCHESS_API ATile : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Appearence")
	UStaticMeshComponent* TileMesh;

	FTileCoord Coord;

	UPROPERTY()
	UDataTable* TileMaterials;

	UPROPERTY()
	AChess* Chess;
	
public:	
	// Sets default values for this actor's properties
	ATile();

	UFUNCTION(BlueprintCallable, Category="Set")
	void SetType(ETileType Type);
	
	void SetLocation(const FTileCoord& Coordinate);

	UFUNCTION(BlueprintCallable, Category = "Set")
	void SetPiece(AChess* NewPiece);
	
	const FTileCoord& GetLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Get")
	AChess* GetPiece() const;

	UFUNCTION(BlueprintCallable, Category = "Get")
	AChessboard* GetOwningBoard() const;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
