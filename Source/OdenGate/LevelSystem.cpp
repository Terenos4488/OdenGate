// Fill out your copyright notice in the Description page of Project Settings.

#include "OdenGate.h"
#include "LevelSystem.h"

FCoords FCoords::FromVector(TArray<FCoords>& coords) {
	FCoords result;
	for (FCoords coord : coords) {
		result += coord;
	}
	return result;
}

FCoords FCoords::FromDir(FCoords c, int32 value) {
	FCoords result;
	result.x = c.x;
	result.y = c.y;
	result *= value;
	return result;
}

const FCoords D_UP(1, 1);
const FCoords D_DOWN(-1, -1);
const FCoords LEFT_UP(1, 0);
const FCoords LEFT_DOWN(0, -1);
const FCoords RIGHT_UP(0, 1);
const FCoords RIGHT_DOWN(-1, 0);
const FCoords DIRECTIONS[6] = { D_UP, D_DOWN, LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN };

TArray<FCoords> FCoords::getClose(FCoords dir) {
	TArray<FCoords> result;
	if (dir == D_UP) {
		result.Add(LEFT_UP);
		result.Add(RIGHT_UP);
	}
	else if (dir == D_DOWN) {
		result.Add(RIGHT_DOWN);
		result.Add(LEFT_DOWN);
	}
	else if (dir == LEFT_UP) {
		result.Add(LEFT_DOWN);
		result.Add(D_UP);
	}
	else if (dir == LEFT_DOWN) {
		result.Add(D_DOWN);
		result.Add(LEFT_UP);
	}
	else if (dir == RIGHT_UP) {
		result.Add(D_UP);
		result.Add(RIGHT_DOWN);
	}
	else if (dir == RIGHT_DOWN) {
		result.Add(RIGHT_UP);
		result.Add(D_DOWN);
	}
	result.Add(dir);
	return result;
}

Shape::Shape(uint8 d, std::initializer_list<FCoords> dirList) {
	doors = d;
	for (FCoords dir : dirList)
		directions.Add(dir);
}

const Shape DOUBLE_LINE_UP(10, { D_UP });
const Shape DOUBLE_LINE_LEFT(10, { LEFT_UP });
const Shape DOUBLE_LINE_RIGHT(10, { RIGHT_UP });
const Shape TRIANGLE_LEFT(12, { D_UP, LEFT_DOWN });
const Shape TRIANGLE_RIGHT(12, { D_UP, RIGHT_DOWN });

const uint8 SHAPES = 5;
const Shape OG_ROOM_SHAPES[SHAPES] = { DOUBLE_LINE_UP,
DOUBLE_LINE_LEFT,
DOUBLE_LINE_RIGHT,
TRIANGLE_LEFT,
TRIANGLE_RIGHT };

uint8 Shape::getRandomShape(uint8 size, FRandomStream* rand) {
	int n = 0;
	for (const Shape &s : OG_ROOM_SHAPES)
		if (s.directions.Num() + 1 == size)
			n++;
	int rand_int = (*rand).RandRange(0, n - 1);
	n = 0;
	for (uint8 i = 0; i < SHAPES; i++)
		if (OG_ROOM_SHAPES[i].directions.Num() + 1 == size)
			if (n == rand_int)
				return i;
			else
				n++;
	return 0;
}
