#pragma once

#include <BWAPI.h>

#define HY_ZERG          BWAPI::Races::Enum::Zerg
#define HY_ZERG_LARVA    BWAPI::UnitTypes::Enum::Zerg_Larva
#define HY_ZERG_EGG      BWAPI::UnitTypes::Enum::Zerg_Egg
#define HY_ZERG_DRONE    BWAPI::UnitTypes::Enum::Zerg_Drone
#define HY_ZERG_OVERLORD BWAPI::UnitTypes::Enum::Zerg_Overlord
#define HY_ZERG_ZERGLING BWAPI::UnitTypes::Enum::Zerg_Zergling

// Buildings
#define HY_ZERG_EXTRACTOR     BWAPI::UnitTypes::Enum::Zerg_Extractor
#define HY_ZERG_HATCHERY      BWAPI::UnitTypes::Enum::Zerg_Hatchery
#define HY_ZERG_CREEP_COLONY  BWAPI::UnitTypes::Enum::Zerg_Creep_Colony
#define HY_ZERG_SUNKEN_COLONY BWAPI::UnitTypes::Enum::Zerg_Sunken_Colony  // ground defender.
#define HY_ZERG_SPORE_COLONY  BWAPI::UnitTypes::Enum::Zerg_Spore_Colony   // air defender.

// Resources
#define HY_MINERAL_UNIT BWAPI::UnitTypes::Enum::Resource_Mineral_Field
#define HY_GAS_UNIT     BWAPI::UnitTypes::Enum::Resource_Vespene_Geyser

typedef enum _StrategyTime
{
    HY_STRATEGY_TIME_START  = 0,
    HY_STRATEGY_TIME_MIDDLE = 1,
    HY_STRATEGY_TIME_END    = 2,
    HY_STRATEGY_TIME_MAX    = 3
} StrategyTime;

typedef enum _BuildLocMethod
{
    HY_BUILD_LOC_RANDOM = 0,
    HY_BUILD_LOC_RANGE,
    HY_BUILD_LOC_NEARBY,
    HY_BUILD_LOC_STARTLOC,
    HY_BUILD_LOC_MAX
} BuildLocMethod;

