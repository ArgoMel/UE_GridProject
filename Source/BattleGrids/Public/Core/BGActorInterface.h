// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UObject/Interface.h"
#include "BGActorInterface.generated.h"

class UBGContextMenu;
class UBGTileComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBGActorInterface : public UInterface
{
	GENERATED_BODY()
};

class BATTLEGRIDS_API IBGActorInterface
{
	GENERATED_BODY()
public:
	/** Context Menu Actions */
	virtual void ToggleContextMenu() = 0;
	virtual void CloseContextMenu() = 0;
	virtual UBGContextMenu* GetContextMenu() = 0;
};
