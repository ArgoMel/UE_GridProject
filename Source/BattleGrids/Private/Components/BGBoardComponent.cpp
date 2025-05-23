// © 2021 Matthew Barham. All Rights Reserved.


#include "Components/BGBoardComponent.h"

#include "Actors/BGActor.h"
#include "Components/BGTileComponent.h"
#include "Core/Gameplay/BGGameplayGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBGBoardComponent::UBGBoardComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UBGBoardComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBGBoardComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBGBoardComponent, BoardTiles)
	DOREPLIFETIME(UBGBoardComponent, BoardSize)
	DOREPLIFETIME(UBGBoardComponent, ZedLevel)
	DOREPLIFETIME(UBGBoardComponent, TileStaticMesh)
}

void UBGBoardComponent::BuildBoard_Implementation(FVector const& CenteredLocation, int const& X, int const& Y)
{
	// Do not execute if BoardTiles contains Tiles already
	if (BoardTiles.Num() == 0)
	{
		BoardSize.X = X;
		BoardSize.Y = Y;

		// 2D Grid Execution Macro, rewritten in C++
		for (auto OuterIndex{0}; OuterIndex <= Y - 1; ++OuterIndex)
		{
			for (auto InnerIndex{0}; InnerIndex <= X - 1; ++InnerIndex)
			{
				float constexpr SectorSize = 100.f;
				// Prepare a fresh Transform
				FVector SpawnLocation =
					FVector(static_cast<float>(InnerIndex) - static_cast<float>(X) / 2.f,
					        static_cast<float>(OuterIndex) - static_cast<float>(Y) / 2.f,
					        0.f)
					* SectorSize
					* 1.f
					+ CenteredLocation;

				FTransform SpawnTransform;
				SpawnTransform.SetLocation(SpawnLocation);

				// Prepare a Tile to spawn
				auto TileToSpawn = Cast<ABGGameplayGameModeBase>(
						UGameplayStatics::GetGameMode(this))->
					CreateNewActor(EBGActorType::Tile);

				auto TileComponent = Cast<UBGTileComponent>(TileToSpawn->
					GetComponentByClass(UBGTileComponent::StaticClass()));

				if (TileToSpawn && TileComponent)
				{
					TileComponent->SetBoardReference(Cast<ABGActor>(GetOwner()));
					TileComponent->SetTileInfo(FBGTileInfo(InnerIndex, OuterIndex, 0));
					if (TileStaticMesh)
					{
						TileComponent->SetTileStaticMesh(TileStaticMesh);
					}
					TileToSpawn->FinishSpawning(SpawnTransform);
				}

				// Spawn the new Tile and add it to the board's array
				BoardTiles.Add(TileToSpawn);
				TileToSpawn->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}
}

void UBGBoardComponent::GrowBoard_Implementation(int const& X, int const& Y)
{
	TArray<ABGActor*> NewTileArray;

	// Grows board in the X-axis
	for (const auto CurrentRowTile : BoardTiles)
	{
		auto const CurrentRowTileComponent = Cast<UBGTileComponent>(CurrentRowTile->GetComponentByClass(UBGTileComponent::StaticClass()));

		// Check if Tile is valid and not pending kill, if it's the current final X-row,
		// and its X is less than the new final X-row
		if (CurrentRowTile->IsValidLowLevel() && !CurrentRowTile->IsPendingKillPending()
			&& CurrentRowTileComponent
			&& CurrentRowTileComponent->GetTileInfo().X == BoardSize.X - 1
			&& CurrentRowTileComponent->GetTileInfo().X < X)
		{
			// Prepare the new Tile's transform location to be 100cm spaced beyond the current Tile in the X-axis
			FTransform SpawnTransform = CurrentRowTile->GetActorTransform();
			SpawnTransform.SetLocation(SpawnTransform.GetLocation() + FVector(100.f, 0, 0));
			// reset the scale to 1,1,1 to prevent quartering the copied 0.25f Z-axis off the reference Tile's transform
			SpawnTransform.SetScale3D(FVector::OneVector);

			// Prepare a Tile to spawn
			auto TileToSpawn = Cast<ABGGameplayGameModeBase>(
					UGameplayStatics::GetGameMode(this))->
				CreateNewActor(EBGActorType::Tile);

			auto NewTileComponent = Cast<UBGTileComponent>(TileToSpawn->
				GetComponentByClass(UBGTileComponent::StaticClass()));

			// Set the deferred Tile's properties
			if (TileToSpawn && NewTileComponent)
			{
				NewTileComponent->SetBoardReference(Cast<ABGActor>(GetOwner()));
				NewTileComponent->SetTileInfo(FBGTileInfo(CurrentRowTileComponent->GetTileInfo().X + 1,
				                                          CurrentRowTileComponent->GetTileInfo().Y,
				                                          CurrentRowTileComponent->GetTileInfo().Z));
				if (TileStaticMesh)
				{
					NewTileComponent->SetTileStaticMesh(TileStaticMesh);
				}
				TileToSpawn->FinishSpawning(SpawnTransform);
				NewTileArray.Add(TileToSpawn);
				TileToSpawn->AttachToActor(Cast<ABGActor>(GetOwner()), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}

	BoardTiles.Append(NewTileArray);
	NewTileArray.Empty();
	BoardSize.X = X;

	// Grows board in the Y-axis
	for (const auto CurrentColumnTile : BoardTiles)
	{
		auto const CurrentColumnTileComponent = Cast<UBGTileComponent>(CurrentColumnTile->GetComponentByClass(UBGTileComponent::StaticClass()));

		// Check if Tile is valid and not pending kill,
		// then check if it's the current final X-row, and its X is less than the new final X-row
		if (CurrentColumnTile->IsValidLowLevel() && !CurrentColumnTile->IsPendingKillPending()
			&& CurrentColumnTileComponent
			&& CurrentColumnTileComponent->GetTileInfo().Y == BoardSize.Y - 1
			&& CurrentColumnTileComponent->GetTileInfo().Y < Y)
		{
			// Prepare the new Tile's transform location to be 100cm spaced beyond the current Tile in the Y-axis
			FTransform SpawnTransform = CurrentColumnTile->GetActorTransform();
			SpawnTransform.SetLocation(SpawnTransform.GetLocation() + FVector(0, 100.f, 0));
			// reset the scale to 1,1,1 to prevent quartering the copied 0.25f Z-axis off the reference Tile's transform
			SpawnTransform.SetScale3D(FVector::OneVector);

			// Prepare a Tile to spawn
			auto TileToSpawn = Cast<ABGGameplayGameModeBase>(
					UGameplayStatics::GetGameMode(this))->
				CreateNewActor(EBGActorType::Tile);

			const auto NewTileComponent = Cast<UBGTileComponent>(TileToSpawn->
				GetComponentByClass(UBGTileComponent::StaticClass()));

			// Set the deferred Tile's properties
			if (TileToSpawn && NewTileComponent)
			{
				NewTileComponent->SetBoardReference(Cast<ABGActor>(GetOwner()));
				NewTileComponent->SetTileInfo(FBGTileInfo(CurrentColumnTileComponent->GetTileInfo().X,
				                                          CurrentColumnTileComponent->GetTileInfo().Y + 1,
				                                          CurrentColumnTileComponent->GetTileInfo().Z));
				if (TileStaticMesh)
				{
					NewTileComponent->SetTileStaticMesh(TileStaticMesh);
				}
				TileToSpawn->FinishSpawning(SpawnTransform);
				NewTileArray.Add(TileToSpawn);
				TileToSpawn->AttachToActor(Cast<ABGActor>(GetOwner()), FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}

	BoardTiles.Append(NewTileArray);
	BoardSize.Y = Y;
}

void UBGBoardComponent::ShrinkBoard_Implementation(int const& X, int const& Y)
{
	int TilesToRemoveCount{};

	for (const auto Tile : BoardTiles)
	{
		auto const TileComponent = Cast<UBGTileComponent>(Tile->GetComponentByClass(UBGTileComponent::StaticClass()));

		// Check if Tile is valid and not pending kill,
		// then check if its X or Y is greater than the incoming shrink size
		if (Tile->IsValidLowLevel() && !Tile->IsPendingKillPending()
			&& TileComponent
			&& TileComponent->GetTileInfo().X > X - 1 || TileComponent->GetTileInfo().Y > Y - 1)
		{
			Tile->Destroy();
			++TilesToRemoveCount;
		}
	}

	// After tiles have been marked to kill, clean out the BoardTiles array
	// with a reverse for loop to as to not encounter off-by-one errors
	for (int i{BoardTiles.Num() - 1}; TilesToRemoveCount != 0; --i)
	{
		if (BoardTiles[i]->IsPendingKillPending())
		{
			BoardTiles.RemoveAt(i);
			--TilesToRemoveCount;
		}
	}

	BoardSize.X = X;
	BoardSize.Y = Y;
}
