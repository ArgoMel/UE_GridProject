// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/BGTypes.h"

#include "BGTileComponent.generated.h"

class ABGActor;
class ABGBoard;

UCLASS(ClassGroup=(BattleGrids), meta=(BlueprintSpawnableComponent))
class BATTLEGRIDS_API UBGTileComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBGTileComponent();

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGTile|Functions")
	void ToggleTileVisibility(bool const bIsVisible);

	UFUNCTION(BlueprintCallable, Category = "BGTile|Functions")
	bool GetTileVisibility() const;

	///////////////////
	/// Getters

	FBGTileInfo GetTileInfo() const { return TileInfo; }

	class UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

	ABGActor* GetBoardReference() const { return BoardActorReference; }

	///////////////////
	/// Setters

	void SetTileInfo(FBGTileInfo const& NewTileInfo) { TileInfo = NewTileInfo; }
	void SetBoardReference(ABGActor* NewBoard) { BoardActorReference = NewBoard; }
	void SetTileStaticMesh(UStaticMesh* InStaticMesh) { TileStaticMesh = InStaticMesh; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config")
	UStaticMesh* TileStaticMesh;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Config")
	ABGActor* BoardActorReference;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"), Category = "Config")
	FBGTileInfo TileInfo;
};
