// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class OdenGateTarget : TargetRules
{
	public OdenGateTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		
		ExtraModuleNames.Add("OdenGate");
	}
}
