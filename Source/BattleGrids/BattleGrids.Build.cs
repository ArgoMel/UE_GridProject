// © 2021 Matthew Barham. All Rights Reserved.

using UnrealBuildTool;

public class BattleGrids : ModuleRules
{
	public BattleGrids(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core",
			"CoreUObject",
			"Engine", 
			"InputCore",
			"AIModule", 
			"UMG", 
			"OnlineSubsystem",
			"OnlineSubsystemSteam"
		});

		PrivateDependencyModuleNames.AddRange(new string[] 
		{
			"Slate", 
			"SlateCore" 
		});
	}
}