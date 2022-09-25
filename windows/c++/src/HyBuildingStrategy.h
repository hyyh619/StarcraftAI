#pragma once

#include <BWAPI.h>
#include "Json.h"
#include "Defines.h"

#define ZERG_9_POOL_SPEED 18 // add 3 zergling

typedef struct _BuildCondition
{
    int         waitForPriorBuild;
    int         waitBuildingNum;
    int         waitForMinerals;
    int         waitForGas;
    std::string waitBuilding;
} BuildCondition;

typedef struct _UnitTypeAndCondition
{
    BWAPI::UnitType type;
    int             count;
    BuildCondition  condition;
    std::string     buildType;
    std::string     comment;
    int             buildID;
    BuildLocMethod  locMethod;
    int             locationID;   // besides some building, the building is represented by buildID.
    int             locRange;

    _UnitTypeAndCondition()
    {
        type       = BWAPI::UnitTypes::Enum::Unknown;
        buildType  = std::string("Unknown");
        buildID    = -1;
        locationID = -1;
        count      = 0;
        locMethod  = HY_BUILD_LOC_STARTLOC;
        locRange   = 64;
    }
} UnitTypeAndCondition;

class BuildingStrategy
{
public:
    BuildingStrategy(UnitTypeAndCondition *pBuildings, int total);
    BuildingStrategy(const std::string strategyFile);
    ~BuildingStrategy();

    UnitTypeAndCondition* GetCurrentUnit();
    void MoveToNextUnit();
    bool IsTheLastOne();
    void ParseStrategyValue(Json::Value &value, UnitTypeAndCondition *p);
    bool ParseStrategyRoot(Json::Value &root);

    int m_checkFrame;                        // the first frame that will be checked if the condition of buildings is satisfied.

private:
    JsonParser           m_parser;
    UnitTypeAndCondition *m_pBuildingsSequence;
    int                  m_currentBuildings;
    int                  m_totalBuildings;
    std::string          m_comments[2];
};

extern std::map<std::string, BuildLocMethod> g_buildLocMethodMap;