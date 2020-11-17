// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessPiece.generated.h"

UENUM(BlueprintType)
enum class EPieceType : uint8
{
	Pawn = 0,
	Horse,
	Bishop,
	Rook,
	Queen,
	King
};

UCLASS(Blueprintable)
class UNREALCHESS_API AChessPiece : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Visual")
	class UStaticMeshComponent* BottomPiece;

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Visual")
	class UStaticMeshComponent* TopPiece;

	UPROPERTY(EditDefaultsOnly, Category="Appearance")
	EPieceType PieceType;
	
public:	
	// Sets default values for this actor's properties
	AChessPiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
