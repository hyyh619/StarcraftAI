#include "HyDebugLog.h"
#include "Defines.h"
#include "HyBot.h"
#include "Tools.h"
#include "MapTools.h"


int g_nFrames = 0;

char* GroupName(UnitGroup group)
{
    static char *groupName[HY_GROUP_MAX] =
    {
        "IN_CREATING",
        "Buildings",
        "Workers",
        "AirUnits",
        "GroudUnits",
        "OtherUnits",
        "Zerg_Larva",
        "Zerg_Overlord",
        "OnTheWay_Creating",
        "All",
        "Minerals",
        "Gas"
    };

    return groupName[group];
}

HyBot::HyBot() :
    m_buildMgr(this),
    m_scoutMgr(this),
    m_workMgr(this),
    m_mapTools(this)
{
    for (int i = 0; i< HY_STRATEGY_TIME_MAX; i++)
        m_pBuildStrategy[i] = nullptr;


    m_totalGroundDefender = 8;
    m_totalAirDefender    = 2;
}

void HyBot::InitMap()
{
    // Get a location that we want to build the building next to
    m_startPos  = BWAPI::Broodwar->self()->getStartLocation();
    m_mapWidth  = BWAPI::Broodwar->mapWidth();
    m_mapHeight = BWAPI::Broodwar->mapHeight();
    m_direction = Tools::GetDirection(m_startPos.x, m_startPos.y);
}

// Called when the bot starts!
void HyBot::onStart()
{
    // Set our BWAPI options here
    BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(5);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Get player
    m_player = BWAPI::Broodwar->self();
    m_buildMgr.SetPlayer(m_player);

    // Create strategies for buildings
#if 0
    m_pBuildStrategy[HY_STRATEGY_TIME_START] = new BuildingStrategy(g_Zerg_9PoolSpeed, ZERG_9_POOL_SPEED);
#else
    m_pBuildStrategy[HY_STRATEGY_TIME_START] = new BuildingStrategy(std::string("BuildStrategy_Zerg_9_pool.json"));
#endif

    // Set strategy time
    m_StrategyTime = HY_STRATEGY_TIME_START;
    m_race         = m_player->getRace();

    // Set frame number
    g_nFrames               = 0;
    m_storedFrameStateIndex = 0;

    // Initialize map information.
    InitMap();

    // Get all units when starting.
    GetAllUnits();

    // Call MapTools OnStart
    m_mapTools.onStart();
}

// Called whenever the game ends and tells you if you won or not
void HyBot::onEnd(bool isWinner)
{
    for (int i = 0; i < HY_STRATEGY_TIME_MAX; i++)
    {
        if (m_pBuildStrategy[i] != nullptr)
            delete m_pBuildStrategy[i];
    }

    TR_LOG(TR_ERROR, TR_GAME, "We %s\n", isWinner ? "won!" : "lost!");
}

void HyBot::ExecuteBuildStrategy()
{
    switch (m_StrategyTime)
    {
    case HY_STRATEGY_TIME_START:
        BuildStrategyStart();
        break;

    case HY_STRATEGY_TIME_MIDDLE:
        BuildStrategyMiddle();
        break;

    case HY_STRATEGY_TIME_END:
        BuildStrategyEnd();
        break;

    default:
        BWAPI::Broodwar->printf("There is no strategy time!");
        break;
    }

    // Re-order the building queue
    m_buildMgr.PrioritiseBuildingQueue();

    // Build buildings based on building queue.
    m_buildMgr.StartBuilding();
}

void HyBot::ExecuteUnitAction()
{
    // Do action of every unit.
    for (int i = 0; i< HY_GROUP_ALL; i++)
    {
        if ((i == HY_GROUP_ALL_GroudUnits) ||
            (i == HY_GROUP_ALL_AirUnits) ||
            (i == HY_GROUP_ALL_Workers) ||
            (i == HY_GROUP_ALL_Zerg_Overlord) ||
            (i == HY_GROUP_ALL_OnTheWay_Creating) ||
            (i == HY_GROUP_ALL_IN_CREATING))
        {
            UnitMap *pGroup = &m_units[i];

            for (UnitMapIter iter = pGroup->begin(); iter != pGroup->end(); iter++)
            {
                Unit *pUnit = iter->second;

                if (pUnit)
                {
                    pUnit->ExecuteAction();
                }
            }
        }
    }

    // Send our idle workers to mine minerals so they don't just stand there
    // sendIdleWorkersToMinerals();
}

// Called on each frame of the game
void HyBot::onFrame()
{
    int nCount = g_nFrames % HY_FRAME_INTERVAL;

    // Update our MapTools information
    m_mapTools.onFrame();

    if (g_nFrames == 0)
    {
        // Send our idle workers to mine minerals so they don't just stand there
        sendIdleWorkersToMinerals();
    }

    if (nCount == 0) // Gather information
    {
        GetCurrentFrameState(g_nFrames);
    }
    else if (nCount == 1)  // Choose what to build
    {
        ExecuteBuildStrategy();
    }
    else if (nCount == 2)  // Build the unit and buildings in queue
    {
        // Worker Strategy
        WorkerStrategy();

        // ScoutStrategy
        ScoutStrategy();
    }
    else if (nCount == 3) // Unit action strategy.
    {
        ExecuteUnitAction();
    }
    else
    {
        BuildAdditionalSupply();
        BuildMissedDefender();
    }

    g_nFrames++;

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}


BWAPI::Unit HyBot::GetClosestAndNotFullUnit(Unit *pWorker, UnitMap *pResources)
{
    BWAPI::Position p           = pWorker->m_unit->getPosition();
    BWAPI::Unit     closestUnit = nullptr;

#if 0
    for (auto &u : units)
    {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }
#endif

    return closestUnit;
}

// Send our idle workers to mine minerals so they don't just stand there
void HyBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset &myUnits = BWAPI::Broodwar->self()->getUnits();

    for (auto &unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral)
            {
                unit->rightClick(closestMineral);
            }
        }
    }
}

// Train more workers so we can gather more income
void HyBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType    = BWAPI::Broodwar->self()->getRace().getWorker();
    const int             workersWanted = 20;
    const int             workersOwned  = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());

    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining())
        {
            myDepot->train(workerType);
        }
    }
}

void HyBot::BuildMissedDefender()
{
    UnitMap *pGroup = &m_units[HY_GROUP_ALL_Buildings];

    for (UnitMapIter iter = pGroup->begin(); iter != pGroup->end(); iter++)
    {
        Unit *pUnit = iter->second;

        if (pUnit->m_type == HY_ZERG_CREEP_COLONY && !pUnit->m_unit->isBeingConstructed())
        {
            if (pUnit->m_currentAction != HY_ACTION_UPGRADE_BUILDING)
            {
                float ratio = (float)m_totalGroundDefender / (float)m_totalAirDefender;

                if (ratio < 8.0f)
                {
                    pUnit->DistributeUpgradeAction(HY_ZERG_SUNKEN_COLONY);
                }
                else
                {
                    pUnit->DistributeUpgradeAction(HY_ZERG_SPORE_COLONY);
                }

                pUnit->ExecuteAction();
            }
            else
            {
                pUnit->ExecuteAction();
            }
        }
    }
}

// Build more supply if we are going to run out soon
void HyBot::BuildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType    = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
    const int             total                 = Tools::GetTotalSupply(true);
    const int             used                  = BWAPI::Broodwar->self()->supplyUsed();
    const int             unusedSupply          = total - used;
    const int             supplyInBuildingQueue = m_buildMgr.GetNumOfTypesInBuildQueue(supplyProviderType);

    if (supplyInBuildingQueue >= 2)
        return;

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 4 || total < 34)
    {
        return;
    }


    // Push provider to building manager.
    m_buildMgr.CreateBuildObj(supplyProviderType, HY_BUILD_PRI_HIGHER);
}

bool HyBot::IsUpgradeBuilding(BWAPI::UnitType type)
{
    if (m_race == HY_ZERG)
    {
        switch (type)
        {
        case HY_ZERG_HATCHERY:
        case HY_ZERG_CREEP_COLONY:
            return true;

        default:
            return false;
        }
    }

    return false;
}

// Draw some relevent information to the screen to help us debug the bot
void HyBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void HyBot::onUnitDestroy(BWAPI::Unit unit)
{
    const int ID     = unit->getID();
    Unit      *pUnit = FindUnit(ID);

    if (unit->getPlayer() != m_player)
    {
        TR_LOG(TR_DEBUG, TR_BUILDING, "%s: skip not our unit_%d destroy\n", unit->getType().getName().c_str(), unit->getID());
        return;
    }

    if (pUnit == nullptr)
    {
        TR_LOG(TR_ERROR, TR_BUILDING, "%s: Cannot find unit_%d for destroy.\n", unit->getType().getName().c_str(), ID);
    }
    else
    {
        m_currentFrameState.DecCurrentUnits(unit->getType());

        switch (pUnit->m_currentAction)
        {
        case HY_ACTION_GATHER_GAS:
        case HY_ACTION_GATHER_MINERAL:
            m_workMgr.RemoveUnit(ID);
            break;

        case HY_ACTION_SCOUTING:
            m_scoutMgr.RemoveUnit(ID);
            break;

        case HY_ACTION_CREATING_BUILDING:
            break;

        default:
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: There is no action(%d).\n", pUnit->m_name.c_str(), pUnit->m_currentAction);
            break;
        }

        // Remove destroyed unit from group.
        pUnit->RemoveFromGroups();
        delete pUnit;
    }
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void HyBot::onUnitMorph(BWAPI::Unit unit)
{
    int        ID        = unit->getID();
    const char *unitType = unit->getType().getName().c_str();

    if (m_player != unit->getPlayer())
    {
        TR_LOG(TR_DEBUG, TR_BUILDING, "%s: It's not my unit_%d to morph.\n", unitType, ID);
        return;
    }

    Unit *pUnit = FindUnit(ID);
    if (pUnit == nullptr)
    {
        pUnit = CreateUnit(unit, true);
        AddUnitToGroup(pUnit);

        // Zerg's building is morphed from Drone and its ID will be changed.
        m_units[HY_GROUP_ALL_IN_CREATING].erase(ID);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morph to %s.\n", pUnit->m_name.c_str(), unitType);

        // TR_LOG(TR_ERROR, TR_BUILDING, "%s: Cannot find unit_%d to morph.\n", unitType, ID);
        return;
    }

    // Moving it from Larva queue and put it into creating queue.
    if (pUnit->m_type == HY_ZERG_LARVA)
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morph to %s.\n", pUnit->m_name.c_str(), unitType);

        // Update unit information
        pUnit->Initialize(true);
    }
    else if (pUnit->m_type == HY_ZERG_EGG)
    {
        m_units[HY_GROUP_ALL_IN_CREATING].erase(ID);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morph to %s.\n", pUnit->m_name.c_str(), unitType);

        // Update unit information
        pUnit->Initialize(true);
        AddUnitToGroup(pUnit);
    }
    else if (pUnit->m_type == HY_ZERG_DRONE)
    {
        m_units[HY_GROUP_ALL_OnTheWay_Creating].erase(ID);
        m_units[HY_GROUP_ALL_Workers].erase(ID);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morph to %s.\n", pUnit->m_name.c_str(), unitType);

        // Update unit information
        pUnit->Initialize(true);
        AddUnitToGroup(pUnit);
    }
    else if (IsUpgradeBuilding(pUnit->m_type))
    {
        m_units[HY_GROUP_ALL_IN_CREATING].erase(ID);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morph to %s.\n", pUnit->m_name.c_str(), unitType);

        // Update unit information
        pUnit->Initialize(true);
        AddUnitToGroup(pUnit);
    }
    else
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_OnTheWay_Creating].begin(); iter != m_units[HY_GROUP_ALL_OnTheWay_Creating].end(); iter++)
        {
            Unit *pUnit = iter->second;

            if (pUnit->m_buildType == unit->getType())
            {
                TR_LOG(TR_INFO, TR_BUILDING, "%s: is deleted.\n", pUnit->m_name.c_str());
                DeleteUnitFromGroup(pUnit->m_ID, HY_GROUP_ALL_OnTheWay_Creating);
                return;
            }
        }

        // We should add new unit.
        Unit *pUnit = CreateUnit(unit, true);
        AddUnitToGroup(pUnit);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: Morphed.\n", pUnit->m_name.c_str());
    }
}

// Called whenever a text is sent to the game by a user
void HyBot::onSendText(std::string text)
{
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible,
// so this will trigger when you issue the build command for most units
void HyBot::onUnitCreate(BWAPI::Unit unit)
{
    int             ID   = unit->getID();
    BWAPI::UnitType type = unit->getType();

    if (unit->getPlayer() != m_player)
    {
        Unit *pUnit = FindUnit(ID);
        if (type == HY_MINERAL_UNIT && !pUnit)
        {
            pUnit                          = CreateUnit(unit, false);
            m_units[HY_GROUP_MINERALS][ID] = pUnit;
        }
        else if (type == HY_GAS_UNIT && !pUnit)
        {
            pUnit                     = CreateUnit(unit, false);
            m_units[HY_GROUP_GAS][ID] = pUnit;
        }
        else
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: skip not our unit_%d create\n", type.getName().c_str(), ID);
            return;
        }

        return;
    }

    if (m_race == HY_ZERG && unit->getType() != HY_ZERG_LARVA)
    {
        Unit *pUnit = FindUnit(ID);
        if (pUnit)
        {
            // Remove it from Larva group and put it into creating group.
            m_units[HY_GROUP_ALL_Zerg_Larva].erase(ID);
            m_units[HY_GROUP_ALL_IN_CREATING][ID] = pUnit;
            TR_LOG(TR_INFO, TR_BUILDING, "%s: not creating new one.\n", pUnit->m_name.c_str());
        }
        else
        {
            pUnit = CreateUnit(unit, true);
            m_units[HY_GROUP_ALL_Zerg_Larva].erase(ID);
            m_units[HY_GROUP_ALL_IN_CREATING][ID] = pUnit;
            TR_LOG(TR_INFO, TR_BUILDING, "%s: creating new one.\n", pUnit->m_name.c_str());
        }
    }
    else
    {
        Unit *pUnit = CreateUnit(unit, true);
        TR_LOG(TR_INFO, TR_BUILDING, "%s: creating new one\n", pUnit->m_name.c_str());
    }
}

// Called whenever a unit finished construction, with a pointer to the unit
void HyBot::onUnitComplete(BWAPI::Unit unit)
{
    BWAPI::UnitType type   = unit->getType();
    int             ID     = unit->getID();
    Unit            *pUnit = nullptr;

    if (unit->getPlayer() != m_player)
    {
        if (!(type == HY_MINERAL_UNIT || type == HY_GAS_UNIT))
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: skip not our unit_%d completed\n", unit->getType().getName().c_str(), ID);
            return;
        }
        else
        {
            if (type == HY_MINERAL_UNIT)
                pUnit = FindUnit(ID, HY_GROUP_MINERALS);
            else if (type == HY_GAS_UNIT)
                pUnit = FindUnit(ID, HY_GROUP_GAS);

            if (pUnit)
            {
                TR_LOG(TR_INFO, TR_BUILDING, "%s: is completed\n", pUnit->m_name.c_str());
            }
            else
            {
                TR_LOG(TR_ERROR, TR_BUILDING, "%s_%d: is completed\n", unit->getType().getName().c_str(), ID);
            }

            return;
        }
    }

    // Caculate the number of all units in type.
    m_currentFrameState.AddCurrentUnits(unit->getType());

    // If unit is finished, we should move it from creating queue to alive queue.
    pUnit = FindUnitInCreating(ID);

    if (pUnit == nullptr)
    {
        // If the unit is morphed from the other unit, just skip it.
        pUnit = FindUnit(ID);
        if (pUnit == nullptr)
        {
            // Create new units for zerg units which is morphed from other units.
            pUnit = CreateUnit(unit, true);
            AddUnitToGroup(pUnit);

            TR_LOG(TR_INFO, TR_BUILDING, "%s: is completed.\n", pUnit->m_name.c_str());
            return;
        }
        else
        {
            if (pUnit->m_bMorphed)
            {
                return;
            }
        }
    }

    TR_LOG(TR_INFO, TR_BUILDING, "%s is completed\n", pUnit->m_name.c_str());

    // Remove it from creating queue.
    m_units[HY_GROUP_ALL_IN_CREATING].erase(pUnit->m_ID);

    AddUnitToGroup(pUnit);
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void HyBot::onUnitShow(BWAPI::Unit unit)
{
    Unit *pUnit = FindUnit(unit->getID());

    if (pUnit)
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: show.\n", pUnit->m_name.c_str());
    }
    else
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: cannot find unit_%d to show.\n", unit->getType().getName().c_str(), unit->getID());
    }
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void HyBot::onUnitHide(BWAPI::Unit unit)
{
    int        ID        = unit->getID();
    const char *unitType = unit->getType().getName().c_str();

    if (m_player != unit->getPlayer())
    {
        TR_LOG(TR_DEBUG, TR_BUILDING, "%s: It's not my unit_%d to hide.\n", unitType, ID);
        return;
    }

    Unit *pUnit = FindUnit(ID);
    if (pUnit == nullptr)
    {
        TR_LOG(TR_ERROR, TR_BUILDING, "%s: Cannot find unit_%d to hide.\n", unitType, ID);
        return;
    }

    if (pUnit->m_type == HY_ZERG_DRONE)
    {
        m_units[HY_GROUP_ALL_OnTheWay_Creating].erase(ID);
        AddUnitToGroup(pUnit, HY_GROUP_ALL_IN_CREATING);

        if (pUnit->m_type == HY_ZERG_DRONE)
        {
            TR_LOG(TR_INFO, TR_BUILDING, "%s: Hide for %s.\n", pUnit->m_name.c_str(), pUnit->m_buildType.getName().c_str());
        }
        else
        {
            TR_LOG(TR_INFO, TR_BUILDING, "%s: Hide.\n", pUnit->m_name.c_str());
        }
    }
    else
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: Unprocessed Hide for %s.\n", pUnit->m_name.c_str(), unitType);
    }
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void HyBot::onUnitRenegade(BWAPI::Unit unit)
{
    Unit *pUnit = FindUnit(unit->getID());

    if (pUnit)
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: Renegade.\n", pUnit->m_name.c_str());
    }
    else
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: cannot find unit_%d to Renegade.\n", unit->getType().getName().c_str(), unit->getID());
    }
}

void HyBot::BuildStrategyStart()
{
    BuildFromStrategy(m_StrategyTime);
}

void HyBot::BuildStrategyMiddle()
{
    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    BuildAdditionalSupply();
}

void HyBot::BuildStrategyEnd()
{}


bool HyBot::CheckBuildCondition(const UnitTypeAndCondition *pUnit)
{
    bool bRes = true;

    do
    {
        // Check minerals is satisfied.
        if (pUnit->condition.waitForMinerals > 0)
        {
            if (pUnit->condition.waitForMinerals > m_currentFrameState.m_minerals)
            {
                bRes = false;
                break;
            }
        }

        // Check gas
        if (pUnit->condition.waitForGas > 0)
        {
            if (pUnit->condition.waitForGas > m_currentFrameState.m_gas)
            {
                bRes = false;
                break;
            }
        }

        // previous building is finished
        if (pUnit->condition.waitForPriorBuild != BWAPI::UnitTypes::Enum::Unknown)
        {
            int             nNum = pUnit->condition.waitBuildingNum;
            BWAPI::UnitType type = pUnit->condition.waitForPriorBuild;

            if (nNum <= m_currentFrameState.m_currentUnits[type])
            {
                bRes = true;
                break;
            }
            else
            {
                bRes = false;
                break;
            }
        }
    }
    while (0);

    return bRes;
}

void HyBot::BuildFromStrategy(StrategyTime time)
{
    BuildingStrategy *pStrategy = m_pBuildStrategy[time];

    if (pStrategy == nullptr)
    {
        // There is nothing to build.
        return;
    }

    UnitTypeAndCondition *pUnitForCreate = pStrategy->GetCurrentUnit();
    if (!pUnitForCreate)
    {
        TR_LOG(TR_DEBUG, TR_BUILDING, "There is nothing to create.\n");
        return;
    }

    if (pStrategy->IsTheLastOne())
    {
        TR_LOG(TR_DEBUG, TR_BUILDING, "The last one will be created in this strategy.\n");
    }

    BWAPI::UnitType type  = pUnitForCreate->type;
    int             count = pUnitForCreate->count;

    for (int j = 0; j < count; j++)
    {
        // Check the condition of unit is satisfied.
        if (pUnitForCreate && CheckBuildCondition(pUnitForCreate))
        {
            bool bMet = m_player->isUnitAvailable(pUnitForCreate->type);
            if (bMet)
            {
                m_buildMgr.CreateBuildObj(type, HY_BUILD_PRI_LOW, pUnitForCreate);
                m_currentFrameState.m_gas      -= type.gasPrice();
                m_currentFrameState.m_minerals -= type.mineralPrice();

                pUnitForCreate->count--;
            }
            else
            {
                TR_LOG(TR_ERROR, TR_BUILDING, "%s: unit build is not available. \n", type.getName().c_str());
                break;
            }
        }
        else
        {
            break;
        }

        if (pUnitForCreate->count == 0)
        {
            m_pBuildStrategy[m_StrategyTime]->MoveToNextUnit();
        }
    }
}


void HyBot::GetCurrentFrameState(int nFrame)
{
    m_currentFrameState.m_gas         = m_player->gas();
    m_currentFrameState.m_minerals    = m_player->minerals();
    m_currentFrameState.m_totalSupply = m_player->supplyTotal();
    m_currentFrameState.m_usedSupply  = m_player->supplyUsed();

    // We should minus the cost that workers will create some buildings.
    UnitMapIter iter;

    for (iter = m_units[HY_GROUP_ALL_OnTheWay_Creating].begin(); iter != m_units[HY_GROUP_ALL_OnTheWay_Creating].end(); iter++)
    {
        Unit            *pUnit = iter->second;
        BWAPI::UnitType type   = pUnit->m_buildType;

        TR_LOG(TR_DEBUG, TR_RESOURCE, "minerals(%d), gas(%d), - %s %d %d\n",
               m_currentFrameState.m_minerals, m_currentFrameState.m_gas, type.getName().c_str(), type.mineralPrice(), type.gasPrice());
        m_currentFrameState.m_gas      -= type.gasPrice();
        m_currentFrameState.m_minerals -= type.mineralPrice();
    }

    for (iter = m_units[HY_GROUP_ALL_IN_CREATING].begin(); iter != m_units[HY_GROUP_ALL_IN_CREATING].end(); iter++)
    {
        Unit *pUnit         = iter->second;
        bool isConstructing = pUnit->m_unit->isConstructing();
        bool isMoving       = pUnit->m_unit->isMoving();

        if (isConstructing && isMoving)
        {
            BWAPI::UnitType type = pUnit->m_unit->getBuildType();

            TR_LOG(TR_DEBUG, TR_RESOURCE, "minerals(%d), gas(%d), - %s %d %d\n",
                   m_currentFrameState.m_minerals, m_currentFrameState.m_gas, type.getName().c_str(), type.mineralPrice(), type.gasPrice());
            m_currentFrameState.m_gas      -= type.gasPrice();
            m_currentFrameState.m_minerals -= type.mineralPrice();
        }
    }

    if ((nFrame % HY_STORE_FRAME_STATE_INTERVAL) == 0)
    {
        m_frameStates[m_storedFrameStateIndex++] = m_currentFrameState;
    }

    TR_LOG(TR_DEBUG, TR_RESOURCE, "current minerals(%d), gas(%d)\n",
           m_currentFrameState.m_minerals, m_currentFrameState.m_gas);
}

void HyBot::GetAllUnits()
{
    for (auto &unit : BWAPI::Broodwar->self()->getUnits())
    {
        Unit *pUnit = CreateUnit(unit, false);
        AddUnitToGroup(pUnit);
    }
}

Unit* HyBot::CreateUnit(BWAPI::Unit unit, bool bCreate)
{
    std::string name = unit->getType().getName();

    Unit *pUnit = new Unit(unit, this);

    if (pUnit == nullptr)
    {
        TR_LOG(TR_ERROR, TR_BUILDING, "%s: Cannot create new unit.\n", name.c_str());
        return nullptr;
    }

    TR_LOG(TR_INFO, TR_BUILDING, "%s: created.\n", pUnit->m_name.c_str());
    if (bCreate)
    {
        // add it to creating queue
        HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_IN_CREATING, pUnit);
    }

    return pUnit;
}

Unit* HyBot::GetFirstGroundScouter()
{
    return m_scoutMgr.m_pFirstGroundUnit;
}

void HyBot::AddUnitToGroup(Unit *pUnit)
{
    // Add it to different alive queue.
    // There are several special groups, such as Zerg_Overlord, Zerg_Larva
    //   1. Overlord of Zerg has several capabilities, such as flying, detecting, supply and transport.
    //   2. Larva of Zerg is used to create other units of Zerg.
    if (HY_IS_ZERG_OVERLORD(pUnit))
    {
        HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_Zerg_Overlord, pUnit);
        return;
    }
    else if (HY_IS_ZERG_LARVA(pUnit))
    {
        HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_Zerg_Larva, pUnit);
        return;
    }
    else     // Select by capabilities.
    {
        if (HY_IS_WORKER(pUnit))
        {
            HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_Workers, pUnit);
            return;
        }
        else if (HY_IS_BUILDING(pUnit))
        {
            HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_Buildings, pUnit);
            return;
        }
        else if (HY_IS_FLYING_UNIT(pUnit))
        {
            HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_AirUnits, pUnit);
            return;
        }
        else
        {
            HY_ADD_UNIT_TO_GROUP(HY_GROUP_ALL_GroudUnits, pUnit);
            return;
        }
    }
}

void HyBot::AddUnitToGroup(BWAPI::Unit unit)
{
    Unit *pUnit = FindUnit(unit->getID());

    if (pUnit)
    {
        AddUnitToGroup(pUnit);
    }
}

Unit* HyBot::FindUnit(int ID, UnitGroup group)
{
    Unit *pUnit = nullptr;

    switch (group)
    {
    case HY_GROUP_ALL:

        for (int i = 0; i < HY_GROUP_ALL; i++)
        {
            pUnit = FindUnit(ID, (UnitGroup)i);
            if (pUnit)
            {
                return pUnit;
            }
        }

        break;

        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_IN_CREATING);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_Buildings);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_Workers);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_AirUnits);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_GroudUnits);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_OtherUnits);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_Zerg_Larva);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_Zerg_Overlord);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_ALL_OnTheWay_Creating);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_MINERALS);
        HY_SEARCH_GROUP_BY_ID(ID, HY_GROUP_GAS);

    default:     // Wrong group.
        TR_LOG(TR_ERROR, TR_BUILDING, "Wrong unit group: %d\n", group);
        break;
    }

    return nullptr;
}

Unit* HyBot::FindUnitInCreating(int ID)
{
    return FindUnit(ID, HY_GROUP_ALL_IN_CREATING);
}


void HyBot::AddUnitToGroup(Unit *pUnit, UnitGroup group)
{
    HY_ADD_UNIT_TO_GROUP(group, pUnit);
}

UnitMap* HyBot::GetUnitGroup(UnitGroup group)
{
    UnitMap *p = (UnitMap*)&m_units[group];

    return p;
}

UnitGroup HyBot::ConvertUnitTypeToGroup(BWAPI::UnitType type)
{
    switch (type)
    {
    case HY_ZERG_DRONE:
        return HY_GROUP_ALL_Workers;

    case HY_ZERG_OVERLORD:
        return HY_GROUP_ALL_Zerg_Overlord;

    case HY_ZERG_LARVA:
        return HY_GROUP_ALL_Zerg_Larva;

    default:
        return HY_GROUP_ALL;
    }
}

Unit* HyBot::ChooseUnit(BWAPI::UnitType type, BWAPI::TilePosition workerPos)
{
    BWAPI::Position p(workerPos);

    return ChooseUnit(type, p);
}

Unit* HyBot::ChooseUnit(BWAPI::UnitType type, BWAPI::Position workerPos)
{
    Unit *pClosestUnit = nullptr;
    Unit *pUnit        = nullptr;

    if (type.isWorker())
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_Workers].begin(); iter != m_units[HY_GROUP_ALL_Workers].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->IsResourceGathering())
                    pClosestUnit = pUnit;
            }
        }
    }
    else if (type.isBuilding())
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_Buildings].begin(); iter != m_units[HY_GROUP_ALL_Buildings].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->m_type == type)
                    pClosestUnit = pUnit;
            }
        }
    }
    else if (type == HY_ZERG_OVERLORD)
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_Zerg_Overlord].begin(); iter != m_units[HY_GROUP_ALL_Zerg_Overlord].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->m_type == type)
                    pClosestUnit = pUnit;
            }
        }
    }
    else if (type == HY_ZERG_LARVA)
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_Zerg_Larva].begin(); iter != m_units[HY_GROUP_ALL_Zerg_Larva].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->m_type == type)
                    pClosestUnit = pUnit;
            }
        }
    }
    else if (type.isFlyer())
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_AirUnits].begin(); iter != m_units[HY_GROUP_ALL_AirUnits].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->m_type == type)
                    pClosestUnit = pUnit;
            }
        }
    }
    else
    {
        for (UnitMapIter iter = m_units[HY_GROUP_ALL_GroudUnits].begin(); iter != m_units[HY_GROUP_ALL_GroudUnits].end(); iter++)
        {
            pUnit = iter->second;
            if (!pClosestUnit || pUnit->m_unit->getDistance(workerPos) < pClosestUnit->m_unit->getDistance(workerPos))
            {
                if (pUnit->m_type == type)
                    pClosestUnit = pUnit;
            }
        }
    }

    return pClosestUnit;
}

void HyBot::DeleteUnitFromGroup(int ID, UnitGroup group)
{
    Unit *p = m_units[group][ID];

    if (p)
    {
        TR_LOG(TR_INFO, TR_BUILDING, "%s: is deleted.\n", p->m_name.c_str());
        p->RemoveFromGroups();
        delete p;
    }
}

void HyBot::MoveUnitGroup(UnitGroup from, UnitGroup to, Unit *pUnit)
{
    m_units[from].erase(pUnit->m_ID);
    AddUnitToGroup(pUnit, to);
}

//
// Building Objects
//
BuildingObj::BuildingObj(BWAPI::UnitType unitType, BuildPriority prior)
{
    m_buildPrior    = prior; // default is HY_BUILD_PRI_LOW
    m_buildType     = unitType;
    m_Minerals      = unitType.mineralPrice();
    m_Gas           = unitType.gasPrice();
    m_buildTime     = unitType.buildTime();
    m_buildID       = -1;
    m_nearByBuildID = -1;
    m_locMethod     = HY_BUILD_LOC_STARTLOC;
    m_locRange      = 64;
}

BuildingObj::~BuildingObj()
{}


//
// Building Manager
//
BuildingManager::BuildingManager(HyBot *pBot)
{
    m_pBot = pBot;
}

BuildingManager::~BuildingManager()
{}


void BuildingManager::SetPlayer(BWAPI::Player player)
{
    m_player = player;
}

void BuildingManager::AddBuildingToBuild(BuildingObj *pBuilding)
{
    m_NeedToBuild[pBuilding->m_buildPrior].push_back(pBuilding);
}

void BuildingManager::CreateBuildObj(BWAPI::UnitType unitType, BuildPriority prior, UnitTypeAndCondition *pUnitForCreate)
{
    BuildingObj *pBuilding = nullptr;

    pBuilding = new BuildingObj(unitType, prior);
    if (pBuilding == nullptr)
    {
        TR_LOG(TR_ERROR, TR_BUILDING, "%s: Cannot create building object.\n", unitType.getName().c_str());
    }

    if (pUnitForCreate != nullptr)
    {
        // add new building to building queue, it will be prioritised before building.
        pBuilding->m_buildID       = pUnitForCreate->buildID;
        pBuilding->m_nearByBuildID = pUnitForCreate->locationID;
        pBuilding->m_locMethod     = pUnitForCreate->locMethod;
        pBuilding->m_locRange      = pUnitForCreate->locRange;
    }

    AddBuildingToBuild(pBuilding);
    TR_LOG(TR_INFO, TR_BUILDING, "%s: add building object to building queue.\n", unitType.getName().c_str());

    return;
}

void BuildingManager::DeleteBuildObj(BuildingObj *pBuilding)
{
    if (pBuilding)
        delete pBuilding;
}

void BuildingManager::PrioritiseBuildingQueue()
{
    BuildBuildingQueue nullQueue;

    // Clear the last queue.
    std::swap(m_BuildingQueue, nullQueue);

    // Add buildings from needed built buildings array to this queue.
    // Just simply push every building to queue.
    for (int i = HY_BUILD_PRI_HIGHEST; i >= 0; i--)
    {
        const size_t n = m_NeedToBuild[i].size();
        if (n == 0)
            continue;

        for (size_t j = 0; j < n; j++)
        {
            BuildingObj *p = m_NeedToBuild[i].at(j);

            m_BuildingQueue.push(p);
        }
    }
}

int BuildingManager::GetNumOfTypesInBuildQueue(BWAPI::UnitType unitType)
{
    BuildingObj *pBuilding = nullptr;
    size_t      queueSize  = m_BuildingQueue.size();
    int         count      = 0;

    for (size_t i = 0; i < queueSize; i++)
    {
        pBuilding = m_BuildingQueue.front();
        m_BuildingQueue.pop();

        if (pBuilding->m_buildType == unitType)
            count++;
    }

    return count;
}

void BuildingManager::StartBuilding()
{
    BuildingObj *pBuilding      = nullptr;
    size_t      queueSize       = m_BuildingQueue.size();
    int         currentMinerals = m_player->minerals();
    int         currentGas      = m_player->gas();
    int         builderID       = -1;

    for (size_t i = 0; i < queueSize; i++)
    {
        pBuilding = m_BuildingQueue.front();
        m_BuildingQueue.pop();

        if (pBuilding->m_Minerals <= currentMinerals)
        {
            // Start to building.
            builderID = DistributeBuildAction(pBuilding);
        }

        // Cannot build the building, push it back to queue.
        if (builderID == -1)
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: Cannot build.\n", pBuilding->m_buildType.getName().c_str());
            m_BuildingQueue.push(pBuilding);
        }
        else
        {
            BuildPriority priority = pBuilding->m_buildPrior;

            TR_LOG(TR_INFO, TR_BUILDING, "%s: Start to build.\n", pBuilding->m_buildType.getName().c_str());

            // Remove it from m_NeedToBuild.
            for (size_t j = 0; j < m_NeedToBuild[priority].size(); j++)
            {
                if (pBuilding == m_NeedToBuild[priority].at(j))
                {
                    m_NeedToBuild[priority].erase(m_NeedToBuild[priority].begin() + j);
                    break;
                }
            }

            // Release memory.
            DeleteBuildObj(pBuilding);
        }
    }
}

Unit* BuildingManager::GetBuilderUnit(BWAPI::UnitType type, BWAPI::TilePosition pos)
{
    UnitMap *p     = nullptr;
    Unit    *pUnit = nullptr;

    if (m_player->getRace() == HY_ZERG)
    {
        if (type == HY_ZERG_LARVA)
        {
            p = m_pBot->GetUnitGroup(HY_GROUP_ALL_Zerg_Larva);
        }
        else if (type == HY_ZERG_DRONE)
        {
            p = m_pBot->GetUnitGroup(HY_GROUP_ALL_Workers);
        }
        else if (m_pBot->IsUpgradeBuilding(type))
        {
            p = m_pBot->GetUnitGroup(HY_GROUP_ALL_Buildings);
        }
        else
        {
            TR_LOG(TR_ERROR, TR_BUILDING, "%s: There is no such group to look for builder.\n", type.getName().c_str());
            return nullptr;
        }
    }

    UnitMapIter iter;

    for (iter = p->begin(); iter != p->end(); iter++)
    {
        pUnit = iter->second;
        if (pUnit->m_bUsed == false && type == pUnit->m_type && pUnit->m_currentAction == HY_ACTION_IDLE)
            return pUnit;
        else
            pUnit = nullptr;
    }

    if (type.isWorker())
    {
        pUnit = m_pBot->ChooseUnit(type, pos);
    }

    return pUnit;
}

BWAPI::TilePosition BuildingManager::GetBuildPos(BWAPI::TilePosition pos, BWAPI::UnitType type, int maxBuildRange, bool buildingOnCreep, bool nearByPos)
{
    BWAPI::TilePosition buildPos;

    if (nearByPos)
    {
        buildPos = m_pBot->m_mapTools.GetNearestByPos(type, pos, buildingOnCreep);
        return buildPos;
    }

    if (buildingOnCreep)
    {
        if (type == HY_ZERG_CREEP_COLONY)
        {
            buildPos = m_pBot->m_mapTools.GetTheFarestCreepAgainstEnemy(type, pos);
        }
        else
        {
            buildPos = BWAPI::Broodwar->getBuildLocation(type, pos, maxBuildRange, buildingOnCreep);
        }
    }
    else
    {
        if (type == HY_ZERG_HATCHERY)
        {
            buildPos = m_pBot->m_mapTools.GetTheFarestCreepAgainstEnemy(type, pos);
        }
        else
        {
            buildPos = BWAPI::Broodwar->getBuildLocation(type, pos, maxBuildRange, buildingOnCreep);
        }
    }

    return buildPos;
}

int BuildingManager::DistributeBuildAction(BuildingObj *pBuilding)
{
    BWAPI::UnitType     type            = pBuilding->m_buildType;
    int                 buildID         = pBuilding->m_buildID;
    int                 builderID       = -1;
    Unit                *builder        = nullptr;
    BWAPI::TilePosition defaultBuildPos = m_pBot->m_startPos;
    bool                bNearByPos      = false;

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;

    // Get the type of unit that is required to build the desired building
    const BWAPI::UnitType builderType = type.whatBuilds().first;

    if (builderType == HY_ZERG_LARVA)
    {
        // Get a unit that we own that is of the given type so it can build
        // If we can't find a valid builder unit, then we have to cancel the building
        builder = GetBuilderUnit(builderType, m_pBot->m_startPos);

        if (!builder)
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: Cannot find worker for building.\n", type.getName().c_str());
            return builderID;
        }

        builder->DistributeCreateAction(type);
        m_pBot->MoveUnitGroup(HY_GROUP_ALL_Zerg_Larva, HY_GROUP_ALL_IN_CREATING, builder);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: distribute build %s\n", builder->m_name.c_str(), type.getName().c_str());
    }
    else if (m_pBot->IsUpgradeBuilding(builderType))
    {
        // Get a unit that we own that is of the given type so it can build
        // If we can't find a valid builder unit, then we have to cancel the building
        builder = GetBuilderUnit(builderType, m_pBot->m_startPos);

        if (!builder)
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: Cannot find worker for building.\n", type.getName().c_str());
            return builderID;
        }

        builder->DistributeUpgradeAction(type);
        m_pBot->MoveUnitGroup(HY_GROUP_ALL_Buildings, HY_GROUP_ALL_IN_CREATING, builder);

        TR_LOG(TR_INFO, TR_BUILDING, "%s: distribute build %s\n", builder->m_name.c_str(), type.getName().c_str());
    }
    else
    {
        const bool buildingOnCreep = type.requiresCreep();

        // The first built
        if (m_pBot->m_currentFrameState.m_currentUnits[type] == 0)
        {
            maxBuildRange = 32;
        }

        // Based on nearest build id.
        switch (pBuilding->m_locMethod)
        {
        case HY_BUILD_LOC_NEARBY:
        {
            Unit *pNearBy = nullptr;

            // Find the building.
            UnitMap *pGroup = m_pBot->GetUnitGroup(HY_GROUP_ALL_Buildings);

            for (UnitMapIter iter = pGroup->begin(); iter != pGroup->end(); iter++)
            {
                Unit *p = iter->second;

                if (p->m_buildID == pBuilding->m_nearByBuildID)
                {
                    pNearBy = p;
                    break;
                }
            }

            if (pNearBy)
            {
                defaultBuildPos = BWAPI::TilePosition(pNearBy->m_unit->getPosition());
                bNearByPos      = true;
            }
        }
        break;

        case HY_BUILD_LOC_RANGE:
        case HY_BUILD_LOC_STARTLOC:
            defaultBuildPos = m_pBot->m_startPos;
            maxBuildRange   = pBuilding->m_locRange;
            break;

        case HY_BUILD_LOC_RANDOM:
            maxBuildRange = m_pBot->m_mapWidth > m_pBot->m_mapHeight ? m_pBot->m_mapWidth : m_pBot->m_mapHeight;
            break;

        default:
            defaultBuildPos = m_pBot->m_startPos;
            break;
        }

        BWAPI::TilePosition buildPos = GetBuildPos(defaultBuildPos, type, maxBuildRange, buildingOnCreep, bNearByPos);
        // Get a unit that we own that is of the given type so it can build
        // If we can't find a valid builder unit, then we have to cancel the building
        builder = GetBuilderUnit(builderType, buildPos);

        if (!builder)
        {
            TR_LOG(TR_DEBUG, TR_BUILDING, "%s: Cannot find worker for building.\n", type.getName().c_str());
            return builderID;
        }

        builder->DistributeCreateAction(type, buildPos, buildID);
        m_pBot->MoveUnitGroup(HY_GROUP_ALL_Workers, HY_GROUP_ALL_OnTheWay_Creating, builder);
        m_pBot->m_workMgr.RemoveUnit(builder->m_ID);

        // If removeing worker from worker queue of Zerg, we should build another worker to do the same thing.
        if (m_pBot->m_race == HY_ZERG)
        {
            CreateBuildObj(HY_ZERG_DRONE, HY_BUILD_PRI_HIGH);
        }

        TR_LOG(TR_INFO, TR_BUILDING, "%s: distribute build %s\n", builder->m_name.c_str(), type.getName().c_str());
    }

    return builder->m_ID;
}

//
// FrameState
//
FrameState::FrameState()
{
    m_totalSupply = 0;

    memset(m_currentUnits, 0, sizeof(unsigned char) * BWAPI::UnitTypes::Enum::MAX);
}

FrameState::~FrameState()
{}