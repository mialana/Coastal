// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Coastal : ModuleRules
{
	public Coastal(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"PhysicsCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[]
		{
			"Coastal",
			"Coastal/Public",
			"Coastal/Variant_Platforming",
			"Coastal/Variant_Platforming/Animation",
			"Coastal/Variant_Combat",
			"Coastal/Variant_Combat/AI",
			"Coastal/Variant_Combat/Animation",
			"Coastal/Variant_Combat/Gameplay",
			"Coastal/Variant_Combat/Interfaces",
			"Coastal/Variant_Combat/UI",
			"Coastal/Variant_SideScrolling",
			"Coastal/Variant_SideScrolling/AI",
			"Coastal/Variant_SideScrolling/Gameplay",
			"Coastal/Variant_SideScrolling/Interfaces",
			"Coastal/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}