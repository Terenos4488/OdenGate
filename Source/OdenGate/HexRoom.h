// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelSystem.h"
#include "HexRoom.generated.h"

UENUM(BlueprintType)
enum class ERoomTypes : uint8
{
	RT_SIMPLE	UMETA(DisplayName = "Simple"),
	RT_TALL		UMETA(DisplayName = "Tall"),
	RT_STARTING	UMETA(DisplayName = "Starting"),
	RT_SHOP		UMETA(DisplayName = "Shop"),
	RT_BOSS		UMETA(DisplayName = "Boss"),
	RT_NPC		UMETA(DisplayName = "Npc"),
	RT_TREASURE	UMETA(DisplayName = "Treasure")
};

UENUM(BlueprintType)
enum class ERoomShapes : uint8
{
	DLU		UMETA(DisplayName = "Double Line Up"),
	DLL		UMETA(DisplayName = "Double Line Left"),
	DLR		UMETA(DisplayName = "Double Line Right"),
	TL		UMETA(DisplayName = "Triangle Left"),
	TR		UMETA(DisplayName = "Triangle Right")
};

USTRUCT(BlueprintType)
struct FHexRoom {
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = Enum)
		ERoomTypes roomType;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay")
		uint8 size;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay")
		bool bChallenge;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay")
		FCoords origin;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay")
		ERoomShapes roomShape;

	uint8 roomId;
	uint8 maxDoors;
	uint8 branch;
	TArray<FCoords> shapeCoords;

	FHexRoom();
	FHexRoom(ERoomTypes type, uint8 id, FCoords orig = FCoords());
	void ChangeType(ERoomTypes newType);
	void ChangeShape(uint8 shapeIndex);

	bool operator==(const FHexRoom &other) const {
		return other.roomId == roomId;
	}

	bool operator!=(const FHexRoom &other) const {
		return other.roomId != roomId;
	}
};
