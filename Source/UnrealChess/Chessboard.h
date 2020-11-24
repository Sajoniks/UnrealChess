#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "ChessGameState.h"
#include "ChessMove.h"

#include "Chessboard.generated.h"

class AChessPlayerController;
class AChess;

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
	
	//Sets default values for this actor's properties
	AChessboard();

	/** Get world tile center location
	 * @param File File coordinate of the board
	 * @param Rank Rank coordinate of the board
	 * @return Location of the center of the tile, in world space
	 */
	FVector GetTileCenter(EBoardFile File, EBoardRank Rank) const;

	/** Get world tile center location
	 * @param X File coordinate of the board
	 * @param Y Rank coordinate of the board
	 * @return Location of the center of the tile, in world space
	 */
	UFUNCTION(BlueprintCallable, Category="Get")
	FVector GetTileCenter(int32 X, int32 Y) const;

	//Add chess piece to focus
	UFUNCTION(BlueprintCallable, Category="Set")
	void AddSelection(AChess* Chess);

	UFUNCTION(BlueprintCallable, Category = "Get")
	AChess* GetSelection() const;

	//Event called on tile click
	UFUNCTION(BlueprintCallable, Category="Action")
	void OnTileClicked(ATile* Tile, AChessPlayerController* Controller);

	//FEN
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Board state")
	FString FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	//Perform move
	//Called from controller
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Move(int32 From, int32 To);
	
protected:
	
	//Called when piece moved
	UFUNCTION(BlueprintImplementableEvent, Category="Event")
	void OnMove(ATile* From, ATile* To, ATile* EnPas, const FChessMove& Move);

	//Called on castling move
	UFUNCTION(BlueprintImplementableEvent, Category="Event")
	void OnCastlingMove(ATile* King, ATile* NewKing, ATile* Piece, ATile* NewPiece);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearence", meta=(ClampMin="10"))
	//Size of the side of the tile in uu
	int32 TileSize;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Array of tile actors
	UPROPERTY()
	TArray<ATile*> Tiles;
	
	UFUNCTION(BlueprintCallable, Category="Get")
	AChessGameState* GetChessGameState() const;

	//Chess in focus
	UPROPERTY()
	AChess* SelectedChess;

	//Draw debug lines
	void DrawDebug();

	//Construction script
	void OnConstruction(const FTransform& Transform) override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
