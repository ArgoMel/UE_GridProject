// © 2021 Matthew Barham. All Rights Reserved.

#pragma once

#include "UI/BGMenuWidget.h"

#include "CoreMinimal.h"
#include "BGInGameMenu.generated.h"

class UBGOptionsMenu;
class UButton;
class UComboBoxString;
class UWidgetSwitcher;

/**
 * BattleGrids InGame Menu
 */
UCLASS()
class BATTLEGRIDS_API UBGInGameMenu : public UBGMenuWidget
{
	GENERATED_BODY()

public:

	virtual bool Initialize() override;
	
private:

	/** InGame Menu Button Callbacks */	

	UFUNCTION()
	void ResumePressed();

	UFUNCTION()
	void SavePressed();

	UFUNCTION()
	void OptionsPressed();

	UFUNCTION()
	void LeaveGamePressed();

	UFUNCTION()
	void QuitToDesktopPressed();

	/** InGame Menu Buttons */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ResumeButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* SaveGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* OptionsMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* LeaveGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* QuitToDesktopButton;

};
