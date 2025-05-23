// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BGLight.h"

#include "BGSpotlight.generated.h"

class USpotLightComponent;

/**
 * BattleGrids movable Spotlight
 */
UCLASS()
class BATTLEGRIDS_API ABGSpotlight : public ABGLight
{
	GENERATED_BODY()
public:
	ABGSpotlight();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* SpotLightComponent;
};
