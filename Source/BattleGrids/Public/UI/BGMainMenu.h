// © 2021 Matthew Barham. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"

#include "Components/ComboBoxString.h"
#include "Core/Save/BGGameSave.h"
#include "UI/BGMenuWidget.h"
#include "BGMainMenu.generated.h"

struct FBGServerData;

class UEditableTextBox;
class UButton;
class UComboBoxString;
class UWidgetSwitcher;
class UPanelWidget;
class UBGServerRow;

/**
 * Main Menu widget for BattleGrids
 */
UCLASS()
class BATTLEGRIDS_API UBGMainMenu : public UBGMenuWidget
{
	GENERATED_BODY()

public:

	void SetActiveWidget(int const& Index);

	void SetServerList(TArray<FBGServerData> ServerNames);
	void SetSaveGameList(TArray<UBGGameSave*> GameSaves);

	void SelectServerRowIndex(uint32 const Index);
	void SelectLoadGameRowIndex(uint32 const Index);

protected:
	UBGMainMenu(FObjectInitializer const& ObjectInitializer);

	virtual bool Initialize() override;

private:

	void UpdateServerRowChildren();
	void UpdateLoadGameRowChildren();

	/** Callback Functions */
	
	UFUNCTION()
	void HostServer();

	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void LoadGame();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void RefreshServerList();

	UFUNCTION()
	void RefreshLoadGameList();

	UFUNCTION()
	void OpenMainMenu();

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenLoadMenu();

	UFUNCTION()
	void OpenOptionsMenu();

	UFUNCTION()
	void QuitPressed();

	/** Main Menu Widgets */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidgetSwitcher* MenuSwitcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UEditableTextBox* ServerNameTextBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* MainMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* HostMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UPanelWidget* ServerList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UPanelWidget* SaveGameList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* JoinMenu;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UWidget* LoadMenu;

	/** Main Menu Buttons */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* HostButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* JoinButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* OptionsButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* QuitButton;

	/** Host Menu Buttons */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* HostNewGameButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* CancelHostMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* LoadGameButton;

	/** Join Menu Buttons */
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* CancelJoinMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* RefreshListJoinMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ConfirmJoinMenuButton;

	/** Load Menu Buttons */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* CancelLoadMenuButton;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	UButton* ConfirmLoadMenuButton;

	/** Class References */
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true),
		Category = "UBGMainMenu|Config")
	TSubclassOf<UUserWidget> ServerRowWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true),
	Category = "UBGMainMenu|Config")
	TSubclassOf<UUserWidget> GameLoadRowWidgetClass;

	/** unexposed variables */
	
	TOptional<uint32> SelectedServerRowIndex;
	TOptional<uint32> SelectedLoadGameRowIndex;

};
