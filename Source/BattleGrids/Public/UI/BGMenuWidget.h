// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BGMenuWidget.generated.h"

class IBGMenuInterface;

/**
 * Parent widget class for all Menus in BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API UBGMenuWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	IBGMenuInterface* MenuInterface;

public:
	void Setup();
	void Teardown();

	IBGMenuInterface* GetMenuInterface() const { return MenuInterface; }

	void SetMenuInterface(IBGMenuInterface* NewMenuInterface);
};
