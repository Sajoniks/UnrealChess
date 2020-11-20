// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"


#include "Chess.generated.h"

class UDataTable;

USTRUCT(BlueprintType)
struct FChessMeshPath : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;
};

USTRUCT(BlueprintType)
struct FChessMaterialPath : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* Material;
};

UENUM(BlueprintType)
enum class EChessPieceRole : uint8
{
	Pawn,
	Bishop,
	Knight,
	Rook,
	Queen,
	King
};

UENUM(BlueprintType)
enum class EChessPieceColor : uint8
{
	Black,
	White
};

UCLASS()
class UNREALCHESS_API AChess : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	UDataTable* MeshPaths;

	UPROPERTY()
	UDataTable* MaterialPaths;
	
public:	
	// Sets default values for this actor's properties
	AChess();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearence")
	EChessPieceRole ChessRole;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearence")
	EChessPieceColor ChessColor;

	void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Mesh")
	UStaticMeshComponent* PieceBase;

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category = "Mesh")
	UStaticMeshComponent* PieceTop;

	void UpdateMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};