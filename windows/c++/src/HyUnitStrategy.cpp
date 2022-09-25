#include "HyDebugLog.h"
#include "Defines.h"
#include "HyBot.h"
#include "HyUnitStrategy.h"
#include "Tools.h"
#include "MapTools.h"


ScoutStrategy g_StartScout[HY_SCOUT_STRATEGY1] =
{
    {
        HY_ZERG_OVERLORD,
        {BWAPI::UnitTypes::Enum::Unknown, 0, BWAPI::UnitTypes::Enum::Unknown, 0},
        2,
        {
            {HY_SCOUT_FIX_DIR_DIAGONAL, HY_SCOUT_BIG},
            {HY_SCOUT_RANDOM, HY_SCOUT_BIG},
        }
    },
    {
        HY_ZERG_DRONE,
        {BWAPI::UnitTypes::Enum::Zerg_Drone, 8, BWAPI::UnitTypes::Enum::Unknown, 0},
        2,
        {
            {HY_SCOUT_FIX_DIR_SAME_HORIZONTAL, HY_SCOUT_BIG},
            {HY_SCOUT_RANDOM, HY_SCOUT_BIG},
        }
    },
    {
        HY_ZERG_OVERLORD,
        {BWAPI::UnitTypes::Enum::Zerg_Overlord, 2, BWAPI::UnitTypes::Enum::Unknown, 0},
        2,
        {
            {HY_SCOUT_FIX_DIR_SAME_VERTICAL, HY_SCOUT_BIG},
            {HY_SCOUT_RANDOM, HY_SCOUT_BIG},
        }
    }
};

WorkStrategy g_workStrategy1 =
{
    0.8f,
    0.2f,
    2,          // 2 workers for each mineral
    3,          // 3 workers for each gas
    8,          // we have 8 workers to gather minerals at least.
    3           // we have 3 workers to gather gas at least.
};

UnitActionStrategy g_scoutStartStrategy =
{
    HY_ACTION_SCOUTING,
    HY_SCOUT_STRATEGY1,
    (void*)&g_StartScout
};

UnitActionStrategy g_workStrategy =
{
    HY_ACTION_WORK,
    1,
    (void*)&g_workStrategy1,
};

void HyBot::ScoutStrategy()
{
    m_scoutMgr.DoScout(m_StrategyTime);
}

void HyBot::WorkerStrategy()
{
    m_workMgr.DoWork(m_StrategyTime);
}

//
// UnitMgr common class for each managers, such as scout, worker and attack
//
UnitMgr::UnitMgr(HyBot *pBot)
{
    m_pBot = pBot;
}

UnitMgr::~UnitMgr()
{
    for (UnitMapIter iter = m_units.begin(); iter != m_units.end();)
    {
        m_units.erase(iter++);
    }
}

int UnitMgr::GetAllUnitsOfType(BWAPI::UnitType type)
{
    int nCount = 0;

    for (UnitMapIter iter = m_units.begin(); iter != m_units.end(); iter++)
    {
        if (iter->second->m_type == type)
        {
            nCount++;
        }
    }

    return nCount;
}

Unit* UnitMgr::FindUnit(int ID)
{
    for (UnitMapIter iter = m_units.begin(); iter != m_units.end(); iter++)
    {
        if (iter->second->m_ID == ID)
        {
            return iter->second;
        }
    }

    return nullptr;
}

void UnitMgr::RemoveUnit(int ID)
{
    m_units.erase(ID);
}

//
// Worker manager
//
WorkMgr::WorkMgr(HyBot *pBot) :
    UnitMgr(pBot)
{
    m_count = 0;

    Clear();
}

WorkMgr::~WorkMgr()
{}

void WorkMgr::Clear()
{
    UnitMap empty;

    m_units.swap(empty);

    m_totalWorkers   = 0;
    m_mineralWorkers = 0;
    m_gasWorkers     = 0;
}

void WorkMgr::RemoveUnit(int ID)
{
    Unit *pUnit = FindUnit(ID);

    if (pUnit == nullptr)
        return;

    if (pUnit->m_currentAction == HY_ACTION_GATHER_MINERAL)
        m_mineralWorkers--;
    else if (pUnit->m_currentAction == HY_ACTION_GATHER_GAS)
        m_gasWorkers--;

    m_totalWorkers--;

    UnitMgr::RemoveUnit(ID);
}

void WorkMgr::DoWork(StrategyTime time)
{
    WorkStrategy *pStrategy = (WorkStrategy*)g_workStrategy.pStrategy;
    int          interval   = m_count % 5;

    m_count++;

    if (interval == 0)
    {
        // For each idle worker, assign some work.
        UnitMap *p = m_pBot->GetUnitGroup(HY_GROUP_ALL_Workers);

        for (UnitMapIter iter = p->begin(); iter != p->end(); iter++)
        {
            Unit *pUnit = iter->second;

            // Not used. add it to group of worker manager.
            if (pUnit->m_bAlive == true)
            {
                if (pUnit->m_bUsed == false)
                {
                    m_totalWorkers++;
                    m_units[pUnit->m_ID] = pUnit;
                }
            }
        }
    }
    else if (interval == 1)
    {
        // based on our strategy to distribute workers.
        for (UnitMapIter iter = m_units.begin(); iter != m_units.end(); iter++)
        {
            Unit *pUnit = iter->second;
            if (pUnit->m_bUsed == false)
            {
                UnitAction action = DistributeWork(pStrategy);
                pUnit->m_currentAction        = action;
                pUnit->m_maxWorkersPerMineral = pStrategy->nPerMineralField;
                pUnit->m_maxWorkersPerGas     = pStrategy->nPerGasField;
                pUnit->DoAction(&g_workStrategy, 0);

                {
                    if (pUnit->m_currentAction == HY_ACTION_GATHER_MINERAL)
                    {
                        TR_LOG(TR_INFO, TR_UNIT_ACTIONS, "%s: will gather mineral.\n", pUnit->m_name.c_str());
                        m_mineralWorkers++;
                    }
                    else if (pUnit->m_currentAction == HY_ACTION_GATHER_GAS)
                    {
                        TR_LOG(TR_INFO, TR_UNIT_ACTIONS, "%s: will gather gas.\n", pUnit->m_name.c_str());
                        m_gasWorkers++;
                    }
                }
            }
        }
    }
}

UnitAction WorkMgr::DistributeWork(WorkStrategy *pStrategy)
{
    if (m_mineralWorkers < pStrategy->leastMineralWorkers)
        return HY_ACTION_GATHER_MINERAL;

    if (m_gasWorkers < pStrategy->leastGasWorkers && m_pBot->m_currentFrameState.m_currentUnits[HY_ZERG_EXTRACTOR])
        return HY_ACTION_GATHER_GAS;

    // If there is only one extractor, we have to distribute workers to gather minerals.
    if (m_pBot->m_currentFrameState.m_currentUnits[HY_ZERG_EXTRACTOR] == 1)
    {
        return HY_ACTION_GATHER_MINERAL;
    }
    else
    {
        // Base on ratio between gas workers and mineral workers.
        float minRatio = (float)m_mineralWorkers / (float)m_totalWorkers;

        if (minRatio < pStrategy->fRatioToMinerals)
            return HY_ACTION_GATHER_MINERAL;
        else
            return HY_ACTION_GATHER_GAS;
    }
}

//
// Scout Manager
//
void ScoutMgr::ZergScout(StrategyTime time, ScoutStrategy *pStrategy)
{
    Unit           *pUnit      = nullptr;
    UnitGroup      group       = m_pBot->ConvertUnitTypeToGroup(pStrategy->type);
    UnitMap        *p          = m_pBot->GetUnitGroup(group);
    ScoutCondition *pCondition = &pStrategy->condition;

    // Get the current step.
    // Check current condition is satisfied.
    if (pCondition->waitUnitType != BWAPI::UnitTypes::Enum::Unknown)
    {
        if (pCondition->waitCount > m_pBot->m_currentFrameState.m_currentUnits[pCondition->waitUnitType])
        {
            return;
        }
    }

    if (pCondition->scoutUnitType != BWAPI::UnitTypes::Enum::Unknown)
    {
        if (pCondition->scoutCount >= GetAllUnitsOfType(pCondition->scoutUnitType))
        {
            return;
        }
    }

    // Pick one unit for scouting.
    UnitMapIter iter;

    for (iter = p->begin(); iter != p->end(); iter++)
    {
        if (iter->second->m_bUsed == false)
        {
            pUnit = iter->second;
            break;
        }
    }

    // There is no idle unit to scout.
    if (pUnit == nullptr)
    {
        // If we need a worker to scout
        if (group == HY_GROUP_ALL_Workers)
        {
#if 0
            for (iter = p->begin(); iter != p->end(); iter++)
            {
                if (iter->second->m_bUsed == true && iter->second->IsResourceGathering())
                {
                    pUnit = iter->second;
                    break;
                }
            }
#endif

            pUnit = m_pBot->ChooseUnit(HY_ZERG_DRONE, m_pBot->m_startPos);
        }

        if (pUnit == nullptr)
        {
            return;
        }
    }

    if (m_pFirstGroundUnit == nullptr && !pUnit->m_caps[HY_CAP_FLYING_UNIT])
    {
        m_pFirstGroundUnit = pUnit;
    }

    m_units[pUnit->m_ID] = pUnit;

    // After get idle unit
    // Send scouting strategy to unit.
    UnitActionStrategy *pAction = &g_scoutStartStrategy;

    pUnit->m_currentAction = HY_ACTION_SCOUTING;
    pUnit->DoAction(pAction, m_currentScouter[time]);

    // Move to next scout.
    m_currentScouter[time]++;
}

ScoutMgr::ScoutMgr(HyBot *pBot) :
    UnitMgr(pBot)
{
    for (int i = 0; i < HY_STRATEGY_TIME_MAX; i++)
    {
        m_pScoutStrategy[i] = nullptr;
        m_totalScouter[i]   = 0;
        m_currentScouter[i] = 0;
    }

    m_pScoutStrategy[HY_STRATEGY_TIME_START] = g_StartScout;
    m_totalScouter[HY_STRATEGY_TIME_START]   = sizeof(g_StartScout) / sizeof(ScoutStrategy);
}

ScoutMgr::~ScoutMgr()
{ }

void ScoutMgr::RemoveUnit(int ID)
{
    m_pFirstGroundUnit = nullptr;
    UnitMgr::RemoveUnit(ID);
}

void ScoutMgr::DoScout(StrategyTime time)
{
    int           currentScouter = m_currentScouter[time];
    ScoutStrategy *pStrategy     = &m_pScoutStrategy[time][currentScouter];

    // There is no scouter in current strategy.
    if (currentScouter >= m_totalScouter[time])
    {
        TR_LOG(TR_DEBUG, TR_SCOUTING, "There is no scouter in current strategy time(%d).\n", time);
        return;
    }

    if (m_pBot->m_race == HY_ZERG)
    {
        ZergScout(time, pStrategy);
    }
}

//
// Unit
//

Unit::Unit(BWAPI::Unit unit, HyBot *pBot)
{
    m_pBot            = pBot;
    m_bUsed           = false;
    m_unit            = unit;
    m_ID              = unit->getID();
    m_nActionExecuted = 0;
    m_pCurrentAction  = nullptr;
    m_nStrategyIndex  = 0;
    m_mapWidth        = BWAPI::Broodwar->mapWidth();
    m_mapHeight       = BWAPI::Broodwar->mapHeight();
    m_pos             = BWAPI::Broodwar->self()->getStartLocation();

    for (int i = 0; i < HY_GROUP_MAX; i++)
    {
        m_pGroups[i] = nullptr;
    }

    Initialize(false);
}

void Unit::RemoveFromGroups()
{
    for (int i = 0; i < HY_GROUP_ALL; i++)
    {
        if (m_pGroups[i])
        {
            UnitMapIter iter = m_pGroups[i]->find(m_ID);
            if (iter != m_pGroups[i]->end())
                m_pGroups[i]->erase(m_ID);
        }
    }

    m_pBot->m_scoutMgr.m_units.erase(m_ID);
}

void Unit::Initialize(bool bMorphed)
{
    m_bUsed         = false;
    m_workersOn     = 0;
    m_bAlive        = true;
    m_bVisible      = m_unit->isVisible();
    m_currentAction = HY_ACTION_IDLE;
    m_bMorphed      = bMorphed;
    m_ID            = m_unit->getID();
    m_type          = m_unit->getType();
    m_typeName      = m_type.getName();
    m_name          = m_typeName + "_" + std::to_string(m_ID);
    m_gas           = m_type.gasPrice();
    m_minerals      = m_type.mineralPrice();

    for (int i = 0; i < HY_CAP_MAX; i++)
        m_caps[i] = false;

    if (m_type.isWorker())
        m_caps[HY_CAP_WORKER] = true;

    if (m_type.isBuilding())
        m_caps[HY_CAP_BUILDING] = true;

    if (m_type.isBurrowable())
        m_caps[HY_CAP_BURROW] = true;

    if (m_type.isFlyer())
        m_caps[HY_CAP_FLYING_UNIT] = true;
}

bool Unit::IsResourceGathering()
{
    if (m_currentAction == HY_ACTION_GATHER_GAS || m_currentAction == HY_ACTION_GATHER_MINERAL)
        return true;
    else
        return false;
}

void Unit::ExecuteDefaultAction()
{
    switch (m_currentAction)
    {
    case HY_ACTION_SCOUTING:
        ExecuteDefaultScout();
        break;

    case HY_ACTION_GATHER_GAS:
    case HY_ACTION_GATHER_MINERAL:
        ExecuteDefaultWork();
        break;

    case HY_ACTION_IDLE:
        if (m_type.isWorker())
        {
            m_currentAction = HY_ACTION_GATHER_MINERAL;
        }

        break;

    case HY_ACTION_CREATING_UNIT:
        CreateUnit();
        break;

    case HY_ACTION_CREATING_BUILDING:
        CreateBuilding();
        break;

    case HY_ACTION_UPGRADE_BUILDING:
        UpgradeUnit();
        break;

    case HY_ACTION_STUDY_TECH:
        break;

    default:
        TR_LOG(TR_INFO, TR_UNIT_ACTIONS, "%s: There is no default action(%d).\n", m_name.c_str(), m_currentAction);
        break;
    }
}

void Unit::ExecuteDefaultScout()
{
    if (!m_unit->isIdle())
        return;

    BWAPI::Position     target;
    BWAPI::Position     pos = m_unit->getPosition();
    BWAPI::TilePosition tilePos(pos);
    MapDirection        dir = Tools::GetDirection(tilePos.x, tilePos.y);
    MapDirection        nextDir;

    nextDir = GetNextDir(dir, HY_SCOUT_RANDOM);
    target  = GetTheNextPos(nextDir, HY_SCOUT_BIG);
    m_unit->move(target, true);
    m_nActionExecuted++;
}

void Unit::ExecuteDefaultWork()
{
    if (!m_unit->isIdle())
        return;

    if (m_currentAction == HY_ACTION_GATHER_MINERAL)
        GatherMineral();
    else if (m_currentAction == HY_ACTION_GATHER_GAS)
        GatherGas();
}

void Unit::DoAction(UnitActionStrategy *pActionStrategy, int nStrategyIndex)
{
    m_pCurrentAction  = pActionStrategy;
    m_nStrategyIndex  = nStrategyIndex;
    m_nActionExecuted = 0;
    m_bUsed           = true;
}

void Unit::ExecuteAction()
{
    // Skip no action unit.
    if (m_type == HY_ZERG_EGG)
    {
        return;
    }

    if (m_pCurrentAction == nullptr)
    {
        ExecuteDefaultAction();
        return;
    }

    switch (m_currentAction)
    {
    case HY_ACTION_SCOUTING:
    {
        ScoutStrategy *pStrategy = (ScoutStrategy*)m_pCurrentAction->pStrategy;
        if (m_nActionExecuted >= pStrategy->stepCount)
            return;
    }
    break;

    case HY_ACTION_GATHER_GAS:
        if (m_pBot->m_currentFrameState.m_currentUnits[HY_ZERG_EXTRACTOR] == 0)
            return;

        break;

    case HY_ACTION_GATHER_MINERAL:
    case HY_ACTION_WORK:
    case HY_ACTION_CREATING_BUILDING:
    case HY_ACTION_CREATING_UNIT:
        break;

    case HY_ACTION_IDLE:
        break;

    default:
        TR_LOG(TR_ERROR, TR_UNIT_ACTIONS, "%s: There is no such action(%d) for checking.\n", m_name.c_str(), m_pCurrentAction->action);
        return;
    }

    switch (m_currentAction)
    {
    case HY_ACTION_SCOUTING:
    {
        ScoutStrategy *pStrategy = (ScoutStrategy*)m_pCurrentAction->pStrategy;
        ExeScouting(pStrategy);
    }
    break;

    case HY_ACTION_GATHER_MINERAL:
    case HY_ACTION_GATHER_GAS:
    case HY_ACTION_WORK:
    {
        if (m_currentAction == HY_ACTION_GATHER_MINERAL)
            GatherMineral();
        else if (m_currentAction == HY_ACTION_GATHER_GAS)
            GatherGas();
    }
    break;

    case HY_ACTION_CREATING_BUILDING:
        CreateBuilding();
        break;

    case HY_ACTION_CREATING_UNIT:
        break;

    default:
        TR_LOG(TR_ERROR, TR_UNIT_ACTIONS, "%s: There is no such action(%d).\n", m_name.c_str(), m_pCurrentAction->action);
        return;
    }
}

void Unit::DistributeCreateAction(BWAPI::UnitType buildType)
{
    m_buildType       = buildType;
    m_currentAction   = HY_ACTION_CREATING_UNIT;
    m_nActionExecuted = 0;
    m_bUsed           = true;
}

void Unit::DistributeCreateAction(BWAPI::UnitType buildType, BWAPI::TilePosition buildPos, int buildID)
{
    m_buildPos        = buildPos;
    m_buildType       = buildType;
    m_buildID         = buildID;
    m_currentAction   = HY_ACTION_CREATING_BUILDING;
    m_nActionExecuted = 0;
    m_bUsed           = true;
    m_pCurrentAction  = nullptr;
}

void Unit::DistributeUpgradeAction(BWAPI::UnitType buildType)
{
    m_buildType       = buildType;
    m_currentAction   = HY_ACTION_UPGRADE_BUILDING;
    m_nActionExecuted = 0;
    m_bUsed           = true;

    if (m_type == HY_ZERG_EXTRACTOR)
        printf("");
}

void Unit::PrintErr()
{
    BWAPI::Error err  = BWAPI::Broodwar->getLastError();
    const char   *str = err.toString().c_str();

    BWAPI::Broodwar->printf("%s Err: %s\n", m_name.c_str(), str);

    TR_LOG(TR_INFO, TR_BUILDING, "%s Err: %s\n", m_name.c_str(),  str);
}

void Unit::CreateBuilding()
{
    if (m_nActionExecuted > 0 && !m_unit->isIdle())
        return;

    int                 maxBuildRange   = 64;
    bool                bRes            = false;
    bool                buildingOnCreep = m_buildType.requiresCreep();
    BWAPI::TilePosition buildPos        = BWAPI::Broodwar->getBuildLocation(m_buildType, m_buildPos, maxBuildRange, buildingOnCreep);

    if (m_pBot->m_mapTools.isValidTile(m_buildPos))
    {
        bRes = m_unit->build(m_buildType, m_buildPos);
    }
    else
    {
        bRes = m_unit->build(m_buildType, buildPos);
    }

    if (!bRes)
    {
        PrintErr();
    }
    else
    {
        m_nActionExecuted++;
        TR_LOG(TR_INFO, TR_BUILDING, "%s: build %s.\n", m_name.c_str(), m_buildType.getName().c_str());
    }
}

void Unit::CreateUnit()
{
    bool bRes = false;

    if (m_nActionExecuted > 0)
        return;

    if (m_type == HY_ZERG_LARVA)
    {
        bRes = m_unit->build(m_buildType);
    }

    if (bRes == false)
    {
        PrintErr();
    }
    else
    {
        m_nActionExecuted++;
        TR_LOG(TR_INFO, TR_BUILDING, "%s: build %s.\n", m_name.c_str(), m_buildType.getName().c_str());
    }
}

void Unit::UpgradeUnit()
{
    bool bRes = false;

    if (m_nActionExecuted > 0)
        return;

    if (m_buildType == m_type)
        return;

    if (!m_unit->isIdle())
        return;

    bRes = m_unit->build(m_buildType);

    if (bRes == false)
    {
        PrintErr();
    }
    else
    {
        m_nActionExecuted++;
        m_currentAction = HY_ACTION_IDLE;

        TR_LOG(TR_INFO, TR_BUILDING, "%s: build %s.\n", m_name.c_str(), m_buildType.getName().c_str());

        if (m_type == HY_ZERG_CREEP_COLONY)
        {
            if (m_buildType == HY_ZERG_SUNKEN_COLONY)
                m_pBot->m_totalGroundDefender++;
            else if (m_buildType == HY_ZERG_SPORE_COLONY)
                m_pBot->m_totalAirDefender++;
        }
    }
}

void Unit::GatherMineral()
{
    bool bRes = false;

    if (m_nActionExecuted > 0 && !m_unit->isIdle())
        return;

    // Get the closest mineral to this worker unit
    BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(m_unit, BWAPI::Broodwar->getMinerals());

    // If a valid mineral was found, right click it with the unit in order to start harvesting
    if (closestMineral)
    {
        bRes = m_unit->rightClick(closestMineral);
        if (bRes)
        {
            m_nActionExecuted++;
            TR_LOG(TR_INFO, TR_UNIT_ACTIONS, "%s: gathers mineral.\n", m_name.c_str());
        }
    }
}

void Unit::GatherGas()
{
    bool bRes = false;

    if (m_nActionExecuted > 0 && !m_unit->isIdle())
        return;

    // Get the closest gas to this worker unit
    // BWAPI::Unit closestGas = Tools::GetClosestUnitTo(m_unit, BWAPI::Broodwar->getGeysers());
    Unit *gasExtractor = m_pBot->ChooseUnit(HY_ZERG_EXTRACTOR, m_pos);

    // If a valid mineral was found, right click it with the unit in order to start harvesting
    if (gasExtractor)
    {
        bRes = m_unit->rightClick(gasExtractor->m_unit);
        if (bRes)
        {
            m_nActionExecuted++;
            TR_LOG(TR_INFO, TR_UNIT_ACTIONS, "%s: gathers gas.\n", m_name.c_str());
        }
    }
}

void Unit::ExeScouting(ScoutStrategy *pStrategy)
{
    BWAPI::Position     target;
    BWAPI::Position     pos = m_unit->getPosition();
    BWAPI::TilePosition tilePos(pos);
    MapDirection        dir = Tools::GetDirection(tilePos.x, tilePos.y);
    MapDirection        nextDir;
    int                 stepIndex = m_nActionExecuted;
    ScoutStep           *pStep    = &pStrategy->steps[stepIndex];

    switch (pStep->method)
    {
    case HY_SCOUT_FIX_DIR_DIAGONAL:
    case HY_SCOUT_FIX_DIR_SAME_HORIZONTAL:
    case HY_SCOUT_FIX_DIR_SAME_VERTICAL:
    case HY_SCOUT_RANDOM:
        nextDir = GetNextDir(dir, pStep->method);
        target  = GetTheNextPos(nextDir, pStep->size);
        break;

    case HY_SCOUT_FIX_CENTER:
        nextDir = HY_CENTER;
        target  = GetTheNextPos(nextDir, pStep->size);
        break;

    case HY_SCOUT_FIX_HOME:
        target = BWAPI::Position(m_pBot->m_startPos);
        break;

    default:
        break;
    }

    m_unit->move(target, true);
    m_nActionExecuted++;
}

MapDirection Unit::GetNextDir(MapDirection dir, ScoutMethod method)
{
    MapDirection goDir = HY_TOP_LEFT;

    if (method == HY_SCOUT_FIX_DIR_DIAGONAL)
    {
        switch (dir)
        {
        case HY_TOP_LEFT:       goDir = HY_BOTTOM_RIGHT; break;

        case HY_TOP:            goDir = HY_BOTTOM_RIGHT; break;

        case HY_TOP_RIGHT:      goDir = HY_BOTTOM_LEFT; break;

        case HY_RIGHT:          goDir = HY_TOP_LEFT; break;

        case HY_BOTTOM_RIGHT:   goDir = HY_TOP_LEFT; break;

        case HY_BOTTOM:         goDir = HY_TOP_LEFT; break;

        case HY_BOTTOM_LEFT:    goDir = HY_TOP_RIGHT; break;

        case HY_LEFT:           goDir = HY_BOTTOM_RIGHT; break;

        case HY_CENTER:         goDir = HY_BOTTOM_RIGHT; break;

        default:
            TR_LOG(TR_ERROR, TR_UNIT_ACTIONS, "%s: There is no such dir(%d).\n", m_name.c_str(), dir);
            break;
        }
    }
    else if (method == HY_SCOUT_FIX_DIR_SAME_HORIZONTAL)
    {
        switch (dir)
        {
        case HY_TOP_LEFT:       goDir = HY_TOP_RIGHT; break;

        case HY_TOP:            goDir = g_nFrames % 2 ? HY_TOP_RIGHT : HY_TOP_LEFT; break;

        case HY_TOP_RIGHT:      goDir = HY_TOP_LEFT; break;

        case HY_RIGHT:          goDir = HY_LEFT; break;

        case HY_BOTTOM_RIGHT:   goDir = HY_BOTTOM_LEFT; break;

        case HY_BOTTOM:         goDir = g_nFrames % 2 ? HY_BOTTOM_RIGHT : HY_BOTTOM_LEFT; break;

        case HY_BOTTOM_LEFT:    goDir = HY_BOTTOM_RIGHT; break;

        case HY_LEFT:           goDir = HY_RIGHT; break;

        case HY_CENTER:         goDir = g_nFrames % 2 ? HY_LEFT : HY_RIGHT; break;

        default:
            TR_LOG(TR_ERROR, TR_UNIT_ACTIONS, "%s: There is no such dir(%d).\n", m_name.c_str(), dir);
            break;
        }
    }
    else if (method == HY_SCOUT_RANDOM)
    {
        goDir = (MapDirection)(g_nFrames % HY_DIR_MAX);
    }

    return goDir;
}


BWAPI::Position Unit::GetTheNextPos(MapDirection dir, ScoutSize size)
{
    BWAPI::Position moveTarget;

    switch (dir)
    {
    case HY_TOP_LEFT:
        moveTarget.x = 1;
        moveTarget.y = 1;
        break;

    case HY_BOTTOM_RIGHT:
        moveTarget.x = (m_mapWidth - 1) * 32 - 1;
        moveTarget.y = (m_mapHeight - 1) * 32 - 1;
        break;

    case HY_TOP_RIGHT:
        moveTarget.x = (m_mapWidth - 1) * 32 - 1;
        moveTarget.y = 1;
        break;

    case HY_BOTTOM_LEFT:
        moveTarget.x = 1;
        moveTarget.y = (m_mapHeight - 1) * 32 - 1;
        break;

    case HY_LEFT:
        moveTarget.x = 1;
        moveTarget.y = (m_mapHeight - 1) * 32 / 2 - 1;
        break;

    case HY_RIGHT:
        moveTarget.x = (m_mapWidth - 1) * 32 - 1;
        moveTarget.y = (m_mapHeight - 1) * 32 / 2 - 1;
        break;

    case HY_TOP:
        moveTarget.x = (m_mapWidth - 1) * 32 / 2 - 1;
        moveTarget.y = 1;
        break;

    case HY_BOTTOM:
        moveTarget.x = (m_mapWidth - 1) * 32 / 2 - 1;
        moveTarget.y = (m_mapHeight - 1) * 32 - 1;
        break;

    default:
        break;
    }

    switch (size)
    {
    case HY_SCOUT_SMALL:
        moveTarget.x = (int)((float)moveTarget.x / 3);
        moveTarget.y = (int)((float)moveTarget.y / 3);
        break;

    case HY_SCOUT_MIDDLE:
        moveTarget.x = (int)((float)moveTarget.x * 2 / 3);
        moveTarget.y = (int)((float)moveTarget.y * 2 / 3);
        break;

    case HY_SCOUT_BIG:
    default:
        break;
    }

    return moveTarget;
}