// Fill out your copyright notice in the Description page of Project Settings.

#include "OdenGate.h"
#include "LevelGenerator.h"
#include "HexRoom.h"
#include "LevelSystem.h"

// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void ALevelGenerator::Clear() {
	grid.Empty();
	doors.Empty();
}

void ALevelGenerator::Generate(int32 roomUnits, int32 seed) {
	Clear();
	uint8 roomId = 0;
	seed = 7 * seed + 13 * roomUnits;
	rand = new FRandomStream(seed);

	ELevelSizes sizes[3] = { ELevelSizes::LITTLE, ELevelSizes::MEDIUM, ELevelSizes::TALL };
	ELevelSizes size = ELevelSizes::LITTLE;
	uint8 sizeIndex = 0;
	for (uint8 i = 1; i < 3; i++)
		if (roomUnits >= sizes[i] && size < sizes[i]) {
			size = sizes[i];
			sizeIndex = i;
		}
	
	// Tall rooms generation
	TArray<FHexRoom> tallRooms;
	uint32 tallRoomUnits = floor((roomUnits - 5 - 2 * sizeIndex) * 0.7);
	uint32 threeRooms = randomInt(0, floor(tallRoomUnits / 3));
	uint32 twoRooms = floor((tallRoomUnits - 3 * threeRooms) / 2);
	for (uint32 i = 0; i < threeRooms; i++) {
		FHexRoom tr(ERoomTypes::RT_TALL, roomId);
		roomId++;
		tr.ChangeShape(Shape::getRandomShape(3, rand));
		tallRooms.Add(tr);
	}
	for (uint32 i = 0; i < twoRooms; i++) {
		FHexRoom tr(ERoomTypes::RT_TALL, roomId);
		roomId++;
		tr.ChangeShape(Shape::getRandomShape(2, rand));
		tallRooms.Add(tr);
	}
	
	// Tiny rooms generation : simple rooms, npc rooms
	TArray<FHexRoom> tinyRooms;
	uint8 treasureRooms = 1 + (size == TALL ? 1 : 0) + (size == MEDIUM ? randomInt(0, 1) : 0);
	uint8 npcRooms = randomInt(0, sizeIndex + 1);
	for (uint8 i = 0; i < npcRooms; i++) {
		tinyRooms.Add(FHexRoom(ERoomTypes::RT_NPC, roomId));
		roomId++;
	}
	int8 restingTinyRooms = roomUnits - 3 * threeRooms - 2 * twoRooms - 4 - treasureRooms;
	for (int8 i = tinyRooms.Num() - 1; i < restingTinyRooms; i++) {
		tinyRooms.Add(FHexRoom(ERoomTypes::RT_SIMPLE, roomId));
		roomId++;
	}

	// Number of branches
	uint8 branches = 1;
	if (size == MEDIUM)
		branches = 2 + randomInt(0, 1);
	if (size == TALL)
		branches = 3 + randomInt(0, 3);
	
	// ORDRE DE PLACEMENT
	// - Placer la salle de départ
	// - Placer une petite salle dans les différentes branches
	// PAR BRANCHE
	// - Répartir les grandes salles dans toutes les branches
	// - Répartir les petites salles restantes un peu partout au pif
	// - Les salles d'une même branche doivent avoir une porte en commun si
	// possible
	// - Transformer 15% des salles vides en salles défi
	// - Détecter toutes les salles sans accès et y ajouter une porte avec
	// salle adjacente
	// - Ajouter quelques portes au hasard entre des salles qui n'en ont pas
	// entre elles
	// - Vérifier si toute la map est bien reliée
	// - Placer la salle Shop
	// - Placer la salle Boss

	// DEBUT DU PLACEMENT
	
	// Placer la salle de départ
	grid.Add(FCoords(), FHexRoom(ERoomTypes::RT_STARTING, roomId));
	roomId++;
	
	// Placer une petite salle dans les différentes branches
	FCoords* branchesDirections = new FCoords[branches];
	FCoords currentDirection = D_UP;
	uint8 currentIndex = 0;
	TArray<FCoords> availableDirections;
	for (const FCoords dir : DIRECTIONS)
		availableDirections.Add(dir);
	
	for (uint8 i = 0; i < branches; i++) {
		uint8 index = randomInt(0, tinyRooms.Num() - 1);
		FHexRoom room = tinyRooms[index];
		room.branch = i;
		FCoords coords = currentDirection;
		room.origin = coords;

		grid.Add(coords, room);
		doors.Add(FHexDoor(FCoords(), coords));
		tinyRooms.RemoveAt(index);

		branchesDirections[i] = currentDirection;
		availableDirections.RemoveAt(currentIndex);
		if (availableDirections.Num() != 0) {
			currentIndex = randomInt(0, availableDirections.Num() - 1);
			currentDirection = availableDirections[currentIndex];
		}
	}
	
	// Répartir les grandes salles dans toutes les branches
	uint8 currentBranch = 0;
	
	while (tallRooms.Num() != 0) {
		TArray<FCoords> closeDirections = FCoords::getClose(branchesDirections[currentBranch]);
		uint8 roomIndex = randomInt(0, tallRooms.Num() - 1);
		FHexRoom room = tallRooms[roomIndex];

		bool success = false;
		uint8 distance = 1;

		while (!success) {
			TArray<FCoords> placeShifts;
			placeShifts.Reserve(room.size);

			for (FCoords dir : closeDirections)
				placeShifts.Add(dir);

			while (placeShifts.Num() != 0) {
				uint8 index = randomInt(0, placeShifts.Num() - 1);
				FCoords shift = placeShifts[index];
				placeShifts.RemoveAt(index);

				success = tryPlacingTallRoom(room, branchesDirections[currentBranch] + shift * distance, currentBranch);
				if (success)
					break;
			}
			distance++;
		}
		tallRooms.RemoveAt(roomIndex);
		currentBranch = currentBranch == branches - 1 ? 0 : currentBranch + 1;
	}
	
	// Répartir les petites salles restantes un peu partout au pif
	TArray<FCoords> freeCloseCoords = getFreeCloseCoords(size == LITTLE);
	while (tinyRooms.Num() != 0) {
		if (freeCloseCoords.Num() == 0)
			freeCloseCoords = getFreeCloseCoords(size == LITTLE);

		uint8 roomIndex = randomInt(0, tinyRooms.Num() - 1);
		uint32 positionIndex = randomInt(0, freeCloseCoords.Num() - 1);

		tinyRooms[roomIndex].origin = freeCloseCoords[positionIndex];
		grid.Add(freeCloseCoords[positionIndex], tinyRooms[roomIndex]);

		tinyRooms.RemoveAt(roomIndex);
		freeCloseCoords.RemoveAt(positionIndex);
	}
	
	// Les salles d'une même branche doivent avoir une porte en commun
	for (auto e1 : grid)
		for (auto e2 : grid) {
			FHexRoom r1 = e1.Value;
			FHexRoom r2 = e2.Value;
			FCoords c1 = e1.Key;
			FCoords c2 = e2.Key;
			if (r1 == r2)
				break;
			if (r1.branch != r2.branch)
				break;
			if (!areClose(c1, c2))
				break;
			if (areLinked(r1, r2))
				break;
			if (r1.maxDoors == countDoors(r1) || r2.maxDoors == countDoors(r2))
				break;
			doors.Add(FHexDoor(c1, c2));
		}
	
	// Transformer 15% des salles vides en salles défi
	uint8 currentChallengeRooms = 0;
	uint8 challengeRoomsTarget = round(roomUnits * 0.15);

	TArray<FHexRoom> emptyRooms;
	for (auto entry : grid)
		if (entry.Value.roomType == ERoomTypes::RT_SIMPLE || entry.Value.roomType == ERoomTypes::RT_TALL)
			if (!emptyRooms.Contains(entry.Value))
				emptyRooms.Add(entry.Value);

	while (currentChallengeRooms < challengeRoomsTarget) {
		uint8 roomIndex = randomInt(0, emptyRooms.Num() - 1);
		FHexRoom room = emptyRooms[roomIndex];
		room.bChallenge = true;
		currentChallengeRooms += room.size;
		emptyRooms.RemoveAt(roomIndex);
	}
	
	// Détecter toutes les salles sans accès et y ajouter une porte
	TArray<FCoords> occupiedCoords;
	TArray<FCoords> farOccupiedCoords;
	for (auto entry : grid) {
		occupiedCoords.Add(entry.Key);
		if (FMath::Abs(entry.Key.x) > 1 || FMath::Abs(entry.Key.y) > 1)
			farOccupiedCoords.Add(entry.Key);
		if (countDoors(entry.Value) == 0)
			saveDoor(entry.Key);
	}
	
	// Ajouter quelques portes au hasard entre des salles
	for (uint8 i = 0; i < floor(grid.Num() * 0.5); i++) {
		FCoords c1 = farOccupiedCoords[randomInt(0, farOccupiedCoords.Num() - 1)];
		FHexRoom r1 = grid[c1];
		if (r1.maxDoors == countDoors(r1))
			continue;
		for (const FCoords dir : DIRECTIONS) {
			FCoords c2 = c1 + dir;
			if (!isEmpty(c2)) {
				FHexRoom r2 = grid[c2];
				if (r2 != r1 && r2.maxDoors != countDoors(r2) && !areLinked(r1, r2)) {
					doors.Add(FHexDoor(c1, c2));
					break;
				}
			}
		}
	}
	
	// Vérifier si toute la map est bien reliée
	while (true) {
		scan.Empty();
		for (auto entry : grid)
			scan.Add(entry.Key, false);

		scanLinkedCoords(FCoords());

		TArray<FCoords> isolatedCoords;
		for (auto entry : scan)
			if (entry.Value == false)
				isolatedCoords.Add(entry.Key);

		if (isolatedCoords.Num() == 0)
			break;
		else
			for (FCoords c : isolatedCoords)
				if (saveDoor(c, true))
					break;
	}

	// Placer les salles Treasure
	for (uint8 i = 0; i < treasureRooms; i++) {
		placeMandatoryRoom(FHexRoom(ERoomTypes::RT_TREASURE, roomId), 1, size);
		roomId++;
	}

	// Placer la salle Shop
	placeMandatoryRoom(FHexRoom(ERoomTypes::RT_SHOP, roomId), 2, size);
	roomId++;

	// Placer la salle Boss
	placeMandatoryRoom(FHexRoom(ERoomTypes::RT_BOSS, roomId), 3, size);
}

uint8 ALevelGenerator::randomInt(uint8 s, uint8 e) {
	return rand->RandRange(s, e);
}

bool ALevelGenerator::isEmpty(FCoords c) {
	return !grid.Contains(c);
}

void ALevelGenerator::placeMandatoryRoom(FHexRoom room, uint8 distance, ELevelSizes levelSize) {
	TArray<FCoords> freeCloseCoords = getFreeCloseCoords(levelSize == LITTLE, distance);
	if (freeCloseCoords.Num() == 0)
		freeCloseCoords = getFreeCloseCoords(levelSize == LITTLE);
	FCoords coords = freeCloseCoords[randomInt(0, freeCloseCoords.Num() - 1)];
	room.origin = coords;
	grid.Add(coords, room);
	saveDoor(coords);
}

bool ALevelGenerator::saveDoor(FCoords c1, bool checkScan) {
	FHexRoom r1 = grid[c1];
	for (const FCoords dir : DIRECTIONS) {
		FCoords c2 = c1 + dir;
		if (!isEmpty(c2)) {
			FHexRoom r2 = grid[c2];
			if (r2 != r1 && r2.maxDoors != countDoors(r2) && (!checkScan || scan[c2] == true)) {
				doors.Add(FHexDoor(c1, c2));
				return true;
			}
		}
	}
	return false;
}

bool ALevelGenerator::tryPlacingTallRoom(FHexRoom room, FCoords position, uint8 branch) {
	// Vérifier si la position initiale est libre
	if (!isEmpty(position))
		return false;

	uint8 roomSize = room.shapeCoords.Num() + 1;
	TArray<FCoords> tallGenerationDir = room.shapeCoords;
	TArray<uint8> tallCoordsOrder = getRandomOrder(roomSize);

	// Vérifier si l'une des configurations est possible
	for (uint8 &startIndex : tallCoordsOrder) {
		bool isAllEmpty = true;
		FCoords current = position;
		for (uint8 i = startIndex; i < roomSize - 1; i++) {
			current += tallGenerationDir[i];
			if (!isEmpty(current))
				isAllEmpty = false;
		}
		current = position;
		for (int8 i = startIndex - 1; i >= 0; i--) {
			current -= tallGenerationDir[i];
			if (!isEmpty(current))
				isAllEmpty = false;
		}

		if (isAllEmpty) {
			current = position;
			room.branch = branch;
			if (startIndex == 0)
				room.origin = current;

			TArray<FCoords> placingCoords;
			placingCoords.Add(current);
			for (uint8 i = startIndex; i < roomSize - 1; i++) {
				current += tallGenerationDir[i];
				placingCoords.Add(current);
			}
			current = position;
			for (int8 i = startIndex - 1; i >= 0; i--) {
				current -= tallGenerationDir[i];
				placingCoords.Add(current);
				if (i == 0)
					room.origin = current;
			}

			for (FCoords c : placingCoords)
				grid.Add(c, room);

			return true;
		}
	}
	return false;
}

bool ALevelGenerator::areClose(FCoords c1, FCoords c2) {
	int32 dX = c1.x - c2.x;
	int32 dY = c1.y - c2.y;

	if (dX > 1 || dX < -1 || dY > 1 || dY < -1)
		return false;
	if (dX == -dY)
		return false;

	return true;
}

bool ALevelGenerator::areLinked(FHexRoom r1, FHexRoom r2) {
	for (FHexDoor door : doors)
		if (doorHasRoom(door, r1) && doorHasRoom(door, r2))
			return true;
	return false;
}

uint8 ALevelGenerator::countDoors(FHexRoom r) {
	uint8 i = 0;
	for (FHexDoor door : doors)
		if (doorHasRoom(door, r))
			i++;
	return i;
}

bool ALevelGenerator::doorHasRoom(FHexDoor d, FHexRoom r) {
	return grid[d.firstCoords] == r || grid[d.secondCoords] == r;
}

bool ALevelGenerator::doorBetween(FCoords c1, FCoords c2) {
	for (FHexDoor door : doors)
		if (door.IsLinking(c1, c2))
			return true;
	return false;
}

void ALevelGenerator::scanLinkedCoords(FCoords start) {
	scan.Add(start, true);
	FCoords c;

	for (const FCoords dir : DIRECTIONS) {
		c = start + dir;
		if (scan.Contains(c) && scan[c] == false)
			if (grid[c] == grid[start] || doorBetween(start, c))
				scanLinkedCoords(c);
	}
}

TArray<FCoords> ALevelGenerator::getFreeCloseCoords(bool upOnly, uint8 distanceFromStart) {
	bool isFarAway;
	bool isUpForUpOnly;
	bool contains;

	TArray<FCoords> freeCloseCoords;
	TArray<FCoords> coords;
	coords.Reserve(grid.Num());

	for (auto entry : grid)
		coords.Add(entry.Key);

	for (FCoords c : coords)
		if (grid[c].roomType != ERoomTypes::RT_SHOP && grid[c].roomType != ERoomTypes::RT_TREASURE)
			for (const FCoords dir : DIRECTIONS) {
				FCoords nc = c + dir;
				isFarAway = nc.x * nc.y >= 0 ? ((nc.x > nc.y ? FMath::Abs(nc.x) : FMath::Abs(nc.y)) >= distanceFromStart)
					: ((FMath::Abs(nc.x) + FMath::Abs(nc.y)) >= distanceFromStart);
				isUpForUpOnly = !upOnly || (nc.x > 0 && nc.y > 0);
				contains = freeCloseCoords.Contains(nc);

				if (isUpForUpOnly && isFarAway && isEmpty(nc) && !contains)
					freeCloseCoords.Add(nc);
			}

	return freeCloseCoords;
}

TArray<uint8> ALevelGenerator::getRandomOrder(uint8 numbers) {
	TArray<uint8> order;
	order.Reserve(numbers);
	TArray<uint8> values;
	values.Reserve(numbers);

	for (uint8 i = 0; i < numbers; i++)
		values.Add(i);
	uint8 index;
	for (uint8 i = 0; i < numbers; i++) {
		index = randomInt(0, values.Num() - 1);
		order.Add(values[index]);
		values.RemoveAt(index);
	}
	return order;
}
