// Fill out your copyright notice in the Description page of Project Settings.

#include "OdenGate.h"
#include "HexDoor.h"


// Sets default values
FHexDoor::FHexDoor() {
}

FHexDoor::FHexDoor(FCoords c1, FCoords c2) {
	firstCoords = c1;
	secondCoords = c2;
}

bool FHexDoor::IsLinking(FCoords c1, FCoords c2) {
	return (firstCoords == c1 && secondCoords == c2) ||
		(firstCoords == c2 && secondCoords == c1);
}

bool FHexDoor::IsOn(FCoords c) {
	return firstCoords == c || secondCoords == c;
}
