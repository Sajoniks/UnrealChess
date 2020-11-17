#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chessboard.generated.h"

class AChessGameState;

/*
 * Actor that represents chessboard
 */

UCLASS(Blueprintable)
class UNREALCHESS_API AChessboard : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	AChessboard();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearence", meta=(ClampMin="10"))
	//Size of the side of the tile in uu
	int32 TileSize;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//
	FORCEINLINE AChessGameState* GetChessGameState() const;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
