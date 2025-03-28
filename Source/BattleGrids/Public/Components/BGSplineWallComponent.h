// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Components/SplineComponent.h"
#include "Core/BGTypes.h"

#include "BGSplineWallComponent.generated.h"

class ABGStructure;
class UBoxComponent;
class USplineComponent;

UCLASS(ClassGroup=(BattleGrids), meta=(BlueprintSpawnableComponent))
class BATTLEGRIDS_API UBGSplineWallComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBGSplineWallComponent();

	////////////////////////
	/// Public Functions

	void Initialize();

	/**
	 * Sets up variables in the Spline Component, makes all spline points linear for Server and Clients
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void SetupSplineComponentVariables();

	/**
	 * Primary function which cleans up all Instances and rebuilds all InstancedStaticMeshComponents for all Splines
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void UpdateSplineStructureMesh(bool const bCleanUpMeshes = false);

	/**
	 * Removes Instance mesh at Index from specified InstancedStaticMeshComponent name
	 * This adds a new InstancedMeshComponent, or Instance to an existing Component, 
	 * of supplied StaticMesh and MaterialInstance
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void ModifyInstanceMeshAtIndex(int const& Index, FString const& NewInstanceName, UStaticMesh* StaticMesh,
	                               UMaterialInstance* MaterialInstance,
	                               FString const& OldInstanceName = "WallInstance");

	/**
	 * Spawns a BGStructure Actor at the provided Index
	 * This removes an Instance from the InstancedStaticMeshComponent
	 */
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void SpawnStructureActorAtIndex(int const& Index, TSubclassOf<ABGStructure> StructureClassToSpawn,
	                                FTransform const& SpawnTransform,
	                                FString const& OldInstanceName);

	/**
	 * Restores an Instance Mesh
	 * By default it is the WallInstance that is restored
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void RestoreInstanceMeshAtIndex(int const& Index, FTransform const& Transform,
	                                FString const& InstanceName = "WallInstance");

	/**
	 * Removes the Instance at the provided Index
	 * By default it removes from the WallInstance
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void RemoveInstanceMeshAtIndex(int const& Index, FString const& InstanceName = "WallInstance");

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void SetLocationOfSplinePoint(int const& PointIndex, FVector const& NewLocation);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void AddPointToSpline(FVector const& Location, float const& Key);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void AddPointToSplineAtIndex(FVector const& Location, int const& Index);

	UFUNCTION(BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void RemoveStructureFromArray(ABGStructure* StructureToRemove);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category= "BGSplineWallComponent|Functions")
	void ResetSplineWallComponent();

	///////////////////////
	/// Getters

	USplineComponent* GetSplineComponent() const { return SplineComponent; }

	/**
	* Searches the Actor's Instances array for an Instance component with the ObjectName matching the provided string.
	* Returns a nullptr if no match is found.
	*/
	UFUNCTION(BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	UInstancedStaticMeshComponent* GetInstancedStaticMeshComponentByTag(FString const& Tag) const;

	FBGStructureInfo GetStructureData() const { return StructureData; }

	FBGWallSplineSaveInfo GetWallSplineSaveInfo() const { return WallSplineSaveInfo; }

	TArray<ABGStructure*> GetSpawnedStructures() const { return SpawnedStructures; }

	///////////////////////
	/// Setters

	UFUNCTION(BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	void SetStructureInfo(FBGStructureInfo const NewStructureInfo);

	void SetWallSplineSaveInfo(FBGWallSplineSaveInfo const InWallSplineSaveInfo);

	void SetSplineCurves(FSplineCurves const InSplineCurves);

	void UpdateWallSplineSaveInfo();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void CleanupInstanceComponents();

	void UpdateWallMesh();

	void UpdateCornerMesh();

	void CreateBoxCollision(UInstancedStaticMeshComponent* InstancedStaticMeshComponent, FTransform const& BaseTransform);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, FHitResult const& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                  int32 OtherBodyIndex);

	UFUNCTION()
	void OnRep_SplineComponent();

	/** Creates an InstancedStaticMeshComponent with the provided Name, Mesh, and Material */
	UFUNCTION(BlueprintCallable, Category = "BGSplineWallComponent|Functions")
	UInstancedStaticMeshComponent* CreateInstancedStaticMeshComponent(FString const& InstanceTag,
	                                                                  UStaticMesh* StaticMesh,
	                                                                  UMaterialInstance* MaterialInstance);

	//////////////////////
	/// Components

	UPROPERTY(ReplicatedUsing=OnRep_SplineComponent, EditAnywhere, BlueprintReadWrite, Category = "Components")
	USplineComponent* SplineComponent;

	////////////////////
	/// Member Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGSplineWallComponent|Config")
	TArray<UBoxComponent*> CollisionBoxes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BGSplineWallComponent|Config")
	TArray<UBoxComponent*> DisabledCollisionBoxes;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "BGSplineWallComponent|Config")
	TArray<ABGStructure*> SpawnedStructures;

	/** Stores the main structural meshes and material information */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "BGSplineWallComponent|Config")
	FBGStructureInfo StructureData;

	UPROPERTY(Replicated, VisibleAnywhere, Category = "BGSplineWallComponent|Config")
	int UniqueCollisionNumber{};

	UPROPERTY(Replicated, VisibleAnywhere, Category = "BGSplineWallComponent|Config")
	int UniqueInstanceNumber{};

	UPROPERTY(Replicated)
	FBGWallSplineSaveInfo WallSplineSaveInfo{};
};
