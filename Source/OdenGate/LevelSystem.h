// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <LevelSystem.generated.h>

USTRUCT(BlueprintType)
struct FCoords {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelGeneration")
		int32 x;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LevelGeneration")
		int32 y;

	FCoords() {
		x = 0;
		y = 0;
	}

	FCoords(int32 xIn, int32 yIn) {
		x = xIn;
		y = yIn;
	}

	FCoords FromVector(TArray<FCoords>& coords);
	FCoords FromDir(FCoords c, int32 value);

	bool operator==(const FCoords &other) const {
		return other.x == x && other.y == y;
	}
	FCoords operator+(const FCoords &other) const {
		FCoords result;
		result.x = x + other.x;
		result.y = y + other.y;
		return result;
	}
	FCoords& operator+=(const FCoords &other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	FCoords operator-(const FCoords &other) const {
		FCoords result;
		result.x = x - other.x;
		result.y = y - other.y;
		return result;
	}
	FCoords& operator-=(const FCoords &other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	FCoords operator-() const {
		FCoords result;
		result.x = -x;
		result.y = -y;
		return result;
	}
	FCoords operator*(const int32 &f) const {
		FCoords result;
		result.x = x*f;
		result.y = y*f;
		return result;
	}
	FCoords operator*=(const int32 &f) const {
		return *this * f;
	}

	friend uint32 GetTypeHash(const FCoords& Other)
	{
		return Other.x * 7 + Other.y * 13;
	}

	static TArray<FCoords> getClose(FCoords dir);
};

const extern FCoords D_UP, D_DOWN, LEFT_UP, LEFT_DOWN, RIGHT_UP, RIGHT_DOWN;
const extern FCoords DIRECTIONS[6];

struct Shape {
	Shape(uint8 d, std::initializer_list<FCoords> dirList);

	uint8 doors;
	TArray<FCoords> directions;

	static uint8 getRandomShape(uint8 size, FRandomStream* rand);
};

const extern Shape  DOUBLE_LINE_UP,
DOUBLE_LINE_LEFT,
DOUBLE_LINE_RIGHT,
TRIANGLE_LEFT,
TRIANGLE_RIGHT,
TRIPLE_LINE_UP,
TRIPLE_LINE_LEFT,
TRIPLE_LINE_RIGHT;

const extern Shape OG_ROOM_SHAPES[8];
