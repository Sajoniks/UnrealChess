#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

enum class EBoardFile : uint8;
enum class EBoardRank : uint8;

class AChessGameState;
class UArrowComponent;

/*
 * Actor that represents chessboard
 */

UCLASS(Blueprintable)
class UNREALCHESS_API AChessboard : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Appearence")
	UStaticMeshComponent* BoardMesh;

	UPROPERTY(VisibleDefaultsOnly, Instanced, Category = "Appearence")
	UArrowComponent* Arrow;
	
public:
	
	// Sets default values for this actor's properties
	AChessboard();

	FVector GetTileCenter(EBoardFile File, EBoardRank Rank) const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearence", meta=(ClampMin="10"))
	//Size of the side of the tile in uu
	int32 TileSize;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	UFUNCTION(BlueprintCallable, Category="Get")
	AChessGameState* GetChessGameState() const;

	void DrawDebug();

	void OnConstruction(const FTransform& Transform) override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
