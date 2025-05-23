// © 2021 Matthew Barham. All Rights Reserved.


#include "Components/BGTileComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UBGTileComponent::UBGTileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bEditableWhenInherited = true;
	SetIsReplicatedByDefault(true);
}

bool UBGTileComponent::GetTileVisibility() const
{
	if (StaticMeshComponent)
	{
		return StaticMeshComponent->IsVisible();
	}
	return false;
}

void UBGTileComponent::ToggleTileVisibility_Implementation(bool const bIsVisible)
{
	StaticMeshComponent->SetVisibility(bIsVisible, true);
	StaticMeshComponent->SetCollisionEnabled(bIsVisible
		                                         ? ECollisionEnabled::Type::QueryAndPhysics
		                                         : ECollisionEnabled::Type::QueryOnly);
	StaticMeshComponent->SetCanEverAffectNavigation(bIsVisible ? true : false);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, bIsVisible ? ECR_Block : ECR_Ignore);
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Pawn, bIsVisible ? ECR_Block : ECR_Ignore);
}

// Called when spawned
void UBGTileComponent::BeginPlay()
{
	Super::BeginPlay();

	StaticMeshComponent = NewObject<UStaticMeshComponent>(GetOwner(), UStaticMeshComponent::StaticClass(),
	                                                      TEXT("Tile Static Mesh"));
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetupAttachment(GetOwner()->GetRootComponent());
		StaticMeshComponent->RegisterComponent();
		StaticMeshComponent->SetIsReplicated(true);
		StaticMeshComponent->CreationMethod = EComponentCreationMethod::Instance;
		StaticMeshComponent->SetCollisionProfileName("Tile");
		StaticMeshComponent->SetRelativeScale3D(FVector(1.f, 1.f, 1.f));

		if (TileStaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(TileStaticMesh);
		}

		ToggleTileVisibility(true);
	}
}

void UBGTileComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBGTileComponent, BoardActorReference)
	DOREPLIFETIME(UBGTileComponent, TileInfo)
	DOREPLIFETIME(UBGTileComponent, TileStaticMesh)
}