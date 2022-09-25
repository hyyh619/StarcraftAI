#pragma once

#include <BWAPI.h>

typedef enum _MapDirection
{
    HY_TOP_LEFT = 0,
    HY_TOP,
    HY_TOP_RIGHT,
    HY_RIGHT,
    HY_BOTTOM_RIGHT,
    HY_BOTTOM,
    HY_BOTTOM_LEFT,
    HY_LEFT,
    HY_CENTER,
    HY_DIR_MAX
} MapDirection;

namespace Tools
{
BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset &units);
BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset &units);

int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset &units);

BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
BWAPI::Unit GetDepot();

int BuildBuilding(BWAPI::UnitType type);

void DrawUnitBoundingBoxes();
void DrawUnitCommands();

void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

int GetTotalSupply(bool inProgress = false);

void DrawUnitHealthBars();
void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);

MapDirection GetDirection(int X, int Y); // X, Y is in tile size. Each tile is 32x32.
}

extern std::map<std::string, BWAPI::UnitType> g_unitTypeMap;

