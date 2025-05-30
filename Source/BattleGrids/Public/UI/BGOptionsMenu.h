// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/BGMenuWidget.h"
#include "BGOptionsMenu.generated.h"

class UButton;
class UComboBoxString;
class UEditableTextBox;
class UGameUserSettings;
class UWidgetSwitcher;

/**
 * BattleGrids Options Menu for Game Settings
 */
UCLASS()
class BATTLEGRIDS_API UBGOptionsMenu : public UBGMenuWidget
{
	GENERATED_BODY()

protected:
	UBGOptionsMenu(FObjectInitializer const& ObjectInitializer);

	virtual bool Initialize() override;

private:
	void SetupGameSettings();

	/** Callback Functions */

	UFUNCTION()
	void ConfirmOptions();

	UFUNCTION()
	void ConfirmGameSettings();

	UFUNCTION()
	void CloseOptionsMenu();

	UFUNCTION()
	void ChangeFullscreenMode(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void ChangeResolution(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void SwitchSettingsMenu();

	UFUNCTION()
	void ConfirmPlayerSettings();

	UFUNCTION()
	void SetPlayerSettingsButtonEnabledState(FText const& InText);

	/** Options Menu Buttons */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ConfirmOptionsMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* CloseOptionsMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* SwitchOptionsMenuButton;

	/** Widgets */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* GameOptions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* PlayerOptions;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidgetSwitcher* OptionsMenuSwitcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UComboBoxString* FullscreenModeDropdown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UComboBoxString* ResolutionDropdown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UEditableTextBox* PlayerNameEntry;

	/** Variables */

	UPROPERTY()
	UGameUserSettings* GameSettings{nullptr};
};
