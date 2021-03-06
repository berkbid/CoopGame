// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class CoopGame : ModuleRules
{
	public CoopGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "UMG", "OnlineSubsystem"});

		PublicDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		//PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils", "OnlineSubsystemSteam" });

		DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
		DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
