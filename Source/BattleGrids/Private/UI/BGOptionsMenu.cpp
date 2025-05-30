// © 2021 Matthew Barham. All Rights Reserved.


#include "UI/BGOptionsMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameFramework/GameUserSettings.h"
#include "UI/BGMainMenu.h"
#include "UI/BGMenuInterface.h"

UBGOptionsMenu::UBGOptionsMenu(FObjectInitializer const& ObjectInitializer)
{
}

bool UBGOptionsMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	if (GEngine)
	{
		GameSettings = GEngine->GetGameUserSettings();
		SetupGameSettings();
	}

	if (!ensure(ConfirmOptionsMenuButton)) return false;
	ConfirmOptionsMenuButton->OnClicked.AddDynamic(this, &UBGOptionsMenu::ConfirmOptions);

	if (!ensure(CloseOptionsMenuButton)) return false;
	CloseOptionsMenuButton->OnClicked.AddDynamic(this, &UBGOptionsMenu::CloseOptionsMenu);

	if (!ensure(SwitchOptionsMenuButton)) return false;
	SwitchOptionsMenuButton->OnClicked.AddDynamic(this, &UBGOptionsMenu::SwitchSettingsMenu);

	return true;
}

void UBGOptionsMenu::SetupGameSettings()
{
	if (!GameSettings) return;

	if (!ensure(FullscreenModeDropdown)) return;
	FullscreenModeDropdown->AddOption(FString("Windowed"));
	FullscreenModeDropdown->AddOption(FString("WindowedFullscreen"));
	FullscreenModeDropdown->AddOption(FString("Fullscreen"));
	FullscreenModeDropdown->SetSelectedOption(UEnum::GetValueAsString(GameSettings->GetFullscreenMode()).RightChop(13));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(GameSettings->GetFullscreenMode()))
	FullscreenModeDropdown->OnSelectionChanged.AddDynamic(this, &UBGOptionsMenu::ChangeFullscreenMode);

	if (!ensure(ResolutionDropdown)) return;
	ResolutionDropdown->AddOption(FString("1280x720"));
	ResolutionDropdown->AddOption(FString("1920x1080"));
	ResolutionDropdown->AddOption(FString("2560x1440"));
	ResolutionDropdown->AddOption(FString("3840x2160"));
	ResolutionDropdown->SetSelectedOption(
		FString::Printf(TEXT("%ix%i"), GameSettings->GetScreenResolution().X, GameSettings->GetScreenResolution().Y));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GameSettings->GetScreenResolution().ToString())
	ResolutionDropdown->OnSelectionChanged.AddDynamic(this, &UBGOptionsMenu::ChangeResolution);
}

void UBGOptionsMenu::ConfirmOptions()
{
	if (OptionsMenuSwitcher->GetActiveWidget() == GameOptions)
	{
		ConfirmGameSettings();
	}
	else
	{
		ConfirmPlayerSettings();
	}
}

void UBGOptionsMenu::ConfirmGameSettings()
{
	if (!GameSettings) return;

	GameSettings->ApplySettings(false);
}

void UBGOptionsMenu::CloseOptionsMenu()
{
	if (MenuInterface)
	{
		MenuInterface->CloseOptionsMenu();
	}
}

void UBGOptionsMenu::ChangeFullscreenMode(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!GameSettings) return;

	if (SelectedItem.Equals("Windowed"))
	{
		GameSettings->SetFullscreenMode(EWindowMode::Windowed);
	}
	else if (SelectedItem.Equals("WindowedFullscreen"))
	{
		GameSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	}
	else if (SelectedItem.Equals("Fullscreen"))
	{
		GameSettings->SetFullscreenMode(EWindowMode::Fullscreen);
	}
}

void UBGOptionsMenu::ChangeResolution(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!GameSettings) return;

	if (SelectedItem.Equals("1280x720"))
	{
		GameSettings->SetScreenResolution(FIntPoint(1280, 720));
	}
	else if (SelectedItem.Equals("1920x1080"))
	{
		GameSettings->SetScreenResolution(FIntPoint(1920, 1080));
	}
	else if (SelectedItem.Equals("2560x1440"))
	{
		GameSettings->SetScreenResolution(FIntPoint(2560, 1440));
	}
	else if (SelectedItem.Equals("3840x2160"))
	{
		GameSettings->SetScreenResolution(FIntPoint(3840, 2160));
	}
}

void UBGOptionsMenu::SwitchSettingsMenu()
{
	if (OptionsMenuSwitcher->GetActiveWidget() == GameOptions)
	{
		OptionsMenuSwitcher->SetActiveWidget(PlayerOptions);
	}
	else if (OptionsMenuSwitcher->GetActiveWidget() == PlayerOptions)
	{
		OptionsMenuSwitcher->SetActiveWidget(GameOptions);
	}
}

void UBGOptionsMenu::ConfirmPlayerSettings()
{
	if (!PlayerNameEntry->GetText().IsEmpty())
	{
		FBGPlayerInfo NewPlayerInfo;
		NewPlayerInfo.PlayerName = PlayerNameEntry->GetText();

		MenuInterface->SavePlayerInfo(NewPlayerInfo);
		PlayerNameEntry->SetText(FText());
		SwitchSettingsMenu();
	}
}

void UBGOptionsMenu::SetPlayerSettingsButtonEnabledState(FText const& InText)
{
	if (OptionsMenuSwitcher->GetActiveWidget() == PlayerOptions && InText.IsEmpty())
	{
		ConfirmOptionsMenuButton->SetIsEnabled(false);
	}
	else
	{
		ConfirmOptionsMenuButton->SetIsEnabled(true);
	}
}
