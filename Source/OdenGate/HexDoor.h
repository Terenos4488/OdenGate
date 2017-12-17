// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelSystem.h"
#include "HexDoor.generated.h"

USTRUCT(BlueprintType)
struct FHexDoor {
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "LevelGeneration")
		FCoords firstCoords;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "LevelGeneration")
		FCoords secondCoords;

	FHexDoor();
	FHexDoor(FCoords c1, FCoords c2);
	bool IsLinking(FCoords c1, FCoords c2);
	bool IsOn(FCoords c);
	bool operator==(const FHexDoor &other) const {
		return (other.firstCoords == firstCoords && other.secondCoords == secondCoords) ||
			(other.firstCoords == secondCoords && other.secondCoords == firstCoords);
	}
};