#pragma once

#include <BWAPI.h>
#include "Defines.h"

#define HY_SCOUT_STEP_MAX  9
#define HY_SCOUT_STRATEGY1 3

class HyBot;

typedef enum _UnitGroup
{
    HY_GROUP_ALL_IN_CREATING = 0,
    HY_GROUP_ALL_Buildings   = 1,
    HY_GROUP_ALL_Workers,
    HY_GROUP_ALL_AirUnits,
    HY_GROUP_ALL_GroudUnits,
    HY_GROUP_ALL_OtherUnits,
    HY_GROUP_ALL_Zerg_Larva,
    HY_GROUP_ALL_Zerg_Overlord,
    HY_GROUP_ALL_OnTheWay_Creating,
    HY_GROUP_ALL,
    HY_GROUP_MINERALS,
    HY_GROUP_GAS,
    HY_GROUP_MAX
} UnitGroup;

typedef enum _UnitCapability
{
    HY_CAP_WORKER      = 0,
    HY_CAP_BUILDING    = 1,
    HY_CAP_BURROW      = 2,
    HY_CAP_FLYING_UNIT = 3,
    HY_CAP_MAX,
} UnitCapability;

typedef enum _ScoutMethod
{
    HY_SCOUT_RANDOM = 0,
    HY_SCOUT_FIX_DIR_SAME_VERTICAL,
    HY_SCOUT_FIX_DIR_SAME_HORIZONTAL,
    HY_SCOUT_FIX_DIR_DIAGONAL,
    HY_SCOUT_FIX_PATH,
    HY_SCOUT_FIX_CENTER,
    HY_SCOUT_FIX_HOME,
    HY_SCOUT_CIRCLE,
    HY_SCOUT_METHOD_MAX
} ScoutMethod;

// If scouting in random/fix path/circle, we should choose the scout size.
typedef enum _ScoutSize
{
    HY_SCOUT_SMALL = 0,
    HY_SCOUT_MIDDLE,
    HY_SCOUT_BIG,
    HY_SCOUT_SIZE_MAX
} ScoutSize;

typedef struct _ScoutCondition
{
    BWAPI::UnitType waitUnitType;     // Wait some unit has enough count and send one to scout.
    int             waitCount;
    BWAPI::UnitType scoutUnitType;    // If there is no enough in scout, we should send one.
    int             scoutCount;
} ScoutCondition;

typedef struct _ScoutStep
{
    ScoutMethod method;
    ScoutSize   size;
} ScoutStep;

// For scouter, we can set several steps for scouting.
// ScoutingExecutor will execute it one by one.
typedef struct _ScoutStrategy
{
    BWAPI::UnitType type;
    ScoutCondition  condition;
    int             stepCount;
    ScoutStep       steps[HY_SCOUT_STEP_MAX];
} ScoutStrategy;

typedef struct _WorkStrategy
{
    float fRatioToMinerals;     // How ratio of workers to gather minerals.
    float fRatioToGas;          // How ratio of workers to gather gas.
    int   nPerMineralField;     // How many workers for each mineral field
    int   nPerGasField;         // How many workers for each gas field
    int   leastMineralWorkers;  // the least workers for gathering minerals.
    int   leastGasWorkers;      // the least workers for gathering gas.
} WorkStrategy;

typedef enum _UnitAction
{
    HY_ACTION_GATHER_MINERAL      = 0,
    HY_ACTION_GATHER_GAS          = 1,
    HY_ACTION_CREATING_BUILDING   = 2,
    HY_ACTION_CREATING_ON_THE_WAY = 3,
    HY_ACTION_SCOUTING            = 4,
    HY_ACTION_IDLE                = 5,
    HY_ACTION_WORK                = 6,
    HY_ACTION_CREATING_UNIT       = 7,
    HY_ACTION_UPGRADE_BUILDING    = 8,
    HY_ACTION_STUDY_TECH          = 9,
    HY_ACTION_MAX,
} UnitAction;

typedef struct _UnitActionStrategy
{
    UnitAction action;
    int        strategyCount;
    void       *pStrategy;
} UnitActionStrategy;


#define HY_IS_WORKER(pUnit)        pUnit->m_caps[HY_CAP_WORKER]
#define HY_IS_BUILDING(pUnit)      pUnit->m_caps[HY_CAP_BUILDING]
#define HY_IS_FLYING_UNIT(pUnit)   pUnit->m_caps[HY_CAP_FLYING_UNIT]
#define HY_IS_ZERG_OVERLORD(pUnit) (pUnit->m_type == BWAPI::UnitTypes::Enum::Zerg_Overlord ? true : false)
#define HY_IS_ZERG_LARVA(pUnit)    (pUnit->m_type == BWAPI::UnitTypes::Enum::Zerg_Larva ? true : false)

#define HY_ADD_UNIT_TO_GROUP(group, pUnit)                      \
    do                                                          \
    {                                                           \
        m_units[group][pUnit->m_ID] = pUnit;                    \
        pUnit->m_pGroups[group]     = &m_units[group];          \
        TR_LOG(TR_INFO, TR_BUILDING, "%s: added to group %s\n", \
               pUnit->m_name.c_str(), GroupName(group));        \
    }                                                           \
    while (0)

#define HY_SEARCH_GROUP_BY_ID(ID, group)            \
case group:                                         \
    do                                              \
    {                                               \
        UnitMapIter iter = m_units[group].find(ID); \
        if (iter != m_units[group].end())           \
        {                                           \
            pUnit = m_units[group][ID];             \
            return pUnit;                           \
        }                                           \
    }                                               \
    while (0);                                      \
    break;

class Unit
{
public:
    Unit(BWAPI::Unit unit, HyBot *pBot);
    ~Unit() {}

    void Initialize(bool bMorphed);
    void RemoveFromGroups();
    void DoAction(UnitActionStrategy *pActionStrategy, int nStrategyIndex);
    void ExecuteAction();
    bool IsResourceGathering();
    void ExeScouting(ScoutStrategy *pStrategy);
    void GatherMineral();
    void GatherGas();
    void CreateBuilding();
    void CreateUnit();
    void UpgradeUnit();
    void DistributeCreateAction(BWAPI::UnitType buildType);
    void DistributeUpgradeAction(BWAPI::UnitType buildType);
    void DistributeCreateAction(BWAPI::UnitType buildType, BWAPI::TilePosition buildPos, int buildID);

private:
    void PrintErr();
    void ExecuteDefaultAction();
    void ExecuteDefaultScout();
    void ExecuteDefaultWork();
    MapDirection GetNextDir(MapDirection dir, ScoutMethod method);
    BWAPI::Position GetTheNextPos(MapDirection dir, ScoutSize size);

public:
    HyBot                *m_pBot;
    BWAPI::Unit          m_unit;
    BWAPI::UnitType      m_type;
    BWAPI::UnitType      m_buildType;  // If worker or Zerg_Larva, it means what is built.
    BWAPI::TilePosition  m_buildPos;
    int                  m_buildID;
    BuildLocMethod       m_locMethod;
    int                  m_locRange;
    std::string          m_name;
    std::string          m_typeName;
    int                  m_gas;
    int                  m_minerals;
    int                  m_ID;
    int                  m_index; // index in array.
    bool                 m_caps[HY_CAP_MAX];
    std::map<int, Unit*> *m_pGroups[HY_GROUP_MAX]; // all the groups that unit is in.
    bool                 m_bMorphed;
    bool                 m_bAlive;
    bool                 m_bVisible;
    UnitActionStrategy   *m_pCurrentAction;
    int                  m_nStrategyIndex;
    bool                 m_bUsed;
    int                  m_nActionExecuted;
    int                  m_mapWidth;    // in tile
    int                  m_mapHeight;   // in tile
    BWAPI::TilePosition  m_pos;         // In tiles
    UnitAction           m_currentAction;
    BWAPI::Error         m_lastErr;
    int                  m_nSameErrCount;

    // For worker only
    int m_maxWorkersPerMineral;
    int m_maxWorkersPerGas;

    // For resources only
    int m_workersOn;  // How many workers are working on this resource.
};

typedef std::map<int, Unit*> UnitMap;
typedef std::map<int, Unit*>::iterator UnitMapIter;

class UnitMgr
{
public:
    UnitMgr(HyBot *pBot);
    ~UnitMgr();

    int GetAllUnitsOfType(BWAPI::UnitType type);
    Unit* FindUnit(int ID);
    void RemoveUnit(int ID);

public:
    UnitMap m_units;
    HyBot   *m_pBot;
};

class ScoutMgr : public UnitMgr
{
public:
    ScoutMgr(HyBot *pBot);
    ~ScoutMgr();

    void DoScout(StrategyTime time);
    void RemoveUnit(int ID);

private:
    void ZergScout(StrategyTime time, ScoutStrategy *pStrategy);

public:
    ScoutStrategy *m_pScoutStrategy[HY_STRATEGY_TIME_MAX];
    int           m_totalScouter[HY_STRATEGY_TIME_MAX];
    int           m_currentScouter[HY_STRATEGY_TIME_MAX];
    Unit          *m_pFirstGroundUnit = nullptr;
};

class WorkMgr : public UnitMgr
{
public:
    WorkMgr(HyBot *pBot);
    ~WorkMgr();

    void DoWork(StrategyTime time);
    void RemoveUnit(int ID);

private:
    void Clear();
    UnitAction DistributeWork(WorkStrategy *pStrategy);

public:
    int m_totalWorkers;
    int m_mineralWorkers;
    int m_gasWorkers;
    int m_count;
};