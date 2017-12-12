// Fill out your copyright notice in the Description page of Project Settings.

#include "OdenGate.h"
#include "HexRoom.h"

// Sets default values
FHexRoom::FHexRoom()
{
	roomId = 0;
	origin = FCoords();
	roomType = ERoomTypes::RT_SIMPLE;
	size = 1;
	bChallenge = false;
	maxDoors = 6;
	branch = 0;
}

FHexRoom::FHexRoom(ERoomTypes typeIn, uint8 id, FCoords orig) {
	roomId = id;
	origin = orig;
	bChallenge = false;
	branch = 0;
	ChangeType(typeIn);
}

void FHexRoom::ChangeType(ERoomTypes newType) {
	roomType = newType;
	switch (newType) {
	case ERoomTypes::RT_BOSS:
	case ERoomTypes::RT_SHOP:
		size = 1;
		maxDoors = 1;
		break;
	default:
		size = 1;
		maxDoors = 6;
		break;
	}
}

void FHexRoom::ChangeShape(uint8 shapeIndex) {
	roomShape = static_cast<ERoomShapes>(shapeIndex);
	const Shape shape = OG_ROOM_SHAPES[shapeIndex];
	for (FCoords c : shape.directions)
		shapeCoords.Add(c);
	size = shape.directions.Num();
	maxDoors = shape.doors;
}