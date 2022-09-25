#include "Defines.h"
#include "HyDebugLog.h"
#include "Tools.h"


std::map<std::string, BWAPI::UnitType> g_unitTypeMap =
{
    {"Terran_Marine", BWAPI::UnitTypes::Enum::Terran_Marine},
    {"Terran_Ghost", BWAPI::UnitTypes::Enum::Terran_Ghost},
    {"Terran_Vulture", BWAPI::UnitTypes::Enum::Terran_Vulture},
    {"Terran_Goliath", BWAPI::UnitTypes::Enum::Terran_Goliath},
    {"Terran_Goliath_Turret", BWAPI::UnitTypes::Enum::Terran_Goliath_Turret},
    {"Terran_Siege_Tank_Tank_Mode", BWAPI::UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode},
    {"Terran_Siege_Tank_Tank_Mode_Turret", BWAPI::UnitTypes::Enum::Terran_Siege_Tank_Tank_Mode_Turret},
    {"Terran_SCV", BWAPI::UnitTypes::Enum::Terran_SCV},
    {"Terran_Wraith", BWAPI::UnitTypes::Enum::Terran_Wraith},
    {"Terran_Science_Vessel", BWAPI::UnitTypes::Enum::Terran_Science_Vessel},
    {"Hero_Gui_Montag", BWAPI::UnitTypes::Enum::Hero_Gui_Montag},
    {"Terran_Dropship", BWAPI::UnitTypes::Enum::Terran_Dropship},
    {"Terran_Battlecruiser", BWAPI::UnitTypes::Enum::Terran_Battlecruiser},
    {"Terran_Vulture_Spider_Mine", BWAPI::UnitTypes::Enum::Terran_Vulture_Spider_Mine},
    {"Terran_Nuclear_Missile", BWAPI::UnitTypes::Enum::Terran_Nuclear_Missile},
    {"Terran_Civilian", BWAPI::UnitTypes::Enum::Terran_Civilian},
    {"Hero_Sarah_Kerrigan", BWAPI::UnitTypes::Enum::Hero_Sarah_Kerrigan},
    {"Hero_Alan_Schezar", BWAPI::UnitTypes::Enum::Hero_Alan_Schezar},
    {"Hero_Alan_Schezar_Turret", BWAPI::UnitTypes::Enum::Hero_Alan_Schezar_Turret},
    {"Hero_Jim_Raynor_Vulture", BWAPI::UnitTypes::Enum::Hero_Jim_Raynor_Vulture},
    {"Hero_Jim_Raynor_Marine", BWAPI::UnitTypes::Enum::Hero_Jim_Raynor_Marine},
    {"Hero_Tom_Kazansky", BWAPI::UnitTypes::Enum::Hero_Tom_Kazansky},
    {"Hero_Magellan", BWAPI::UnitTypes::Enum::Hero_Magellan},
    {"Hero_Edmund_Duke_Tank_Mode", BWAPI::UnitTypes::Enum::Hero_Edmund_Duke_Tank_Mode},
    {"Hero_Edmund_Duke_Tank_Mode_Turret", BWAPI::UnitTypes::Enum::Hero_Edmund_Duke_Tank_Mode_Turret},
    {"Hero_Edmund_Duke_Siege_Mode", BWAPI::UnitTypes::Enum::Hero_Edmund_Duke_Siege_Mode},
    {"Hero_Edmund_Duke_Siege_Mode_Turret", BWAPI::UnitTypes::Enum::Hero_Edmund_Duke_Siege_Mode_Turret},
    {"Hero_Arcturus_Mengsk", BWAPI::UnitTypes::Enum::Hero_Arcturus_Mengsk},
    {"Hero_Hyperion", BWAPI::UnitTypes::Enum::Hero_Hyperion},
    {"Hero_Norad_II", BWAPI::UnitTypes::Enum::Hero_Norad_II},
    {"Terran_Siege_Tank_Siege_Mode", BWAPI::UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode},
    {"Terran_Siege_Tank_Siege_Mode_Turret", BWAPI::UnitTypes::Enum::Terran_Siege_Tank_Siege_Mode_Turret},
    {"Terran_Firebat", BWAPI::UnitTypes::Enum::Terran_Firebat},
    {"Spell_Scanner_Sweep", BWAPI::UnitTypes::Enum::Spell_Scanner_Sweep},
    {"Terran_Medic", BWAPI::UnitTypes::Enum::Terran_Medic},
    {"Zerg_Larva", BWAPI::UnitTypes::Enum::Zerg_Larva},
    {"Zerg_Egg", BWAPI::UnitTypes::Enum::Zerg_Egg},
    {"Zerg_Zergling", BWAPI::UnitTypes::Enum::Zerg_Zergling},
    {"Zerg_Hydralisk", BWAPI::UnitTypes::Enum::Zerg_Hydralisk},
    {"Zerg_Ultralisk", BWAPI::UnitTypes::Enum::Zerg_Ultralisk},
    {"Zerg_Broodling", BWAPI::UnitTypes::Enum::Zerg_Broodling},
    {"Zerg_Drone", BWAPI::UnitTypes::Enum::Zerg_Drone},
    {"Zerg_Overlord", BWAPI::UnitTypes::Enum::Zerg_Overlord},
    {"Zerg_Mutalisk", BWAPI::UnitTypes::Enum::Zerg_Mutalisk},
    {"Zerg_Guardian", BWAPI::UnitTypes::Enum::Zerg_Guardian},
    {"Zerg_Queen", BWAPI::UnitTypes::Enum::Zerg_Queen},
    {"Zerg_Defiler", BWAPI::UnitTypes::Enum::Zerg_Defiler},
    {"Zerg_Scourge", BWAPI::UnitTypes::Enum::Zerg_Scourge},
    {"Hero_Torrasque", BWAPI::UnitTypes::Enum::Hero_Torrasque},
    {"Hero_Matriarch", BWAPI::UnitTypes::Enum::Hero_Matriarch},
    {"Zerg_Infested_Terran", BWAPI::UnitTypes::Enum::Zerg_Infested_Terran},
    {"Hero_Infested_Kerrigan", BWAPI::UnitTypes::Enum::Hero_Infested_Kerrigan},
    {"Hero_Unclean_One", BWAPI::UnitTypes::Enum::Hero_Unclean_One},
    {"Hero_Hunter_Killer", BWAPI::UnitTypes::Enum::Hero_Hunter_Killer},
    {"Hero_Devouring_One", BWAPI::UnitTypes::Enum::Hero_Devouring_One},
    {"Hero_Kukulza_Mutalisk", BWAPI::UnitTypes::Enum::Hero_Kukulza_Mutalisk},
    {"Hero_Kukulza_Guardian", BWAPI::UnitTypes::Enum::Hero_Kukulza_Guardian},
    {"Hero_Yggdrasill", BWAPI::UnitTypes::Enum::Hero_Yggdrasill},
    {"Terran_Valkyrie", BWAPI::UnitTypes::Enum::Terran_Valkyrie},
    {"Zerg_Cocoon", BWAPI::UnitTypes::Enum::Zerg_Cocoon},
    {"Protoss_Corsair", BWAPI::UnitTypes::Enum::Protoss_Corsair},
    {"Protoss_Dark_Templar", BWAPI::UnitTypes::Enum::Protoss_Dark_Templar},
    {"Zerg_Devourer", BWAPI::UnitTypes::Enum::Zerg_Devourer},
    {"Protoss_Dark_Archon", BWAPI::UnitTypes::Enum::Protoss_Dark_Archon},
    {"Protoss_Probe", BWAPI::UnitTypes::Enum::Protoss_Probe},
    {"Protoss_Zealot", BWAPI::UnitTypes::Enum::Protoss_Zealot},
    {"Protoss_Dragoon", BWAPI::UnitTypes::Enum::Protoss_Dragoon},
    {"Protoss_High_Templar", BWAPI::UnitTypes::Enum::Protoss_High_Templar},
    {"Protoss_Archon", BWAPI::UnitTypes::Enum::Protoss_Archon},
    {"Protoss_Shuttle", BWAPI::UnitTypes::Enum::Protoss_Shuttle},
    {"Protoss_Scout", BWAPI::UnitTypes::Enum::Protoss_Scout},
    {"Protoss_Arbiter", BWAPI::UnitTypes::Enum::Protoss_Arbiter},
    {"Protoss_Carrier", BWAPI::UnitTypes::Enum::Protoss_Carrier},
    {"Protoss_Interceptor", BWAPI::UnitTypes::Enum::Protoss_Interceptor},
    {"Hero_Dark_Templar", BWAPI::UnitTypes::Enum::Hero_Dark_Templar},
    {"Hero_Zeratul", BWAPI::UnitTypes::Enum::Hero_Zeratul},
    {"Hero_Tassadar_Zeratul_Archon", BWAPI::UnitTypes::Enum::Hero_Tassadar_Zeratul_Archon},
    {"Hero_Fenix_Zealot", BWAPI::UnitTypes::Enum::Hero_Fenix_Zealot},
    {"Hero_Fenix_Dragoon", BWAPI::UnitTypes::Enum::Hero_Fenix_Dragoon},
    {"Hero_Tassadar", BWAPI::UnitTypes::Enum::Hero_Tassadar},
    {"Hero_Mojo", BWAPI::UnitTypes::Enum::Hero_Mojo},
    {"Hero_Warbringer", BWAPI::UnitTypes::Enum::Hero_Warbringer},
    {"Hero_Gantrithor", BWAPI::UnitTypes::Enum::Hero_Gantrithor},
    {"Protoss_Reaver", BWAPI::UnitTypes::Enum::Protoss_Reaver},
    {"Protoss_Observer", BWAPI::UnitTypes::Enum::Protoss_Observer},
    {"Protoss_Scarab", BWAPI::UnitTypes::Enum::Protoss_Scarab},
    {"Hero_Danimoth", BWAPI::UnitTypes::Enum::Hero_Danimoth},
    {"Hero_Aldaris", BWAPI::UnitTypes::Enum::Hero_Aldaris},
    {"Hero_Artanis", BWAPI::UnitTypes::Enum::Hero_Artanis},
    {"Critter_Rhynadon", BWAPI::UnitTypes::Enum::Critter_Rhynadon},
    {"Critter_Bengalaas", BWAPI::UnitTypes::Enum::Critter_Bengalaas},
    {"Special_Cargo_Ship", BWAPI::UnitTypes::Enum::Special_Cargo_Ship},
    {"Special_Mercenary_Gunship", BWAPI::UnitTypes::Enum::Special_Mercenary_Gunship},
    {"Critter_Scantid", BWAPI::UnitTypes::Enum::Critter_Scantid},
    {"Critter_Kakaru", BWAPI::UnitTypes::Enum::Critter_Kakaru},
    {"Critter_Ragnasaur", BWAPI::UnitTypes::Enum::Critter_Ragnasaur},
    {"Critter_Ursadon", BWAPI::UnitTypes::Enum::Critter_Ursadon},
    {"Zerg_Lurker_Egg", BWAPI::UnitTypes::Enum::Zerg_Lurker_Egg},
    {"Hero_Raszagal", BWAPI::UnitTypes::Enum::Hero_Raszagal},
    {"Hero_Samir_Duran", BWAPI::UnitTypes::Enum::Hero_Samir_Duran},
    {"Hero_Alexei_Stukov", BWAPI::UnitTypes::Enum::Hero_Alexei_Stukov},
    {"Special_Map_Revealer", BWAPI::UnitTypes::Enum::Special_Map_Revealer},
    {"Hero_Gerard_DuGalle", BWAPI::UnitTypes::Enum::Hero_Gerard_DuGalle},
    {"Zerg_Lurker", BWAPI::UnitTypes::Enum::Zerg_Lurker},
    {"Hero_Infested_Duran", BWAPI::UnitTypes::Enum::Hero_Infested_Duran},
    {"Spell_Disruption_Web", BWAPI::UnitTypes::Enum::Spell_Disruption_Web},
    {"Terran_Command_Center", BWAPI::UnitTypes::Enum::Terran_Command_Center},
    {"Terran_Comsat_Station", BWAPI::UnitTypes::Enum::Terran_Comsat_Station},
    {"Terran_Nuclear_Silo", BWAPI::UnitTypes::Enum::Terran_Nuclear_Silo},
    {"Terran_Supply_Depot", BWAPI::UnitTypes::Enum::Terran_Supply_Depot},
    {"Terran_Refinery", BWAPI::UnitTypes::Enum::Terran_Refinery},
    {"Terran_Barracks", BWAPI::UnitTypes::Enum::Terran_Barracks},
    {"Terran_Academy", BWAPI::UnitTypes::Enum::Terran_Academy},
    {"Terran_Factory", BWAPI::UnitTypes::Enum::Terran_Factory},
    {"Terran_Starport", BWAPI::UnitTypes::Enum::Terran_Starport},
    {"Terran_Control_Tower", BWAPI::UnitTypes::Enum::Terran_Control_Tower},
    {"Terran_Science_Facility", BWAPI::UnitTypes::Enum::Terran_Science_Facility},
    {"Terran_Covert_Ops", BWAPI::UnitTypes::Enum::Terran_Covert_Ops},
    {"Terran_Physics_Lab", BWAPI::UnitTypes::Enum::Terran_Physics_Lab},
    {"Unused_Terran1", BWAPI::UnitTypes::Enum::Unused_Terran1},
    {"Terran_Machine_Shop", BWAPI::UnitTypes::Enum::Terran_Machine_Shop},
    {"Unused_Terran2", BWAPI::UnitTypes::Enum::Unused_Terran2},
    {"Terran_Engineering_Bay", BWAPI::UnitTypes::Enum::Terran_Engineering_Bay},
    {"Terran_Armory", BWAPI::UnitTypes::Enum::Terran_Armory},
    {"Terran_Missile_Turret", BWAPI::UnitTypes::Enum::Terran_Missile_Turret},
    {"Terran_Bunker", BWAPI::UnitTypes::Enum::Terran_Bunker},
    {"Special_Crashed_Norad_II", BWAPI::UnitTypes::Enum::Special_Crashed_Norad_II},
    {"Special_Ion_Cannon", BWAPI::UnitTypes::Enum::Special_Ion_Cannon},
    {"Powerup_Uraj_Crystal", BWAPI::UnitTypes::Enum::Powerup_Uraj_Crystal},
    {"Powerup_Khalis_Crystal", BWAPI::UnitTypes::Enum::Powerup_Khalis_Crystal},
    {"Zerg_Infested_Command_Center", BWAPI::UnitTypes::Enum::Zerg_Infested_Command_Center},
    {"Zerg_Hatchery", BWAPI::UnitTypes::Enum::Zerg_Hatchery},
    {"Zerg_Lair", BWAPI::UnitTypes::Enum::Zerg_Lair},
    {"Zerg_Hive", BWAPI::UnitTypes::Enum::Zerg_Hive},
    {"Zerg_Nydus_Canal", BWAPI::UnitTypes::Enum::Zerg_Nydus_Canal},
    {"Zerg_Hydralisk_Den", BWAPI::UnitTypes::Enum::Zerg_Hydralisk_Den},
    {"Zerg_Defiler_Mound", BWAPI::UnitTypes::Enum::Zerg_Defiler_Mound},
    {"Zerg_Greater_Spire", BWAPI::UnitTypes::Enum::Zerg_Greater_Spire},
    {"Zerg_Queens_Nest", BWAPI::UnitTypes::Enum::Zerg_Queens_Nest},
    {"Zerg_Evolution_Chamber", BWAPI::UnitTypes::Enum::Zerg_Evolution_Chamber},
    {"Zerg_Ultralisk_Cavern", BWAPI::UnitTypes::Enum::Zerg_Ultralisk_Cavern},
    {"Zerg_Spire", BWAPI::UnitTypes::Enum::Zerg_Spire},
    {"Zerg_Spawning_Pool", BWAPI::UnitTypes::Enum::Zerg_Spawning_Pool},
    {"Zerg_Creep_Colony", BWAPI::UnitTypes::Enum::Zerg_Creep_Colony},
    {"Zerg_Spore_Colony", BWAPI::UnitTypes::Enum::Zerg_Spore_Colony},
    {"Unused_Zerg1", BWAPI::UnitTypes::Enum::Unused_Zerg1},
    {"Zerg_Sunken_Colony", BWAPI::UnitTypes::Enum::Zerg_Sunken_Colony},
    {"Special_Overmind_With_Shell", BWAPI::UnitTypes::Enum::Special_Overmind_With_Shell},
    {"Special_Overmind", BWAPI::UnitTypes::Enum::Special_Overmind},
    {"Zerg_Extractor", BWAPI::UnitTypes::Enum::Zerg_Extractor},
    {"Special_Mature_Chrysalis", BWAPI::UnitTypes::Enum::Special_Mature_Chrysalis},
    {"Special_Cerebrate", BWAPI::UnitTypes::Enum::Special_Cerebrate},
    {"Special_Cerebrate_Daggoth", BWAPI::UnitTypes::Enum::Special_Cerebrate_Daggoth},
    {"Unused_Zerg2", BWAPI::UnitTypes::Enum::Unused_Zerg2},
    {"Protoss_Nexus", BWAPI::UnitTypes::Enum::Protoss_Nexus},
    {"Protoss_Robotics_Facility", BWAPI::UnitTypes::Enum::Protoss_Robotics_Facility},
    {"Protoss_Pylon", BWAPI::UnitTypes::Enum::Protoss_Pylon},
    {"Protoss_Assimilator", BWAPI::UnitTypes::Enum::Protoss_Assimilator},
    {"Unused_Protoss1", BWAPI::UnitTypes::Enum::Unused_Protoss1},
    {"Protoss_Observatory", BWAPI::UnitTypes::Enum::Protoss_Observatory},
    {"Protoss_Gateway", BWAPI::UnitTypes::Enum::Protoss_Gateway},
    {"Unused_Protoss2", BWAPI::UnitTypes::Enum::Unused_Protoss2},
    {"Protoss_Photon_Cannon", BWAPI::UnitTypes::Enum::Protoss_Photon_Cannon},
    {"Protoss_Citadel_of_Adun", BWAPI::UnitTypes::Enum::Protoss_Citadel_of_Adun},
    {"Protoss_Cybernetics_Core", BWAPI::UnitTypes::Enum::Protoss_Cybernetics_Core},
    {"Protoss_Templar_Archives", BWAPI::UnitTypes::Enum::Protoss_Templar_Archives},
    {"Protoss_Forge", BWAPI::UnitTypes::Enum::Protoss_Forge},
    {"Protoss_Stargate", BWAPI::UnitTypes::Enum::Protoss_Stargate},
    {"Special_Stasis_Cell_Prison", BWAPI::UnitTypes::Enum::Special_Stasis_Cell_Prison},
    {"Protoss_Fleet_Beacon", BWAPI::UnitTypes::Enum::Protoss_Fleet_Beacon},
    {"Protoss_Arbiter_Tribunal", BWAPI::UnitTypes::Enum::Protoss_Arbiter_Tribunal},
    {"Protoss_Robotics_Support_Bay", BWAPI::UnitTypes::Enum::Protoss_Robotics_Support_Bay},
    {"Protoss_Shield_Battery", BWAPI::UnitTypes::Enum::Protoss_Shield_Battery},
    {"Special_Khaydarin_Crystal_Form", BWAPI::UnitTypes::Enum::Special_Khaydarin_Crystal_Form},
    {"Special_Protoss_Temple", BWAPI::UnitTypes::Enum::Special_Protoss_Temple},
    {"Special_XelNaga_Temple", BWAPI::UnitTypes::Enum::Special_XelNaga_Temple},
    {"Resource_Mineral_Field", BWAPI::UnitTypes::Enum::Resource_Mineral_Field},
    {"Resource_Mineral_Field_Type_2", BWAPI::UnitTypes::Enum::Resource_Mineral_Field_Type_2},
    {"Resource_Mineral_Field_Type_3", BWAPI::UnitTypes::Enum::Resource_Mineral_Field_Type_3},
    {"Unused_Cave", BWAPI::UnitTypes::Enum::Unused_Cave},
    {"Unused_Cave_In", BWAPI::UnitTypes::Enum::Unused_Cave_In},
    {"Unused_Cantina", BWAPI::UnitTypes::Enum::Unused_Cantina},
    {"Unused_Mining_Platform", BWAPI::UnitTypes::Enum::Unused_Mining_Platform},
    {"Unused_Independant_Command_Center", BWAPI::UnitTypes::Enum::Unused_Independant_Command_Center},
    {"Special_Independant_Starport", BWAPI::UnitTypes::Enum::Special_Independant_Starport},
    {"Unused_Independant_Jump_Gate", BWAPI::UnitTypes::Enum::Unused_Independant_Jump_Gate},
    {"Unused_Ruins", BWAPI::UnitTypes::Enum::Unused_Ruins},
    {"Unused_Khaydarin_Crystal_Formation", BWAPI::UnitTypes::Enum::Unused_Khaydarin_Crystal_Formation},
    {"Resource_Vespene_Geyser", BWAPI::UnitTypes::Enum::Resource_Vespene_Geyser},
    {"Special_Warp_Gate", BWAPI::UnitTypes::Enum::Special_Warp_Gate},
    {"Special_Psi_Disrupter", BWAPI::UnitTypes::Enum::Special_Psi_Disrupter},
    {"Unused_Zerg_Marker", BWAPI::UnitTypes::Enum::Unused_Zerg_Marker},
    {"Unused_Terran_Marker", BWAPI::UnitTypes::Enum::Unused_Terran_Marker},
    {"Unused_Protoss_Marker", BWAPI::UnitTypes::Enum::Unused_Protoss_Marker},
    {"Special_Zerg_Beacon", BWAPI::UnitTypes::Enum::Special_Zerg_Beacon},
    {"Special_Terran_Beacon", BWAPI::UnitTypes::Enum::Special_Terran_Beacon},
    {"Special_Protoss_Beacon", BWAPI::UnitTypes::Enum::Special_Protoss_Beacon},
    {"Special_Zerg_Flag_Beacon", BWAPI::UnitTypes::Enum::Special_Zerg_Flag_Beacon},
    {"Special_Terran_Flag_Beacon", BWAPI::UnitTypes::Enum::Special_Terran_Flag_Beacon},
    {"Special_Protoss_Flag_Beacon", BWAPI::UnitTypes::Enum::Special_Protoss_Flag_Beacon},
    {"Special_Power_Generator", BWAPI::UnitTypes::Enum::Special_Power_Generator},
    {"Special_Overmind_Cocoon", BWAPI::UnitTypes::Enum::Special_Overmind_Cocoon},
    {"Spell_Dark_Swarm", BWAPI::UnitTypes::Enum::Spell_Dark_Swarm},
    {"Special_Floor_Missile_Trap", BWAPI::UnitTypes::Enum::Special_Floor_Missile_Trap},
    {"Special_Floor_Hatch", BWAPI::UnitTypes::Enum::Special_Floor_Hatch},
    {"Special_Upper_Level_Door", BWAPI::UnitTypes::Enum::Special_Upper_Level_Door},
    {"Special_Right_Upper_Level_Door", BWAPI::UnitTypes::Enum::Special_Right_Upper_Level_Door},
    {"Special_Pit_Door", BWAPI::UnitTypes::Enum::Special_Pit_Door},
    {"Special_Right_Pit_Door", BWAPI::UnitTypes::Enum::Special_Right_Pit_Door},
    {"Special_Floor_Gun_Trap", BWAPI::UnitTypes::Enum::Special_Floor_Gun_Trap},
    {"Special_Wall_Missile_Trap", BWAPI::UnitTypes::Enum::Special_Wall_Missile_Trap},
    {"Special_Wall_Flame_Trap", BWAPI::UnitTypes::Enum::Special_Wall_Flame_Trap},
    {"Special_Right_Wall_Missile_Trap", BWAPI::UnitTypes::Enum::Special_Right_Wall_Missile_Trap},
    {"Special_Right_Wall_Flame_Trap", BWAPI::UnitTypes::Enum::Special_Right_Wall_Flame_Trap},
    {"Special_Start_Location", BWAPI::UnitTypes::Enum::Special_Start_Location},
    {"Powerup_Flag", BWAPI::UnitTypes::Enum::Powerup_Flag},
    {"Powerup_Young_Chrysalis", BWAPI::UnitTypes::Enum::Powerup_Young_Chrysalis},
    {"Powerup_Psi_Emitter", BWAPI::UnitTypes::Enum::Powerup_Psi_Emitter},
    {"Powerup_Data_Disk", BWAPI::UnitTypes::Enum::Powerup_Data_Disk},
    {"Powerup_Khaydarin_Crystal", BWAPI::UnitTypes::Enum::Powerup_Khaydarin_Crystal},
    {"Powerup_Mineral_Cluster_Type_1", BWAPI::UnitTypes::Enum::Powerup_Mineral_Cluster_Type_1},
    {"Powerup_Mineral_Cluster_Type_2", BWAPI::UnitTypes::Enum::Powerup_Mineral_Cluster_Type_2},
    {"Powerup_Protoss_Gas_Orb_Type_1", BWAPI::UnitTypes::Enum::Powerup_Protoss_Gas_Orb_Type_1},
    {"Powerup_Protoss_Gas_Orb_Type_2", BWAPI::UnitTypes::Enum::Powerup_Protoss_Gas_Orb_Type_2},
    {"Powerup_Zerg_Gas_Sac_Type_1", BWAPI::UnitTypes::Enum::Powerup_Zerg_Gas_Sac_Type_1},
    {"Powerup_Zerg_Gas_Sac_Type_2", BWAPI::UnitTypes::Enum::Powerup_Zerg_Gas_Sac_Type_2},
    {"Powerup_Terran_Gas_Tank_Type_1", BWAPI::UnitTypes::Enum::Powerup_Terran_Gas_Tank_Type_1},
    {"Powerup_Terran_Gas_Tank_Type_2", BWAPI::UnitTypes::Enum::Powerup_Terran_Gas_Tank_Type_2},
};

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset &units)
{
    BWAPI::Unit closestUnit = nullptr;

    for (auto &u : units)
    {
        if (!closestUnit || u->getDistance(p) < closestUnit->getDistance(p))
        {
            closestUnit = u;
        }
    }

    return closestUnit;
}

BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset &units)
{
    if (!unit)
    {
        return nullptr;
    }

    return GetClosestUnitTo(unit->getPosition(), units);
}

int Tools::CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset &units)
{
    int sum = 0;

    for (auto &unit : units)
    {
        if (unit->getType() == type)
        {
            sum++;
        }
    }

    return sum;
}

BWAPI::Unit Tools::GetUnitOfType(BWAPI::UnitType type)
{
    // For each unit that we own
    for (auto &unit : BWAPI::Broodwar->self()->getUnits())
    {
        // if the unit is of the correct type, and it actually has been constructed, return it
        if (unit->getType() == type && unit->isCompleted() && !unit->isConstructing())
        {
            return unit;
        }
    }

    // If we didn't find a valid unit to return, make sure we return nullptr
    return nullptr;
}

BWAPI::Unit Tools::GetDepot()
{
    const BWAPI::UnitType depot = BWAPI::Broodwar->self()->getRace().getResourceDepot();

    return GetUnitOfType(depot);
}

// Attempt tp construct a building of a given type
int Tools::BuildBuilding(BWAPI::UnitType type)
{
    int  builderID = -1;
    bool bRes      = false;

    // Get the type of unit that is required to build the desired building
    BWAPI::UnitType builderType = type.whatBuilds().first;

    // Get a unit that we own that is of the given type so it can build
    // If we can't find a valid builder unit, then we have to cancel the building
    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);

    if (!builder)
    {
        return builderID;
    }

    builderID = builder->getID();

    // Get a location that we want to build the building next to
    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;

    if (builderType == HY_ZERG_LARVA)
    {
        bRes = builder->build(type);
    }
    else
    {
        bool                buildingOnCreep = type.requiresCreep();
        BWAPI::TilePosition buildPos        = BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);

        bRes = builder->build(type, buildPos);
    }

    if (!bRes)
    {
        BWAPI::Error err  = BWAPI::Broodwar->getLastError();
        const char   *str = err.toString().c_str();

        BWAPI::Broodwar->printf("Err: %s\n", str);
        TR_LOG(TR_DEBUG, TR_BUILDING, "Err: %s\n", str);

        return -1;
    }

    return builderID;
}

void Tools::DrawUnitCommands()
{
    for (auto &unit : BWAPI::Broodwar->self()->getUnits())
    {
        const BWAPI::UnitCommand &command = unit->getLastCommand();

        // If the previous command had a ground position target, draw it in red
        // Example: move to location on the map
        if (command.getTargetPosition() != BWAPI::Positions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTargetPosition(), BWAPI::Colors::Red);
        }

        // If the previous command had a tile position target, draw it in red
        // Example: build at given tile position location
        if (command.getTargetTilePosition() != BWAPI::TilePositions::None)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), BWAPI::Position(command.getTargetTilePosition()), BWAPI::Colors::Green);
        }

        // If the previous command had a unit target, draw it in red
        // Example: attack unit, mine mineral, etc
        if (command.getTarget() != nullptr)
        {
            BWAPI::Broodwar->drawLineMap(unit->getPosition(), command.getTarget()->getPosition(), BWAPI::Colors::White);
        }

        BWAPI::Position pos(unit->getPosition());
        BWAPI::Broodwar->drawText(BWAPI::CoordinateType::Enum::Map, pos.x, pos.y, "%d", unit->getID());
    }
}

void Tools::DrawUnitBoundingBoxes()
{
    for (auto &unit : BWAPI::Broodwar->getAllUnits())
    {
        BWAPI::Position topLeft(unit->getLeft(), unit->getTop());
        BWAPI::Position bottomRight(unit->getRight(), unit->getBottom());
        BWAPI::Broodwar->drawBoxMap(topLeft, bottomRight, BWAPI::Colors::White);
    }
}

void Tools::SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target)
{
    // if there's no valid unit, ignore the command
    if (!unit || !target)
    {
        return;
    }

    // Don't issue a 2nd command to the unit on the same frame
    if (unit->getLastCommandFrame() >= BWAPI::Broodwar->getFrameCount())
    {
        return;
    }

    // If we are issuing the same type of command with the same arguments, we can ignore it
    // Issuing multiple identical commands on successive frames can lead to bugs
    if (unit->getLastCommand().getTarget() == target)
    {
        return;
    }

    // If there's nothing left to stop us, right click!
    unit->rightClick(target);
}

int Tools::GetTotalSupply(bool inProgress)
{
    // start the calculation by looking at our current completed supply
    int totalSupply = BWAPI::Broodwar->self()->supplyTotal();

    // if we don't want to calculate the supply in progress, just return that value
    if (!inProgress)
    {
        return totalSupply;
    }

    // if we do care about supply in progress, check all the currently constructing units if they will add supply
    for (auto &unit : BWAPI::Broodwar->self()->getUnits())
    {
        // ignore units that are fully completed
        if (unit->isCompleted())
        {
            continue;
        }

        // Just skip zerg hatchery
        if (unit->getType() == HY_ZERG_HATCHERY)
            continue;

        int supplyProvided = unit->getType().supplyProvided();
        if (supplyProvided)
        {
            // if they are not completed, then add their supply provided to the total supply
            totalSupply += unit->getType().supplyProvided();
        }
        else
        {
            // get the last command given to the unit
            const BWAPI::UnitCommand &command = unit->getLastCommand();

            // if it's not a build command we can ignore it
            if (command.getType() != BWAPI::UnitCommandTypes::Build)
            {
                continue;
            }

            supplyProvided = command.getUnitType().supplyProvided();

            if (supplyProvided > 0)
            {
                // add the supply amount of the unit that it's trying to build
                totalSupply += supplyProvided;
            }
        }
    }

#if 0 // There is replicated caculation of unit which always provides supply and its command provides supply too.
    // one last tricky case: if a unit is currently on its way to build a supply provider, add it
    for (auto &unit : BWAPI::Broodwar->self()->getUnits())
    {
        // get the last command given to the unit
        const BWAPI::UnitCommand &command = unit->getLastCommand();

        // if it's not a build command we can ignore it
        if (command.getType() != BWAPI::UnitCommandTypes::Build)
        {
            continue;
        }

        // Just skip zerg hatchery
        if (unit->getType() == HY_ZERG_HATCHERY)
            continue;

        // add the supply amount of the unit that it's trying to build
        totalSupply += command.getUnitType().supplyProvided();
    }
#endif

    return totalSupply;
}

void Tools::DrawUnitHealthBars()
{
    // how far up from the unit to draw the health bar
    int verticalOffset = -10;

    // draw a health bar for each unit on the map
    for (auto &unit : BWAPI::Broodwar->getAllUnits())
    {
        // determine the position and dimensions of the unit
        const BWAPI::Position &pos   = unit->getPosition();
        int                   left   = pos.x - unit->getType().dimensionLeft();
        int                   right  = pos.x + unit->getType().dimensionRight();
        int                   top    = pos.y - unit->getType().dimensionUp();
        int                   bottom = pos.y + unit->getType().dimensionDown();

        // if it's a resource, draw the resources remaining
        if (unit->getType().isResourceContainer() && unit->getInitialResources() > 0)
        {
            double mineralRatio = (double)unit->getResources() / (double)unit->getInitialResources();
            DrawHealthBar(unit, mineralRatio, BWAPI::Colors::Cyan, 0);
        }
        // otherwise if it's a unit, draw the hp
        else if (unit->getType().maxHitPoints() > 0)
        {
            double       hpRatio = (double)unit->getHitPoints() / (double)unit->getType().maxHitPoints();
            BWAPI::Color hpColor = BWAPI::Colors::Green;
            if (hpRatio < 0.66)
                hpColor = BWAPI::Colors::Orange;

            if (hpRatio < 0.33)
                hpColor = BWAPI::Colors::Red;

            DrawHealthBar(unit, hpRatio, hpColor, 0);

            // if it has shields, draw those too
            if (unit->getType().maxShields() > 0)
            {
                double shieldRatio = (double)unit->getShields() / (double)unit->getType().maxShields();
                DrawHealthBar(unit, shieldRatio, BWAPI::Colors::Blue, -3);
            }
        }
    }
}

void Tools::DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset)
{
    int                   verticalOffset = -10;
    const BWAPI::Position &pos           = unit->getPosition();

    int left   = pos.x - unit->getType().dimensionLeft();
    int right  = pos.x + unit->getType().dimensionRight();
    int top    = pos.y - unit->getType().dimensionUp();
    int bottom = pos.y + unit->getType().dimensionDown();

    int ratioRight = left + (int)((right - left) * ratio);
    int hpTop      = top + yOffset + verticalOffset;
    int hpBottom   = top + 4 + yOffset + verticalOffset;

    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Grey, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(ratioRight, hpBottom), color, true);
    BWAPI::Broodwar->drawBoxMap(BWAPI::Position(left, hpTop), BWAPI::Position(right, hpBottom), BWAPI::Colors::Black, false);

    int ticWidth = 3;

    for (int i(left); i < right - 1; i += ticWidth)
    {
        BWAPI::Broodwar->drawLineMap(BWAPI::Position(i, hpTop), BWAPI::Position(i, hpBottom), BWAPI::Colors::Black);
    }
}

// X, Y is in tile size. Each tile is 32x32.
MapDirection Tools::GetDirection(int X, int Y)
{
    MapDirection dir;
    const int    mapWidth  = BWAPI::Broodwar->mapWidth();
    const int    mapHeight = BWAPI::Broodwar->mapHeight();

    // Check we are in what direction of this map.
    const int center1X = mapWidth / 3;
    const int center1Y = mapHeight / 3;
    const int center2X = (mapWidth / 3) * 2;
    const int center2Y = (mapHeight / 3) * 2;
    const int myX      = X;
    const int myY      = Y;

    if (myX <= center1X && myY <= center1Y)
    {
        dir = HY_TOP_LEFT;
    }
    else if (myX >= center2X && myY >= center2Y)
    {
        dir = HY_BOTTOM_RIGHT;
    }
    else if (myX > center1X && myY <= center1Y && myX <= center2X)
    {
        dir = HY_TOP;
    }
    else if (myX > center2X && myY <= center1Y)
    {
        dir = HY_TOP_RIGHT;
    }
    else if (myX > center2X && myY > center1Y && myY <= center2Y)
    {
        dir = HY_RIGHT;
    }
    else if (myX > center1X && myX <= center2X && myY <= center2Y)
    {
        dir = HY_BOTTOM;
    }
    else if (myX <= center1X && myY > center2Y)
    {
        dir = HY_BOTTOM_LEFT;
    }
    else if (myX <= center1X && myY > center1Y && myY <= center2Y)
    {
        dir = HY_LEFT;
    }
    else
    {
        dir = HY_CENTER;
    }

    return dir;
}