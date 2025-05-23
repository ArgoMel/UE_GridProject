// © 2021 Matthew Barham. All Rights Reserved.

#include "Core/Gameplay/BGGameplayGameModeBase.h"
#include "AIController.h"
#include "Actors/BGStructure.h"
#include "Characters/BGCharacter.h"
#include "Components/BGBoardComponent.h"
#include "Components/BGSplineWallComponent.h"
#include "Components/BGTileComponent.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Core/BGGameInstance.h"
#include "Core/Gameplay/BGGamePlayerController.h"
#include "Core/Gameplay/BGGameplayGameStateBase.h"
#include "Core/Save/BGGameSave.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"

ABGGameplayGameModeBase::ABGGameplayGameModeBase()
{
}

void ABGGameplayGameModeBase::SetupLoadedGameData(UBGGameSave* InGameSave)
{
	if (InGameSave)
	{
		/** Rebuild Character data on board */
		auto CharacterArray = InGameSave->GetSavedCharacterModelInfoArray();
		for (auto Character : CharacterArray)
		{
			SpawnCharacterAtLocation(
				Character.CharacterLocation,
				Character.CharacterModelStaticMeshName,
				Character.CharacterModelMaterialName);
		}

		/** Rebuild SplineWall data on board */
		auto SplineWallArray = InGameSave->GetSavedSplineWallInfoArray();
		for (auto WallSplineSaveInfo : SplineWallArray)
		{
			auto const SplineWallActor = InitializeSplineWallActor(
				WallSplineSaveInfo.WallStaticMeshName,
				WallSplineSaveInfo.WallMaskedMaterialInstanceName,
				WallSplineSaveInfo.CornerStaticMeshName,
				WallSplineSaveInfo.CornerMaskedMaterialInstanceName,
				WallSplineSaveInfo.BaseStaticMeshName,
				WallSplineSaveInfo.BaseMaterialInstanceName,
				WallSplineSaveInfo.Location);

			if (SplineWallActor)
			{
				auto SplineWallComponent = Cast<UBGSplineWallComponent>(
					SplineWallActor->GetComponentByClass(UBGSplineWallComponent::StaticClass()));

				if (SplineWallComponent)
				{
					SplineWallComponent->SetWallSplineSaveInfo(WallSplineSaveInfo);
					SplineWallComponent->SetSplineCurves(WallSplineSaveInfo.SplineCurves);
					SplineWallComponent->UpdateSplineStructureMesh(true);
				}
			}
		}
	}
}

void ABGGameplayGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	auto const GameInstance = GetGameInstance<UBGGameInstance>();
	if (GameInstance && GameInstance->GetLoading())
	{
		UE_LOG(LogTemp, Warning, TEXT("Loading Game Data"))
		auto const GameSave = GameInstance->GetGameSave();
		SetupLoadedGameData(GameSave);
	}
}

void ABGGameplayGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ABGGameplayGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UE_LOG(LogTemp, Warning, TEXT("PostLogin"))

	/**
	 * This handles spawning for a player who connects mid-session
	 */
	auto const BGPlayerController = Cast<ABGGamePlayerController>(NewPlayer);
	if (BGPlayerController)
	{
		BGPlayerController->Spawn();
	}
}

void ABGGameplayGameModeBase::HandleSeamlessTravelPlayer(AController*& C)
{
	Super::HandleSeamlessTravelPlayer(C);

	/*
	 * This handles spawning for players who travel from the Lobby
	 */
	auto const BGPlayerController = Cast<ABGGamePlayerController>(C);
	if (BGPlayerController)
	{
		BGPlayerController->Spawn();
	}

	UE_LOG(LogTemp, Warning, TEXT("Handle Seamless Travel Player"))
}

ABGActor* ABGGameplayGameModeBase::CreateNewActor(EBGActorType const& ActorType)
{
	ABGActor* NewActor{};

	switch (ActorType)
	{
	case EBGActorType::None: break;

	case EBGActorType::Structure:
		NewActor = CreateNewSplineWallActor();
		break;

	case EBGActorType::Tile:
		NewActor = CreateNewTileActor();
		break;

	case EBGActorType::Board: break;

	case EBGActorType::Light: break;

	default: ;
	}

	/** Add the newly spawned Actor to the GameState's array */
	if (NewActor && GameState)
	{
		auto BGGameState = Cast<ABGGameplayGameStateBase>(GameState);
		if (BGGameState)
		{
			BGGameState->AddSpawnedEntityToArray(NewActor);
		}
		return NewActor;
	}

	return nullptr;
}

ABGActor* ABGGameplayGameModeBase::CreateNewSplineWallActor()
{
	FName Tag("Spline Wall");
	auto const NewStructureActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(
		this, ABGActor::StaticClass(), FTransform::Identity);
	auto CastNewStructure = Cast<ABGActor>(NewStructureActor);
	if (CastNewStructure)
	{
		CastNewStructure->SetActorType(EBGActorType::Structure);

		auto SplineWallComponent = NewObject<UBGSplineWallComponent>(
			CastNewStructure,
			UBGSplineWallComponent::StaticClass(),
			TEXT("Spline Wall Component"));

		SplineWallComponent->CreationMethod = EComponentCreationMethod::Instance;
		CastNewStructure->SetupComponentSlot(SplineWallComponent);
		CastNewStructure->Tags.Add(Tag);

		return CastNewStructure;
	}

	return nullptr;
}

ABGActor* ABGGameplayGameModeBase::CreateNewTileActor() const
{
	auto const NewTileActor = UGameplayStatics::BeginDeferredActorSpawnFromClass(
		this, ABGActor::StaticClass(), FTransform::Identity);
	auto CastNewTile = Cast<ABGActor>(NewTileActor);
	if (CastNewTile)
	{
		CastNewTile->SetActorType(EBGActorType::Tile);

		auto TileComponent = NewObject<UBGTileComponent>(
			CastNewTile,
			UBGTileComponent::StaticClass(),
			TEXT("Tile Component"));

		TileComponent->CreationMethod = EComponentCreationMethod::Instance;
		CastNewTile->SetupComponentSlot(TileComponent);
		CastNewTile->Tags.Add(FName("Tile"));

		return CastNewTile;
	}
	return nullptr;
}

TArray<FName> ABGGameplayGameModeBase::GetRowNamesOfType(EBGClassCategory const& ClassCategory) const
{
	TArray<FName> Names{};

	for (auto const It : StaticMeshBank->GetRowMap())
	{
		if (auto const Struct = reinterpret_cast<FBGStaticMeshBank*>(It.Value))
		{
			if (Struct->ClassCategory == ClassCategory)
			{
				auto const NewString = Struct->StaticMeshName.ToString();
				auto const NewName = FName(*NewString);
				Names.AddUnique(NewName);
			}
		}
	}

	return Names;
}

void ABGGameplayGameModeBase::DestroyGameActor(AActor* ActorToDestroy)
{
	if (ActorToDestroy)
	{
		ActorToDestroy->Destroy();
	}
}

void ABGGameplayGameModeBase::SpawnCharacterAtLocation(FVector const& Location, FName const& MeshName,
                                                       FName const& MaterialName)
{
	if (StaticMeshBank && MaterialInstanceBank)
	{
		auto const ModelStaticMeshRow = StaticMeshBank->FindRow<FBGStaticMeshBank>(MeshName, "");
		auto const MaterialInstanceRow = MaterialInstanceBank->FindRow<FBGMaterialInstanceBank>(MaterialName, "");
		auto const TokenBaseStaticMeshRow = StaticMeshBank->FindRow<FBGStaticMeshBank>("Simple_Base_Round_01", "");

		if (ModelStaticMeshRow && MaterialInstanceRow && TokenBaseStaticMeshRow)
		{
			FTransform NewTransform{};
			NewTransform.SetLocation(Location);

			auto const CharacterToSpawn = Cast<ABGCharacter>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
				this, ABGCharacter::StaticClass(), NewTransform,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn,
				this));

			CharacterToSpawn->InitializeMeshAndMaterial(ModelStaticMeshRow->StaticMesh, MaterialInstanceRow->MaterialInstance,
			                                            TokenBaseStaticMeshRow->StaticMesh);

			CharacterToSpawn->GetTokenModelStaticMeshComponent()->SetSimulatePhysics(true);

			FBGCharacterModelSaveInfo const NewSaveInfo(
				UniqueCharacterID++, FName(), Location,
				MeshName, MaterialName);

			CharacterToSpawn->SetCharacterModelSaveInfo(NewSaveInfo);

			CharacterToSpawn->FinishSpawning(NewTransform);

			/** Add newly spawned Character to GameState array */
			if (GameState)
			{
				auto BGGameState = Cast<ABGGameplayGameStateBase>(GameState);
				if (BGGameState)
				{
					BGGameState->AddSpawnedEntityToArray(CharacterToSpawn);
				}
			}
		}
	}
}

void ABGGameplayGameModeBase::MoveCharacterToLocation(ABGCharacter* CharacterToMove, FVector const& Location,
                                                      FRotator const& ActorRotation)
{
	if (CharacterToMove)
	{
		if (CharacterToMove->GetIsTokenLocked())
		{
			return;
		}

		CharacterToMove->SetActorRotation(ActorRotation, ETeleportType::ResetPhysics);
		CharacterToMove->GetCharacterAIController()->MoveToLocation(
			Location,
			.1f,
			false,
			true,
			true,
			true,
			nullptr,
			false);
	}
}

void ABGGameplayGameModeBase::ToggleCharacterLockInPlace(ABGCharacter* CharacterToToggle, bool const bLock)
{
	if (CharacterToToggle)
	{
		CharacterToToggle->ToggleLockTokenInPlace(bLock);
	}
}

void ABGGameplayGameModeBase::ToggleCharacterPermissionsForPlayer(ABGPlayerState* PlayerStateToToggle,
                                                                  ABGCharacter* CharacterToToggle)
{
	if (PlayerStateToToggle && CharacterToToggle)
	{
		CharacterToToggle->PlayerHasPermissions(PlayerStateToToggle)
			? CharacterToToggle->RemovePlayerFromPermissionsArray(PlayerStateToToggle)
			: CharacterToToggle->AddPlayerToPermissionsArray(PlayerStateToToggle);
	}
}

ABGActor* ABGGameplayGameModeBase::InitializeSplineWallActor(FName const& WallStaticMeshName,
                                                             FName const& WallMaskedMaterialInstanceName,
                                                             FName const& CornerStaticMeshName,
                                                             FName const& CornerMaskedMaterialInstanceName,
                                                             FName const& BaseStaticMeshName,
                                                             FName const& BaseMaterialInstanceName,
                                                             FVector const& Location)
{
	if (StaticMeshBank && MaterialInstanceBank)
	{
		auto const WallStaticMeshRow = StaticMeshBank->FindRow<FBGStaticMeshBank>(WallStaticMeshName, "");
		auto const WallMaskedMaterialInstanceRow = MaterialInstanceBank->FindRow<FBGMaterialInstanceBank>(
			WallMaskedMaterialInstanceName, "");
		auto const CornerStaticMeshRow = StaticMeshBank->FindRow<FBGStaticMeshBank>(CornerStaticMeshName, "");
		auto const CornerMaskedMaterialInstanceRow = MaterialInstanceBank->FindRow<FBGMaterialInstanceBank>(
			CornerMaskedMaterialInstanceName, "");
		auto const BaseStaticMeshRow = StaticMeshBank->FindRow<FBGStaticMeshBank>(BaseStaticMeshName, "");
		auto const BaseMaterialRow = MaterialInstanceBank->FindRow<FBGMaterialInstanceBank>(
			BaseMaterialInstanceName, "");

		FTransform NewTransform{};
		NewTransform.SetLocation(Location);

		// Check to make sure we found all the data we need
		if (!WallStaticMeshRow || !WallMaskedMaterialInstanceRow
			|| !CornerStaticMeshRow || !CornerMaskedMaterialInstanceRow
			|| !BaseStaticMeshRow || !BaseMaterialRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bad Row Name"))
			return nullptr;
		}

		FBGStructureInfo const NewStructureInfo(WallStaticMeshRow->StaticMesh,
		                                        WallMaskedMaterialInstanceRow->MaterialInstance,
		                                        CornerStaticMeshRow->StaticMesh,
		                                        CornerMaskedMaterialInstanceRow->MaterialInstance,
		                                        BaseStaticMeshRow->StaticMesh,
		                                        BaseMaterialRow->MaterialInstance,
		                                        NewTransform);

		auto const NewSplineWall = CreateNewActor(EBGActorType::Structure);
		if (NewSplineWall)
		{
			auto SplineComponent = Cast<UBGSplineWallComponent>(NewSplineWall->
				GetComponentByClass(UBGSplineWallComponent::StaticClass()));
			if (SplineComponent)
			{
				SplineComponent->SetStructureInfo(NewStructureInfo);
				FTransform SpawnTransform;
				SpawnTransform.SetLocation(Location);
				NewSplineWall->FinishSpawning(SpawnTransform);

				FBGWallSplineSaveInfo NewWallSplineSaveInfo(UniqueActorID++, FName("Wall Spline"), Location,
				                                            WallStaticMeshName, WallMaskedMaterialInstanceName,
				                                            CornerStaticMeshName, CornerMaskedMaterialInstanceName,
				                                            BaseStaticMeshName, BaseMaterialInstanceName);

				SplineComponent->SetWallSplineSaveInfo(NewWallSplineSaveInfo);
				SplineComponent->Initialize();
				UE_LOG(LogTemp, Warning, TEXT("Spawning Structure At Location (server)"))
			}

			return NewSplineWall;
		}
	}

	return nullptr;
}

void ABGGameplayGameModeBase::ModifySplineStructureLength(UBGSplineWallComponent* InSplineComponent,
                                                          int const& PointIndex,
                                                          FVector const& NewLocation)
{
	if (InSplineComponent)
	{
		InSplineComponent->SetLocationOfSplinePoint(PointIndex, NewLocation);
		InSplineComponent->UpdateSplineStructureMesh(true);
	}
}

void ABGGameplayGameModeBase::AddSplinePointToSplineStructure(UBGSplineWallComponent* InSplineComponent,
                                                              FVector const& ClickLocation, int const& Index)
{
	if (InSplineComponent)
	{
		InSplineComponent->AddPointToSplineAtIndex(ClickLocation, Index);
	}
}

void ABGGameplayGameModeBase::AddPointToSpline(UBGSplineWallComponent* InSplineComponent, FVector const& ClickLocation,
                                               float const& Key)
{
	if (InSplineComponent)
	{
		InSplineComponent->AddPointToSpline(ClickLocation, Key);
	}
}

void ABGGameplayGameModeBase::ModifySplineStructureInstanceMeshAtIndex(UBGSplineWallComponent* InSplineComponent,
                                                                       int const& Index,
                                                                       FString const& NewInstanceName,
                                                                       UStaticMesh* StaticMesh,
                                                                       UMaterialInstance* MaterialInstance,
                                                                       FString const& OldInstanceName)
{
	if (InSplineComponent)
	{
		InSplineComponent->ModifyInstanceMeshAtIndex(Index, NewInstanceName, StaticMesh,
		                                             MaterialInstance, OldInstanceName);
	}
}

void ABGGameplayGameModeBase::SpawnStructureActorAtSplineStructureIndex(UBGSplineWallComponent* InSplineComponent,
                                                                        int const& Index,
                                                                        TSubclassOf<ABGStructure> StructureClassToSpawn,
                                                                        FString const& OldInstanceName)
{
	if (InSplineComponent && StructureClassToSpawn.Get())
	{
		auto const Instance = InSplineComponent->
			GetInstancedStaticMeshComponentByTag(OldInstanceName);

		if (Instance)
		{
			FTransform SavedTransform;
			Instance->GetInstanceTransform(Index, SavedTransform, true);
			SavedTransform.SetLocation(SavedTransform.GetLocation() + FVector(0.f, 0.f, 0.1f));

			InSplineComponent->
				SpawnStructureActorAtIndex(Index, StructureClassToSpawn, SavedTransform, OldInstanceName);
			InSplineComponent->RemoveInstanceMeshAtIndex(Index);
		}
	}
}

void ABGGameplayGameModeBase::DestroyStructureActor(ABGStructure* StructureToRemove)
{
	if (StructureToRemove)
	{
		if (!StructureToRemove->IsPendingKillPending())
		{
			StructureToRemove->GetParentSplineWallComponent()->RemoveStructureFromArray(StructureToRemove);
			StructureToRemove->Destroy();
		}
	}
}

void ABGGameplayGameModeBase::RestoreInstanceMeshOnSplineStructure(UBGSplineWallComponent* InSplineComponent,
                                                                   int const& Index,
                                                                   FTransform const& NewInstanceTransform,
                                                                   FString const& InstanceName)
{
	InSplineComponent->RestoreInstanceMeshAtIndex(Index, NewInstanceTransform, InstanceName);
}

void ABGGameplayGameModeBase::ResetSplineStructure(UBGSplineWallComponent* InSplineComponent)
{
	if (InSplineComponent)
	{
		InSplineComponent->ResetSplineWallComponent();
		
		// if (InSplineComponent->GetSplineComponent()->GetNumberOfSplinePoints() > 2)
		// {
		// 	// clear all spline points except for indices 0 and 1
		// 	for (int i{InSplineComponent->GetSplineComponent()->GetNumberOfSplinePoints()}; i > 2; --i)
		// 	{
		// 		InSplineComponent->GetSplineComponent()->RemoveSplinePoint(i - 1);
		// 	}
		// }
		//
		// auto SplineActorLocation = InSplineComponent->GetOwner()->GetActorLocation();
		//
		// // reset Spline Point 0 to actor's origin
		// InSplineComponent->GetSplineComponent()->SetLocationAtSplinePoint(
		// 	0, SplineActorLocation, ESplineCoordinateSpace::World, true);
		//
		// // reset Spline Point 1 to 105.f away the origin
		// SplineActorLocation.X += 50.f;
		//
		// InSplineComponent->GetSplineComponent()->SetLocationAtSplinePoint(
		// 	1, SplineActorLocation, ESplineCoordinateSpace::World, true);
		//
		// InSplineComponent->UpdateSplineStructureMesh(true);
	}
}

void ABGGameplayGameModeBase::SpawnNewBoard(int const& Zed, int const& X, int const& Y) const
{
	// TODO: Rebuild the function for spawning new boards

	// auto const BGGameplayGameState = GetGameState<ABGGameplayGameStateBase>();
	// if (BGGameplayGameState)
	// {
	// 	for (auto Board : BGGameplayGameState->GetAllBoards())
	// 	{
	// 		if (Board->GetZedLevel() == Zed && Board->GetBoardTiles().Num() == 0)
	// 		{
	// 			Board->BuildBoard(Board->GetActorLocation(), X, Y);
	// 		}
	// 	}
	// }
}

void ABGGameplayGameModeBase::ToggleTileVisibility(UBGTileComponent* TileToToggle)
{
	if (TileToToggle)
	{
		TileToToggle->ToggleTileVisibility(!TileToToggle->GetTileVisibility());
	}
}

void ABGGameplayGameModeBase::ShrinkBoard(UBGBoardComponent* BoardToShrink)
{
	if (BoardToShrink)
	{
		UE_LOG(LogTemp, Warning, TEXT("Shrink Board!"))
		BoardToShrink->ShrinkBoard(BoardToShrink->GetBoardSize().X - 1, BoardToShrink->GetBoardSize().Y - 1);
	}
}

void ABGGameplayGameModeBase::GrowBoard(UBGBoardComponent* BoardToGrow)
{
	if (BoardToGrow)
	{
		BoardToGrow->GrowBoard(BoardToGrow->GetBoardSize().X + 1, BoardToGrow->GetBoardSize().Y + 1);
	}
}

void ABGGameplayGameModeBase::SpawnNewActor(TSubclassOf<ABGActor> const ActorToSpawn) const
{
	if (ActorToSpawn.GetDefaultObject())
	{
		auto NewActor = Cast<ABGActor>(UGameplayStatics::BeginDeferredActorSpawnFromClass(
			this, ActorToSpawn, FTransform::Identity));

		NewActor->FinishSpawning(FTransform::Identity);
	}
}

void ABGGameplayGameModeBase::MoveActorToLocation(AActor* ActorToMove, FTransform const& NewTransform)
{
	if (ActorToMove)
	{
		ActorToMove->SetActorTransform(NewTransform);
	}
}
