#pragma once

#include <vector>
#include <queue>
#include "Tools.h"
#include "MapTools.h"
#include "Json.h"
#include "HyBuildingStrategy.h"
#include "HyUnitStrategy.h"

#include <BWAPI.h>

#define HY_FRAME_INTERVAL             5 // We do building and unit control per 5 frames.
#define HY_STORE_FRAME_STATE_INTERVAL 50 // Store frame state in every 50 frames.
#define HY_TOTAL_STORED_FRAME_STATE   1000

class HyBot;

typedef enum _BuildPriority
{
    HY_BUILD_PRI_LOWEST  = 0,
    HY_BUILD_PRI_LOWER   = 1,
    HY_BUILD_PRI_LOW     = 2,
    HY_BUILD_PRI_MEDIUM  = 3,
    HY_BUILD_PRI_HIGH    = 4,
    HY_BUILD_PRI_HIGHER  = 5,
    HY_BUILD_PRI_HIGHEST = 6,
    HY_BUILD_PRI_MAX     = 7
} BuildPriority;

class BuildingObj
{
public:
    BuildingObj(BWAPI::UnitType unitType, BuildPriority prior);
    ~BuildingObj();

public:
    BuildPriority m_buildPrior; // build priority

    int            m_Minerals; // Minerals for building this building.
    int            m_Gas; // Gas for building this building.
    int            m_buildTime; // in frames
    int            m_buildID;
    int            m_nearByBuildID;
    BuildLocMethod m_locMethod;
    int            m_locRange;

    BWAPI::UnitType m_buildType;
};

typedef std::vector<BuildingObj*> BuildObjArray;
typedef std::queue<BuildingObj*>  BuildBuildingQueue;

class BuildingManager
{
public:
    BuildingManager(HyBot *pBot);
    ~BuildingManager();

    void SetPlayer(BWAPI::Player player);
    void CreateBuildObj(BWAPI::UnitType unitType, BuildPriority prior, UnitTypeAndCondition *pUnitForCreate = nullptr);
    void DeleteBuildObj(BuildingObj *pBuilding);
    void PrioritiseBuildingQueue();
    void StartBuilding();
    BWAPI::TilePosition GetBuildPos(BWAPI::TilePosition pos, BWAPI::UnitType type, int maxBuildRange, bool buildingOnCreep, bool nearByPos);
    int GetNumOfTypesInBuildQueue(BWAPI::UnitType unitType);

private:
    void AddBuildingToBuild(BuildingObj *pBuilding);
    int DistributeBuildAction(BuildingObj *pBuilding);
    Unit* GetBuilderUnit(BWAPI::UnitType type, BWAPI::TilePosition pos);

public:
    BuildObjArray      m_BuiltBuildings; // all built buildings from game start.
    BuildObjArray      m_AliveBuildings; // current alived buildings.
    BuildObjArray      m_NeedToBuild[HY_BUILD_PRI_MAX];   // buildings will be built.
    BuildBuildingQueue m_BuildingQueue; // buildings' queue in priority.

private:
    BWAPI::Player m_player;
    HyBot         *m_pBot;
};

class FrameState
{
public:
    FrameState();
    ~FrameState();

    void ClearCurrentUnits()
    {
        memset(m_currentUnits, 0, sizeof(unsigned char) * BWAPI::UnitTypes::Enum::MAX);
    }

    void AddCurrentUnits(BWAPI::UnitType type)
    {
        if (type == HY_ZERG_EXTRACTOR)
            printf("");

        m_currentUnits[type]++;
    }

    void DecCurrentUnits(BWAPI::UnitType type)
    {
        m_currentUnits[type]--;
    }

public:
    int           m_totalSupply;
    int           m_usedSupply;
    int           m_minerals;
    int           m_gas;
    unsigned char m_currentUnits[BWAPI::UnitTypes::Enum::MAX];
};

class HyBot
{
public:

    HyBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void BuildAdditionalSupply();
    void drawDebugInformation();
    void BuildMissedDefender();

    // functions that are triggered by various BWAPI events from main.cpp
    void onStart();
    void onFrame();
    void onEnd(bool isWinner);
    void onUnitDestroy(BWAPI::Unit unit);
    void onUnitMorph(BWAPI::Unit unit);
    void onSendText(std::string text);
    void onUnitCreate(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onUnitShow(BWAPI::Unit unit);
    void onUnitHide(BWAPI::Unit unit);
    void onUnitRenegade(BWAPI::Unit unit);

    // For unit
    UnitGroup ConvertUnitTypeToGroup(BWAPI::UnitType type);
    Unit* FindUnit(int ID, UnitGroup group = HY_GROUP_ALL);
    void  AddUnitToGroup(Unit *pUnit, UnitGroup group);
    void  DeleteUnitFromGroup(int ID, UnitGroup group);
    void  MoveUnitGroup(UnitGroup from, UnitGroup to, Unit *pUnit);
    UnitMap* GetUnitGroup(UnitGroup group);
    Unit* ChooseUnit(BWAPI::UnitType type, BWAPI::TilePosition workerPos);
    Unit* ChooseUnit(BWAPI::UnitType type, BWAPI::Position workerPos);
    Unit* GetFirstGroundScouter();
    bool  IsUpgradeBuilding(BWAPI::UnitType type);

    // For resources
    BWAPI::Unit GetClosestAndNotFullUnit(Unit *pWorker, UnitMap *pResources);


private:
    // Map information
    void InitMap();

    // For building units
    void BuildStrategyStart();
    void BuildStrategyMiddle();
    void BuildStrategyEnd();
    void BuildFromStrategy(StrategyTime time);
    bool CheckBuildCondition(const UnitTypeAndCondition *pUnit);
    void GetAllUnits(); // Tranverse all units and save it in different groups.
    Unit* FindUnitInCreating(int ID);
    Unit* CreateUnit(BWAPI::Unit unit, bool bCreate);
    void  AddUnitToGroup(Unit *pUnit);
    void  AddUnitToGroup(BWAPI::Unit unit);

    // Units strategy for actions, such as scout, attack, defend and worker.
    void WorkerStrategy();
    void ScoutStrategy();

    // For each frame, we should execute the following actions
    //  1. Get current frame states.
    //  2. Choose what to build and distribute action to builder based on building strategy
    //  3.
    void GetCurrentFrameState(int nFrame);
    void ExecuteBuildStrategy();
    void ExecuteUnitAction();


public:
    MapTools            m_mapTools;
    BWAPI::Race         m_race;
    BWAPI::TilePosition m_startPos; // In tiles
    int                 m_mapWidth;
    int                 m_mapHeight;
    MapDirection        m_direction;
    FrameState          m_currentFrameState;
    ScoutMgr            m_scoutMgr;
    WorkMgr             m_workMgr;
    int                 m_totalGroundDefender;
    int                 m_totalAirDefender;

private:
    BuildingManager  m_buildMgr;
    BWAPI::Player    m_player;
    BuildingStrategy *m_pBuildStrategy[HY_STRATEGY_TIME_MAX];
    StrategyTime     m_StrategyTime;
    FrameState       m_frameStates[HY_TOTAL_STORED_FRAME_STATE];
    int              m_storedFrameStateIndex;
    UnitMap          m_units[HY_GROUP_MAX];    // All units in alive.
};

extern UnitTypeAndCondition g_Zerg_9PoolSpeed[ZERG_9_POOL_SPEED];

char* GroupName(UnitGroup group);