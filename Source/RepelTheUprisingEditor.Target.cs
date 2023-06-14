// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class RepelTheUprisingEditorTarget : TargetRules
{
	public RepelTheUprisingEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("RepelTheUprising");
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
	}
}
