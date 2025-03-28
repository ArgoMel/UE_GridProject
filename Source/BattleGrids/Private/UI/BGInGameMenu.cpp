// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGInGameMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/BGMenuInterface.h"

bool UBGInGameMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	/** InGame Menu Buttons */
	if (!ensure(ResumeButton)) return false;
	ResumeButton->OnClicked.AddDynamic(this, &UBGInGameMenu::ResumePressed);
	
	if (!ensure(SaveGameButton)) return false;
	SaveGameButton->OnClicked.AddDynamic(this, &UBGInGameMenu::SavePressed);
	
	if (!ensure(OptionsMenuButton)) return false;
	OptionsMenuButton->OnClicked.AddDynamic(this, &UBGInGameMenu::OptionsPressed);
	
	if (!ensure(LeaveGameButton)) return false;
	LeaveGameButton->OnClicked.AddDynamic(this, &UBGInGameMenu::LeaveGamePressed);
	
	if (!ensure(QuitToDesktopButton)) return false;
	QuitToDesktopButton->OnClicked.AddDynamic(this, &UBGInGameMenu::QuitToDesktopPressed);
	
	return true;
}

void UBGInGameMenu::ResumePressed()
{
	Teardown();
}

void UBGInGameMenu::SavePressed()
{
	if (MenuInterface)
	{
		MenuInterface->SaveGameInfo();
	}
}

void UBGInGameMenu::OptionsPressed()
{
	if (MenuInterface)
	{
		MenuInterface->OpenOptionsMenu();
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBGInGameMenu::LeaveGamePressed()
{
	if (MenuInterface)
	{
		Teardown();
		MenuInterface->LoadMainMenu();
	}
}

void UBGInGameMenu::QuitToDesktopPressed()
{
	auto OwningPlayerController = GetOwningPlayer();
	if (OwningPlayerController)
	{
		OwningPlayerController->ConsoleCommand("quit");
	}
}