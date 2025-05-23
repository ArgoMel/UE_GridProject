// © 2021 Matthew Barham. All Rights Reserved.

#include "Components/BGSplineWallComponent.h"

#include "Actors/BGPawn.h"
#include "Actors/BGStructure.h"
#include "Components/BoxComponent.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Core/Gameplay/BGGameplayGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBGSplineWallComponent::UBGSplineWallComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bEditableWhenInherited = true;
	SetIsReplicatedByDefault(true);
}

void UBGSplineWallComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBGSplineWallComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBGSplineWallComponent, SplineComponent)
	DOREPLIFETIME(UBGSplineWallComponent, StructureData)
	DOREPLIFETIME(UBGSplineWallComponent, SpawnedStructures)
	DOREPLIFETIME(UBGSplineWallComponent, UniqueCollisionNumber)
	DOREPLIFETIME(UBGSplineWallComponent, UniqueInstanceNumber)
	DOREPLIFETIME(UBGSplineWallComponent, WallSplineSaveInfo)
}

void UBGSplineWallComponent::Initialize()
{
	SplineComponent = NewObject<USplineComponent>(
		GetOwner(),
		USplineComponent::StaticClass(),
		TEXT("Spline Component"));

	if (SplineComponent)
	{
		SplineComponent->SetupAttachment(GetOwner()->GetRootComponent());
		SplineComponent->RegisterComponent();
		SplineComponent->SetIsReplicated(true);
		SplineComponent->CreationMethod = EComponentCreationMethod::Instance;

		SetupSplineComponentVariables();
		UpdateSplineStructureMesh();
	}
}

void UBGSplineWallComponent::SetupSplineComponentVariables_Implementation()
{
	if (!SplineComponent || SplineComponent->GetNumberOfSplinePoints() == 0) return;

	for (int i{}; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
	}
}

void UBGSplineWallComponent::RemoveInstanceMeshAtIndex_Implementation(int const& Index, FString const& InstanceName)
{
	// UE_LOG(LogTemp, Warning, TEXT("Removing Instance at Index: %i"), Index);

	auto Instance = GetInstancedStaticMeshComponentByTag(InstanceName);

	if (Instance)
	{
		Instance->RemoveInstance(Index);
		CollisionBoxes[Index]->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
		CollisionBoxes[Index]->SetCollisionResponseToChannel(ECC_GRAB, ECR_Ignore);
		CollisionBoxes[Index]->SetCollisionResponseToChannel(ECC_RIGHT_CLICK, ECR_Ignore);
		CollisionBoxes[Index]->SetCanEverAffectNavigation(false);
		DisabledCollisionBoxes.Add(CollisionBoxes[Index]);
	}
}

void UBGSplineWallComponent::RestoreInstanceMeshAtIndex_Implementation(int const& Index, FTransform const& Transform,
                                                                       FString const& InstanceName)
{
	auto Instance = GetInstancedStaticMeshComponentByTag(InstanceName);

	if (Instance)
	{
		auto const NewTransform = Transform.GetRelativeTransform(GetOwner()->GetActorTransform());
		Instance->AddInstance(NewTransform);

		// Turn BoxCollision back on
		CollisionBoxes[Index]->SetCollisionProfileName("StructureBoxCollision");
		CollisionBoxes[Index]->SetCanEverAffectNavigation(true);
		DisabledCollisionBoxes.RemoveSingle(CollisionBoxes[Index]);
	}
}

void UBGSplineWallComponent::SpawnStructureActorAtIndex_Implementation(int const& Index,
                                                                       TSubclassOf<ABGStructure> StructureClassToSpawn,
                                                                       FTransform const& SpawnTransform,
                                                                       FString const& OldInstanceName)
{
	if (StructureClassToSpawn.Get())
	{
		// Prepare a Structure to spawn using the passed parameter
		ABGStructure* StructureToSpawn = GetWorld()->SpawnActorDeferred<ABGStructure>(
			StructureClassToSpawn->GetDefaultObject()->GetClass(), SpawnTransform);
		StructureToSpawn->SetParentSplineWallComponent(this);
		StructureToSpawn->SetIndexOnSplineStructure(Index);
		StructureToSpawn->SetActorType(EBGActorType::Door);

		// Spawn the new Structure and add it to the SplineStructure's array
		StructureToSpawn->FinishSpawning(SpawnTransform);
		SpawnedStructures.Add(StructureToSpawn);
		StructureToSpawn->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepWorldTransform);
	}
}

void UBGSplineWallComponent::ModifyInstanceMeshAtIndex_Implementation(int const& Index, FString const& NewInstanceName,
                                                                      UStaticMesh* StaticMesh,
                                                                      UMaterialInstance* MaterialInstance,
                                                                      FString const& OldInstanceName)
{
	if (StaticMesh && MaterialInstance)
	{
		FTransform SavedTransform;

		auto const OldInstance = GetInstancedStaticMeshComponentByTag(OldInstanceName);

		// Find the Old Instance (such as "WallInstance"), save its transform and then remove it
		if (OldInstance)
		{
			OldInstance->GetInstanceTransform(Index, SavedTransform, false);
			SavedTransform.SetLocation(SavedTransform.GetLocation() + FVector(0.f, 0.f, 0.1f));

			RemoveInstanceMeshAtIndex(Index);

			// CollisionBoxes[Index]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		// Check if we already have an InstancedStaticMeshComponent with the new name (such as "DoorInstance")
		// If we do, add to that one instead of making a new one
		for (auto InstanceComponent : GetOwner()->GetInstanceComponents())
		{
			if (InstanceComponent && InstanceComponent->GetName().Equals(NewInstanceName))
			{
				if (auto CastInstance = Cast<UInstancedStaticMeshComponent>(InstanceComponent))
				{
					CastInstance->AddInstance(SavedTransform);
					return;
				}
			}
		}

		// Otherwise...make a new InstancedStaticMeshComponent with the new InstanceName
		auto NewInstance = CreateInstancedStaticMeshComponent(NewInstanceName, StaticMesh,
		                                                      MaterialInstance);
		NewInstance->AddInstance(SavedTransform);
	}
}

void UBGSplineWallComponent::UpdateSplineStructureMesh_Implementation(bool const bCleanUpMeshes)
{
	if (!SplineComponent) return;

	if (bCleanUpMeshes)
	{
		CleanupInstanceComponents();
	}

	if (StructureData.WallStaticMesh && StructureData.WallMaskedMaterialInstance)
	{
		UpdateWallMesh();
	}

	if (StructureData.CornerStaticMesh && StructureData.CornerMaskedMaterialInstance)
	{
		UpdateCornerMesh();
	}

	/** Update the save info variable to normalize the point arrival and leave tangents before replication */
	UpdateWallSplineSaveInfo();
	GetOwner()->UpdateComponentTransforms();
}

void UBGSplineWallComponent::SetLocationOfSplinePoint_Implementation(int const& PointIndex, FVector const& NewLocation)
{
	SplineComponent->SetLocationAtSplinePoint(PointIndex, NewLocation, ESplineCoordinateSpace::World, true);

	/** Update the save info variable to normalize the point arrival and leave tangents before replication */
	UpdateWallSplineSaveInfo();
}

void UBGSplineWallComponent::AddPointToSpline_Implementation(FVector const& Location, float const& Key)
{
	FSplinePoint const NewSplinePoint(Key, Location, ESplinePointType::Linear);
	SplineComponent->AddPoint(NewSplinePoint);
	UpdateWallSplineSaveInfo();
}

void UBGSplineWallComponent::AddPointToSplineAtIndex_Implementation(FVector const& Location, int const& Index)
{
	SplineComponent->AddSplinePointAtIndex(Location, Index, ESplineCoordinateSpace::World, true);
	SplineComponent->SetSplinePointType(Index, ESplinePointType::Linear, true);
	UpdateWallSplineSaveInfo();
}

// void UBGSplineWallComponent::ToggleLockStructureInPlace_Implementation(bool const bNewLocked)
// {
// 	SetLocked(bNewLocked);
// }

void UBGSplineWallComponent::RemoveStructureFromArray(ABGStructure* StructureToRemove)
{
	SpawnedStructures.RemoveSingle(StructureToRemove);
}

void UBGSplineWallComponent::ResetSplineWallComponent_Implementation()
{
	if (GetSplineComponent()->GetNumberOfSplinePoints() > 2)
	{
		/** clear all spline points except for indices 0 and 1 */
		for (int i{GetSplineComponent()->GetNumberOfSplinePoints()}; i > 2; --i)
		{
			GetSplineComponent()->RemoveSplinePoint(i - 1);
		}
	}

	auto SplinePointZeroLocation = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	/** reset Spline Point 1 to 100.f away the Point 0 */
	SplinePointZeroLocation.X += 50.f;
	GetSplineComponent()->SetLocationAtSplinePoint(
		1, SplinePointZeroLocation, ESplineCoordinateSpace::World,
		true);

	UpdateSplineStructureMesh(true);

	for (auto Structure : SpawnedStructures)
	{
		if (Structure && !Structure->IsPendingKillPending())
		{
			RemoveStructureFromArray(Structure);
			Structure->Destroy();
		}
	}
}

UInstancedStaticMeshComponent* UBGSplineWallComponent::GetInstancedStaticMeshComponentByTag(FString const& Tag) const
{
	for (auto Instance : GetOwner()->GetInstanceComponents())
	{
		if (Instance && !Instance->IsBeingDestroyed() && Instance->ComponentHasTag(FName(Tag)))
		{
			if (auto const CastInstance = Cast<UInstancedStaticMeshComponent>(Instance))
			{
				return CastInstance;
			}
		}
	}
	return nullptr;
}

void UBGSplineWallComponent::SetStructureInfo(FBGStructureInfo const NewStructureInfo)
{
	StructureData = NewStructureInfo;
}

void UBGSplineWallComponent::SetWallSplineSaveInfo(FBGWallSplineSaveInfo const InWallSplineSaveInfo)
{
	FBGWallSplineSaveInfo NewSaveInfo = InWallSplineSaveInfo;

	for (auto& Point : NewSaveInfo.SplineCurves.Rotation.Points)
	{
		Point.ArriveTangent.Normalize();
		Point.LeaveTangent.Normalize();
	}

	WallSplineSaveInfo = NewSaveInfo;
}

void UBGSplineWallComponent::UpdateWallSplineSaveInfo()
{
	FSplineCurves NewSplineCurves = SplineComponent->SplineCurves;

	for (auto& Point : NewSplineCurves.Rotation.Points)
	{
		Point.ArriveTangent.Normalize();
		Point.LeaveTangent.Normalize();
	}

	WallSplineSaveInfo.SplineCurves = NewSplineCurves;
	WallSplineSaveInfo.Location = GetOwner()->GetActorLocation();
}

void UBGSplineWallComponent::SetSplineCurves(FSplineCurves const InSplineCurves)
{
	if (!SplineComponent) return;
	SplineComponent->SplineCurves = InSplineCurves;
}

void UBGSplineWallComponent::CleanupInstanceComponents()
{
	auto CurrentInstanceComponents = GetOwner()->GetInstanceComponents();

	// UE_LOG(LogTemp, Warning, TEXT("Removing all instances"))

	if (CurrentInstanceComponents.Num() > 0)
	{
		for (auto Instance : CurrentInstanceComponents)
		{
			if (Instance && !IsValid(Instance))
			{
				Instance->DestroyComponent();
			}
		}
		CurrentInstanceComponents.Empty();
	}

	if (CollisionBoxes.Num() > 0)
	{
		for (auto Box : CollisionBoxes)
		{
			if (Box && !IsValid(Box))
			{
				Box->DestroyComponent();
			}
		}
		CollisionBoxes.Empty();
	}

	if (DisabledCollisionBoxes.Num() > 0)
	{
		for (auto Box : DisabledCollisionBoxes)
		{
			if (Box && !IsValid(Box))
			{
				Box->DestroyComponent();
			}
		}
		DisabledCollisionBoxes.Empty();
	}

	if (SpawnedStructures.Num() > 0)
	{
		for (auto Structure : SpawnedStructures)
		{
			if (Structure && !Structure->IsPendingKillPending())
			{
				Structure->Destroy();
			}
		}
		SpawnedStructures.Empty();
	}
}

void UBGSplineWallComponent::CreateBoxCollision(UInstancedStaticMeshComponent* InstancedStaticMeshComponent,
                                                FTransform const& BaseTransform)
{
	/** Create a Box Collision for each instance */
	auto BoxCollision = NewObject<UBoxComponent>(this, UBoxComponent::StaticClass(),
	                                             FName(TEXT(
		                                             "Collision" + FString::FromInt(UniqueCollisionNumber++))),
	                                             RF_Transient);
	BoxCollision->RegisterComponent();
	BoxCollision->SetCollisionProfileName("StructureBoxCollision");
	BoxCollision->AttachToComponent(SplineComponent,
	                                FAttachmentTransformRules::SnapToTargetIncludingScale);
	BoxCollision->SetRelativeLocation(BaseTransform.GetLocation() + FVector(
		0.f, 0.f, InstancedStaticMeshComponent->GetStaticMesh()->
		                                        GetBounds().BoxExtent.Z));
	BoxCollision->SetRelativeRotation(BaseTransform.GetRotation());
	FVector BoxBounds = InstancedStaticMeshComponent->GetStaticMesh()->GetBounds().Origin
		+ InstancedStaticMeshComponent->GetStaticMesh()->GetBounds().BoxExtent
		+ FVector(0.5f, 0.5f, 0.05f);
	BoxBounds.Z /= 2;
	BoxCollision->SetBoxExtent(BoxBounds);
	CollisionBoxes.AddUnique(BoxCollision);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &UBGSplineWallComponent::OnOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &UBGSplineWallComponent::OnOverlapEnd);
}

void UBGSplineWallComponent::UpdateWallMesh()
{
	auto WallInstancedComponent = GetInstancedStaticMeshComponentByTag("WallInstance");

	if (!WallInstancedComponent->IsValidLowLevel() || IsValid(WallInstancedComponent))
	{
		// UE_LOG(LogTemp, Warning, TEXT("UpdateWallMesh(): Creating new CornerInstance"))

		WallInstancedComponent = CreateInstancedStaticMeshComponent(
			"WallInstance", StructureData.WallStaticMesh,
			StructureData.WallMaskedMaterialInstance);
		WallInstancedComponent->SetCollisionProfileName("Structure");
	}

	auto BaseInstancedComponent = GetInstancedStaticMeshComponentByTag("WallBaseInstance");

	if (!BaseInstancedComponent->IsValidLowLevel() || IsValid(WallInstancedComponent))
	{
		// UE_LOG(LogTemp, Warning, TEXT("UpdateWallMesh(): Creating new WallBaseInstance"))

		BaseInstancedComponent = CreateInstancedStaticMeshComponent(
			"WallBaseInstance", StructureData.BaseStaticMesh,
			StructureData.BaseMaterialInstance);
		BaseInstancedComponent->SetCollisionProfileName("Base");
	}

	int LoopCount{1};

	if (!SplineComponent) return;
	for (int i{}; i <= SplineComponent->GetNumberOfSplinePoints() - 2; ++i)
	{
		auto LocationOne = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto LocationTwo = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		auto const Distance = FVector::Dist(LocationOne, LocationTwo);
		// TODO: don't hardcode repeat-width (eventually remove and replace with globally set variable)
		auto const RepeatWidth{100.f};

		int const Integral = FMath::TruncToInt(Distance / 100.f) - 1;
		// UE_LOG(LogTemp, Warning, TEXT("Integral: %i"), Integral)

		/** Builds base meshes between Spline Points */
		for (int j{}; j <= Integral; ++j)
		{
			FTransform BaseTransform{};

			const auto SplineLocation = SplineComponent->GetLocationAtDistanceAlongSpline(
				LoopCount * RepeatWidth, ESplineCoordinateSpace::Local);
			const auto SplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(
				LoopCount * RepeatWidth, ESplineCoordinateSpace::Local);

			FVector BaseLocation{};
			BaseLocation.X = SplineLocation.X;
			BaseLocation.Y = SplineLocation.Y;
			BaseLocation.Z = SplineLocation.Z;

			FRotator BaseRotation{};
			BaseRotation.Pitch = SplineRotation.Pitch + StructureData.Transform.GetRotation().Rotator().Pitch;
			BaseRotation.Yaw = SplineRotation.Yaw + StructureData.Transform.GetRotation().Rotator().Yaw;
			BaseRotation.Roll = SplineRotation.Roll + StructureData.Transform.GetRotation().Rotator().Roll;

			BaseTransform.SetLocation(BaseLocation);
			BaseTransform.SetRotation(FQuat(BaseRotation));
			BaseTransform.SetScale3D(StructureData.Transform.GetScale3D());

			++LoopCount;
			BaseInstancedComponent->AddInstance(BaseTransform);

			if (BaseTransform.GetLocation() == LocationOne || BaseTransform.GetLocation() == LocationTwo)
			{
				// UE_LOG(LogTemp, Warning, TEXT("Corner Point, Skipping Wall Mesh"))
				continue;
			}

			FTransform StructureTransform{BaseTransform};

			int const RandomNumber = FMath::RandRange(0, 10);
			StructureTransform.SetRotation(
				FQuat(StructureTransform.Rotator().Add(0.f, RandomNumber < 4 ? 0.f : 180.f, 0.f)));
			StructureTransform.SetLocation(StructureTransform.GetLocation() + FVector(0.f, 0.f, 0.1f));

			WallInstancedComponent->AddInstance(StructureTransform);
			CreateBoxCollision(WallInstancedComponent, BaseTransform);
		}
	}
}

void UBGSplineWallComponent::UpdateCornerMesh()
{
	auto CornerInstancedComponent = GetInstancedStaticMeshComponentByTag("CornerInstance");

	if (!CornerInstancedComponent->IsValidLowLevel() || IsValid(CornerInstancedComponent))
	{
		// UE_LOG(LogTemp, Warning, TEXT("UpdateCornerMesh(): Creating new CornerInstance"))
		CornerInstancedComponent = CreateInstancedStaticMeshComponent(
			"CornerInstance", StructureData.CornerStaticMesh,
			StructureData.CornerMaskedMaterialInstance);
		CornerInstancedComponent->SetCollisionProfileName("Structure");
	}

	auto BaseInstancedComponent = GetInstancedStaticMeshComponentByTag("CornerBaseInstance");

	if (!BaseInstancedComponent->IsValidLowLevel() || IsValid(BaseInstancedComponent))
	{
		// UE_LOG(LogTemp, Warning, TEXT("UpdateCornerMesh(): Creating new CornerBaseInstance"))
		BaseInstancedComponent = CreateInstancedStaticMeshComponent(
			"CornerBaseInstance", StructureData.BaseStaticMesh,
			StructureData.BaseMaterialInstance);
		BaseInstancedComponent->SetCollisionProfileName("Base");
	}

	for (int i{}; i < SplineComponent->GetNumberOfSplinePoints(); ++i)
	{
		auto SplinePointTransform = SplineComponent->GetTransformAtSplinePoint(i, ESplineCoordinateSpace::Local, false);

		CornerInstancedComponent->AddInstance(SplinePointTransform);
		BaseInstancedComponent->AddInstance(SplinePointTransform);
		CreateBoxCollision(CornerInstancedComponent, SplinePointTransform);
	}
}

void UBGSplineWallComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            FHitResult const& SweepResult)
{
	/** Check if the overlapping actor is a Player (ABGPawn) */
	if (OtherActor->IsA(ABGPawn::StaticClass()))
	{
		if (OverlappedComponent && !DisabledCollisionBoxes.Contains(Cast<UBoxComponent>(OverlappedComponent)))
		{
			OverlappedComponent->SetCollisionResponseToChannel(ECC_GRAB, ECR_Ignore);
			OverlappedComponent->SetCollisionResponseToChannel(ECC_RIGHT_CLICK, ECR_Ignore);
		}
	}
}

void UBGSplineWallComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp,
                                          int32 OtherBodyIndex)
{
	/** Check if the overlapping actor is a Player (ABGPawn) */
	if (OtherActor->IsA(ABGPawn::StaticClass()))
	{
		if (OverlappedComponent && !DisabledCollisionBoxes.Contains(Cast<UBoxComponent>(OverlappedComponent)))
		{
			OverlappedComponent->SetCollisionProfileName("StructureBoxCollision");
		}
	}
}

void UBGSplineWallComponent::OnRep_SplineComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_SplineComponent()"))

	SplineComponent->SplineCurves = WallSplineSaveInfo.SplineCurves;

	SetupSplineComponentVariables();
	UpdateSplineStructureMesh();
}

UInstancedStaticMeshComponent* UBGSplineWallComponent::CreateInstancedStaticMeshComponent(FString const& InstanceTag,
	UStaticMesh* StaticMesh,
	UMaterialInstance* MaterialInstance)
{
	auto NewInstancedStaticMeshComponent = NewObject<UInstancedStaticMeshComponent>(
		this, *(InstanceTag + FString::FromInt(UniqueInstanceNumber++)));
	NewInstancedStaticMeshComponent->RegisterComponent();
	NewInstancedStaticMeshComponent->SetIsReplicated(true);
	NewInstancedStaticMeshComponent->AttachToComponent(SplineComponent,
	                                                   FAttachmentTransformRules::SnapToTargetIncludingScale);
	NewInstancedStaticMeshComponent->SetFlags(RF_Transactional);

	/** Use tag for keeping track of Instance type (i.e. Wall, Corner, Base, etc.) */
	NewInstancedStaticMeshComponent->ComponentTags.Add(FName(InstanceTag));

	if (StaticMesh)
	{
		NewInstancedStaticMeshComponent->SetStaticMesh(StaticMesh);
	}
	if (MaterialInstance)
	{
		NewInstancedStaticMeshComponent->SetMaterial(0, MaterialInstance);
	}

	GetOwner()->AddInstanceComponent(NewInstancedStaticMeshComponent);

	return NewInstancedStaticMeshComponent;
}
