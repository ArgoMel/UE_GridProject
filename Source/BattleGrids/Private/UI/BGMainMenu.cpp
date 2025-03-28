// © 2021 Matthew Barham. All Rights Reserved.

#include "UI/BGMainMenu.h"
#include "Core/BGTypes.h"
#include "UI/BGServerRow.h"
#include "UI/BGMenuInterface.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/PanelWidget.h"
#include "UI/BGGameLoadRow.h"

UBGMainMenu::UBGMainMenu(FObjectInitializer const& ObjectInitializer)
{
	if (ServerRowWidgetClass->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("Found class %s"), *ServerRowWidgetClass->GetName());
	}
}

bool UBGMainMenu::Initialize()
{
	bool const bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	/** Initialize Main Menu widgets and bind Callbacks */
	if (!ensure(HostButton)) return false;
	HostButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenHostMenu);

	if (!ensure(JoinButton)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenJoinMenu);

	if (!ensure(OptionsButton)) return false;
	OptionsButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenOptionsMenu);

	if (!ensure(QuitButton)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UBGMainMenu::QuitPressed);

	/** Initialize Join Menu widgets and bind Callbacks */
	if (!ensure(CancelJoinMenuButton)) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenMainMenu);

	if (!ensure(RefreshListJoinMenuButton)) return false;
	RefreshListJoinMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::RefreshServerList);

	if (!ensure(ConfirmJoinMenuButton)) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::JoinServer);

	/** Initialize Host Menu widgets and bind Callbacks */
	if (!ensure(HostNewGameButton)) return false;
	HostNewGameButton->OnClicked.AddDynamic(this, &UBGMainMenu::HostServer);

	if (!ensure(CancelHostMenuButton)) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenMainMenu);

	if (!ensure(LoadGameButton)) return false;
	LoadGameButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenLoadMenu);

	/** Initialize Load Menu widgets and bind Callbacks */
	if (!ensure(ConfirmLoadMenuButton)) return false;
	ConfirmLoadMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::LoadGame);

	if (!ensure(CancelLoadMenuButton)) return false;
	CancelLoadMenuButton->OnClicked.AddDynamic(this, &UBGMainMenu::OpenHostMenu);

	MenuSwitcher->SetActiveWidget(MainMenu);

	return true;
}

void UBGMainMenu::SetActiveWidget(int const& Index)
{
	MenuSwitcher->SetActiveWidgetIndex(Index);
}

void UBGMainMenu::SetServerList(TArray<FBGServerData> ServerNames)
{
	auto const World = GetWorld();
	if (!ensure(World)) return;

	ServerList->ClearChildren();

	uint32 i = 0;
	for (FBGServerData const& ServerData : ServerNames)
	{
		UBGServerRow* ServerRow = CreateWidget<UBGServerRow>(World, ServerRowWidgetClass);
		if (!ensure(ServerRow)) return;

		ServerRow->SetServerData(ServerData);
		ServerRow->Setup(this, i);
		++i;

		ServerList->AddChild(ServerRow);
	}
}

void UBGMainMenu::SetSaveGameList(TArray<UBGGameSave*> GameSaves)
{
	auto const World = GetWorld();
	if (!ensure(World)) return;

	SaveGameList->ClearChildren();

	uint32 i = 0;
	for (UBGGameSave*& Save : GameSaves)
	{
		UBGGameLoadRow* GameLoadRow = CreateWidget<UBGGameLoadRow>(World, GameLoadRowWidgetClass);
		if (!ensure(GameLoadRow)) return;

		GameLoadRow->SetGameSave(Save);
		GameLoadRow->Setup(this, i);
		++i;

		SaveGameList->AddChild(GameLoadRow);
	}

	MenuInterface->ToggleThinkingPopup(false);
}

void UBGMainMenu::SelectServerRowIndex(uint32 const Index)
{
	SelectedServerRowIndex = Index;
	UpdateServerRowChildren();
}

void UBGMainMenu::SelectLoadGameRowIndex(uint32 const Index)
{
	SelectedLoadGameRowIndex = Index;
	UpdateLoadGameRowChildren();
}

void UBGMainMenu::UpdateServerRowChildren()
{
	for (int32 i{}; i < ServerList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UBGServerRow>(ServerList->GetChildAt(i));
		if (Row)
		{
			Row->SetRowIsSelected(SelectedServerRowIndex.IsSet() && SelectedServerRowIndex.GetValue() == i);
		}
	}
}

void UBGMainMenu::UpdateLoadGameRowChildren()
{
	for (int32 i{}; i < SaveGameList->GetChildrenCount(); ++i)
	{
		auto Row = Cast<UBGGameLoadRow>(SaveGameList->GetChildAt(i));
		if (Row)
		{
			Row->SetRowIsSelected(SelectedLoadGameRowIndex.IsSet() && SelectedLoadGameRowIndex.GetValue() == i);
		}
	}
}

void UBGMainMenu::HostServer()
{
	if (MenuInterface)
	{
		if (!ServerNameTextBox->GetText().IsEmpty())
		{
			MenuSwitcher->SetActiveWidget(MainMenu);
			MenuInterface->Host(ServerNameTextBox->GetText().ToString());
			ServerNameTextBox->SetText(FText());
		}
		else
		{
			MenuInterface->Host("Default");
		}
	}
}

void UBGMainMenu::JoinServer()
{
	if (SelectedServerRowIndex.IsSet() && MenuInterface)
	{
		if (auto const ServerRow = Cast<UBGServerRow>(ServerList->GetChildAt(SelectedServerRowIndex.GetValue())))
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Index %d"), SelectedServerRowIndex.GetValue())
			MenuSwitcher->SetActiveWidget(MainMenu);
			MenuInterface->Join(SelectedServerRowIndex.GetValue(), ServerRow->GetServerData());
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Selected Index Not Set"))
	}
}

void UBGMainMenu::LoadGame()
{
	if (SelectedLoadGameRowIndex.IsSet() && MenuInterface)
	{
		if (auto const GameLoadRow = Cast<
			UBGGameLoadRow>(SaveGameList->GetChildAt(SelectedLoadGameRowIndex.GetValue())))
		{
			UE_LOG(LogTemp, Warning, TEXT("Selected Index %d"), SelectedLoadGameRowIndex.GetValue())
			MenuSwitcher->SetActiveWidget(MainMenu);
			MenuInterface->LoadGameInfo(SelectedLoadGameRowIndex.GetValue(), GameLoadRow->GetGameSave());
		}
	}
}

void UBGMainMenu::OpenJoinMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(JoinMenu)) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	RefreshServerList();
}

void UBGMainMenu::RefreshServerList()
{
	if (MenuInterface)
	{
		ServerList->ClearChildren();
		MenuInterface->RefreshServerList();
	}
}

void UBGMainMenu::RefreshLoadGameList()
{
	if (MenuInterface)
	{
		SaveGameList->ClearChildren();
		MenuInterface->RefreshLoadGameList();
	}
}

void UBGMainMenu::OpenMainMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(MainMenu)) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UBGMainMenu::OpenHostMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(HostMenu)) return;
	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UBGMainMenu::OpenLoadMenu()
{
	if (!ensure(MenuSwitcher)) return;
	if (!ensure(LoadMenu)) return;
	MenuSwitcher->SetActiveWidget(LoadMenu);

	RefreshLoadGameList();
}

void UBGMainMenu::OpenOptionsMenu()
{
	if (MenuInterface)
	{
		MenuInterface->OpenOptionsMenu();
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UBGMainMenu::QuitPressed()
{
	auto const World = GetWorld();
	if (!ensure(World)) return;

	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController)) return;

	PlayerController->ConsoleCommand("quit");
}
