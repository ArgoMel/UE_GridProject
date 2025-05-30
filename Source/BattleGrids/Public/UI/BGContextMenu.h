// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Core/BGTypes.h"
#include "UI/BGMenuWidget.h"
#include "BGContextMenu.generated.h"

/**
 * Parent class for Context Menus, inherits from BGMenuWidget
 */
UCLASS()
class BATTLEGRIDS_API UBGContextMenu : public UBGMenuWidget
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true),
	Category = "BGContextMenu|Config")
	FHitResult HitResult;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true, ExposeOnSpawn = true),
		Category = "BGContextMenu|Config")
	EBGClassCategory ClassCategory {EBGClassCategory::None};
	
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "BGContextMenu|Events")
	void Update();

	AActor* GetActorReference() const { return HitResult.GetActor(); }
	FHitResult GetHitResult() const { return HitResult; }

	void SetHitResult(FHitResult const& InHitResult);
};
