// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"

#include "Chess.generated.h"

class FChessPiece;
class AChessboard;

enum class ETileState :uint8;
enum class EChessPieceColor : uint8;
enum class EChessPieceRole : uint8;

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

UCLASS()
class UNREALCHESS_API AChess : public AActor
{
	GENERATED_BODY()

	//DT that contains references to the static meshes of the chess pieces
	UPROPERTY()
	UDataTable* MeshPaths;

	//DT that contains references to the material instances of the chess piece
	UPROPERTY()
	UDataTable* MaterialPaths;
	
public:	
	// Sets default values for this actor's properties
	AChess();

	EChessPieceRole ChessRole;
	EChessPieceColor ChessColor;

	void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category="Get")
	bool IsWhite() const;

	UFUNCTION(BlueprintCallable, Category="Get")
	bool IsBlack() const;

	//	UFUNCTION(BlueprintCallable, Category="Get")
	EChessPieceRole GetPieceRole() const;

	//Init piece from chess piece struct
	void InitPiece(const FChessPiece& Piece, AChessboard* Board);

protected:
	// Called when the game starts or when spawned
	void BeginPlay() override;

	//Base piece
	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Mesh")
	UStaticMeshComponent* PieceBase;

	//Top piece
	UPROPERTY(VisibleDefaultsOnly, Instanced, Category = "Mesh")
	UStaticMeshComponent* PieceTop;

	//Chessboard that owns this piece
	UPROPERTY()
	AChessboard* OwningChessboard;

	//Value of the piece
	int32 PieceCost;

	//Update appearance
	void UpdateMesh();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
