// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MonsterBattleSystem : ModuleRules
{
	public MonsterBattleSystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


        PublicDependencyModuleNames.AddRange(new string[] 
		{
			"Core", 
			"CoreUObject",
			"Engine", 
			"InputCore" ,
            "GameplayAbilities", 
			"GameplayTags", 
			"GameplayTasks", 
			"Paper2D",
        });


        PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Slate",
				"SlateCore",
                "AIModule", 
				"NavigationSystem",
				"UMG",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
