// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include <vector>
#include "LevelSystem.h"
#include "HexDoor.h"
#include "HexRoom.h"
#include "LevelGenerator.generated.h"

UCLASS()
class ODENGATE_API ALevelGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "LevelGeneration")
	void Clear();

	UFUNCTION(BlueprintCallable, Category = "LevelGeneration")
	void Generate(int32 roomUnits, int32 seed);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "LevelGeneration")
		TMap<FCoords, FHexRoom> grid;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay")
		TArray<FHexDoor> doors;

	TMap<FCoords, bool> scan;
	FRandomStream* rand;
	enum ELevelSizes { LITTLE = 1, MEDIUM = 11, TALL = 19 };

private:
	uint8 randomInt(uint8 s, uint8 e);
	bool isEmpty(FCoords c);
	void placeMandatoryRoom(FHexRoom room, uint8 distance, ELevelSizes levelSize);
	bool saveDoor(FCoords c1, bool checkScan = false);
	bool tryPlacingTallRoom(FHexRoom room, FCoords position, uint8 branch);
	bool areClose(FCoords c1, FCoords c2);
	bool areLinked(FHexRoom r1, FHexRoom r2);
	uint8 countDoors(FHexRoom r);
	bool doorHasRoom(FHexDoor d, FHexRoom r);
	bool doorBetween(FCoords c1, FCoords c2);
	void scanLinkedCoords(FCoords start);
	TArray<FCoords> getFreeCloseCoords(bool upOnly, uint8 distanceFromStart = 0);
	TArray<uint8> getRandomOrder(uint8 numbers);
};
