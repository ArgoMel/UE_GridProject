// © Argo. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

MONSTERBATTLESYSTEM_API DECLARE_LOG_CATEGORY_EXTERN(LogMonsterBattleSystem, Log, All);

class FMonsterBattleSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
