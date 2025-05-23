// © 2021 Matthew Barham. All Rights Reserved.

#include "Actors/Lights/BGSpotlight.h"

#include "Components/SpotLightComponent.h"

ABGSpotlight::ABGSpotlight()
{
	SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("Spot Light"));
	SpotLightComponent->SetupAttachment(RootComponent);
	SpotLightComponent->SetRelativeLocation(FVector(0.f, 0.f, 300.f));
	SpotLightComponent->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
}