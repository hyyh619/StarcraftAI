#include "HyDebugLog.h"
#include "Tools.h"
#include "HyBuildingStrategy.h"

//
// Prepared building sequences
//

// 9 Pool Speed
// Build order:
//   9 / 9 - Spawning Pool
//   8 / 9 - Drone
//   9 / 9 - Extractor
//   8 / 9 - Overlord
//   8 / 17 - Drone
//   @100 % Extractor - send 3 Drones to mine gas
//   @100 % Spawning Pool - 6 Zerglings
//   @88 Gas - take Drones off of gas one by one, giving you 104 gas.Research Speed.
//   12 / 17 - 2 Zerglings(optional) (see note)
#if 0
#if 0
UnitTypeAndCondition g_Zerg_9PoolSpeed[ZERG_9_POOL_SPEED] =
{
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, {BWAPI::UnitTypes::Enum::Unknown, 0, 200, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Extractor, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Overlord, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0} // 8 units
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Zerg_Overlord, 100, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Zergling, {BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 100, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Zergling, {BWAPI::UnitTypes::Enum::Unknown, 100, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Zergling, {BWAPI::UnitTypes::Enum::Unknown, 100, 50, 0}  // 14 units
    }
};
#else
UnitTypeAndCondition g_Zerg_9PoolSpeed[ZERG_9_POOL_SPEED] =
{
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 4, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 200, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Extractor, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}
    },
    {BWAPI::UnitTypes::Enum::Zerg_Overlord, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 100, 0} // 7 units, total: 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Zergling, 3, {BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool, 1, 150, 0} // 10 units total  17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 50, 0}  // 11 units, total 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 3, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0}  // 14 units, total 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Creep_Colony, 2, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0}  // 14 units, total 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Sunken_Colony, 2, {BWAPI::UnitTypes::Enum::Unknown, 0, 100, 0}  // 14 units, total 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Zergling, 2, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0} // 17 units total  17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Overlord, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 100, 0} // 17 units, total: 25
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 3, {BWAPI::UnitTypes::Enum::Zerg_Overlord, 3, 150, 0}  // 20 units, total 17
    },
    {BWAPI::UnitTypes::Enum::Zerg_Hatchery, 1, {BWAPI::UnitTypes::Enum::Unknown, 0, 300, 0} // 19 units total 25
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 3, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0}  // 22 units, total 25
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 3, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0}  // 25 units, total 25
    },
    {BWAPI::UnitTypes::Enum::Zerg_Drone, 3, {BWAPI::UnitTypes::Enum::Unknown, 0, 150, 0}  // 28 units, total 25
    },
    {BWAPI::UnitTypes::Enum::Zerg_Overlord, 2, {BWAPI::UnitTypes::Enum::Unknown, 0, 100, 0} // 28 units, total: 41
    },
};
#endif
#endif

std::map<std::string, BuildLocMethod> g_buildLocMethodMap = {
    {"random", HY_BUILD_LOC_RANDOM},
    {"range", HY_BUILD_LOC_RANGE},
    {"nearby", HY_BUILD_LOC_NEARBY},
    {"startloc", HY_BUILD_LOC_STARTLOC},
    {"max", HY_BUILD_LOC_MAX}
};

//
// Building Strategy
//
BuildingStrategy::BuildingStrategy(UnitTypeAndCondition *pBuildings, int total) :
    m_parser(std::string(""))
{
    m_pBuildingsSequence = pBuildings;
    m_totalBuildings     = total;
    m_currentBuildings   = 0;
    m_checkFrame         = 0;
}

BuildingStrategy::BuildingStrategy(const std::string strategyFile) :
    m_parser(strategyFile)
{
    ParseStrategyRoot(m_parser.m_root);
}

BuildingStrategy::~BuildingStrategy()
{}

UnitTypeAndCondition* BuildingStrategy::GetCurrentUnit()
{
    UnitTypeAndCondition *p;

    if (m_currentBuildings >= m_totalBuildings)
        return nullptr;

    p = (UnitTypeAndCondition* )&m_pBuildingsSequence[m_currentBuildings];
    return p;
}

void BuildingStrategy::MoveToNextUnit()
{
    m_currentBuildings++;
}

bool BuildingStrategy::IsTheLastOne()
{
    if ((m_currentBuildings + 1) == m_totalBuildings)
        return true;
    else
        return false;
}

bool BuildingStrategy::ParseStrategyRoot(Json::Value &root)
{
    if (root.type() != Json::arrayValue)
    {
        TR_LOG(TR_ERROR, TR_BUILDING, "Wrong root\n");
        return false;
    }

    Json::ArrayIndex size = root.size();

    // Based on array size to allocation strategy items.
    m_pBuildingsSequence = new UnitTypeAndCondition[size];
    m_totalBuildings     = size;
    m_currentBuildings   = 0;

    for (Json::ArrayIndex index = 0; index < size; ++index)
    {
        ParseStrategyValue(root[index], &m_pBuildingsSequence[index]);
    }

    return true;
}

void BuildingStrategy::ParseStrategyValue(Json::Value &value, UnitTypeAndCondition *p)
{
    if (value.hasComment(Json::commentBefore))
    {
        m_comments[0] = value.getComment(Json::commentBefore);
    }

    switch (value.type())
    {
    case Json::arrayValue:
    {
        Json::ArrayIndex size = value.size();

        // Based on array size to allocation strategy items.
        m_pBuildingsSequence = new UnitTypeAndCondition[size];
        m_totalBuildings     = size;
        m_currentBuildings   = 0;


        for (Json::ArrayIndex index = 0; index < size; ++index)
        {
            ParseStrategyValue(value[index], &m_pBuildingsSequence[index]);
        }
    }
    break;

    case Json::objectValue:
    {
        Json::Value::Members members(value.getMemberNames());
        std::sort(members.begin(), members.end());

        for (const auto &name : members)
        {
            if (name == "buildType")
            {
                p->buildType = std::string(value[name].asString().c_str());
                p->type = g_unitTypeMap[p->buildType];
            }
            else if (name == "num")
            {
                p->count = value[name].asInt();
            }
            else if (name == "minerals")
            {
                p->condition.waitForMinerals = value[name].asInt();
            }
            else if (name == "gas")
            {
                p->condition.waitForGas = value[name].asInt();
            }
            else if (name == "waitBuilding")
            {
                p->condition.waitBuilding = std::string(value[name].asString().c_str());
                p->condition.waitForPriorBuild = g_unitTypeMap[p->condition.waitBuilding];
            }
            else if (name == "waitBuildingNum")
            {
                p->condition.waitBuildingNum = value[name].asInt();
            }
            else if (name == "comment")
            {
                p->comment = std::string(value[name].asString().c_str());
            }
            else if (name == "buildID")
            {
                p->buildID = value[name].asInt();
            }
            else if (name == "locationID")
            {
                p->locationID = value[name].asInt();
            }
            else if (name == "location")
            {
                p->locMethod = g_buildLocMethodMap[std::string(value[name].asString())];
            }
            else if (name == "locationRange")
            {
                p->locRange = value[name].asInt();
            }
            else
            {
                TR_LOG(TR_ERROR, TR_BUILDING, "Unknown name: %s\n", name.c_str());
            }
        }
    }
    break;

    default:
        break;
    }

    if (value.hasComment(Json::commentAfter))
    {
        m_comments[1] = value.getComment(Json::commentAfter);
    }
}