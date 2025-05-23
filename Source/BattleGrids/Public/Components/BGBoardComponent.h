// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BGBoardComponent.generated.h"

class ABGActor;
class ABGTile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEGRIDS_API UBGBoardComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBGBoardComponent();

	/* Build out a new grid of tiles.
	* Will not execute if BoardTiles array is not empty. */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGBoard|Functions")
	void BuildBoard(FVector const& CenteredLocation, int const& X, int const& Y);

	/* Shrinks board in both X and Y axes to the specified new X,Y size */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGBoard|Functions")
	void ShrinkBoard(int const& X, int const& Y);

	/* Grows board in both X and Y axes to the specified new X,Y size */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGBoard|Functions")
	void GrowBoard(int const& X, int const& Y);

	///////////////////
	/// Getters

	FVector2D GetBoardSize() const { return BoardSize; }

	int GetZedLevel() const { return ZedLevel; }

	TArray<ABGActor*> GetBoardTiles() const { return BoardTiles; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGBoard|Config")
	FVector2D BoardSize;
	
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGBoard|Config")
	int ZedLevel{};

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGBoard|Config")
	TArray<ABGActor*> BoardTiles;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGBoard|Config")
	UStaticMesh* TileStaticMesh;
};
