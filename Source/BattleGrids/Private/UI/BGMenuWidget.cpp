// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGMenuWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"

void UBGMenuWidget::Setup()
{
	this->AddToViewport();

	auto const World = GetWorld();
	if (!ensure(World)) return;
	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	this->SetOwningPlayer(PlayerController);

	/** Set input mode to Game & UI, focus the new widget, and show mouse cursor */
	FInputModeGameAndUI InputModeData;
	InputModeData.SetWidgetToFocus(this->TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);

	/** Center the mouse cursor */
	PlayerController->SetMouseLocation(
		UWidgetLayoutLibrary::GetViewportSize(this).X / 2,
		UWidgetLayoutLibrary::GetViewportSize(this).Y / 2);
	
	PlayerController->bShowMouseCursor = true;
}

void UBGMenuWidget::Teardown()
{
	this->RemoveFromParent();

	auto const World = GetWorld();
	if (!ensure(World)) return;
	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	FInputModeGameAndUI const InputModeData;
	PlayerController->SetInputMode(InputModeData);
}

void UBGMenuWidget::SetMenuInterface(IBGMenuInterface* NewMenuInterface)
{
	MenuInterface = NewMenuInterface;
}