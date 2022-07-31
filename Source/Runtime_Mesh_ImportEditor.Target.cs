// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class Runtime_Mesh_ImportEditorTarget : TargetRules
{
	public Runtime_Mesh_ImportEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "Runtime_Mesh_Import" } );
	}
}
