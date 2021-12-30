#include "peds.h"

void SetPedOnMount(Ped ped, Ped mount, int seat)
{
	/** __SET_PED_ON_MOUNT */
	invoke<Void>(0x028F76B6E78246EB, ped, mount, seat, true);
}

Ped SpawnPedAroundPlayer(Hash skinModel, bool bSetInVehicle)
{
	LoadModel(skinModel);

	/** IS_MODEL_A_HORSE */
	bool bModelIsHorse = invoke<bool>(0x772A1969F649E902, skinModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerLocation = ENTITY::GET_ENTITY_COORDS(playerPed, true, 0);

	bool bPlayerInVehicle = PED::IS_PED_IN_ANY_VEHICLE(playerPed, true);
	
	if (bPlayerInVehicle)
	{
		playerLocation.z += 2.0f;
	}

	Ped ped = PED::CREATE_PED(skinModel, playerLocation.x, playerLocation.y, playerLocation.z, 0.0f, 1, 0, 0, 0);
	PED::SET_PED_VISIBLE(ped, true);
	PED::SET_PED_HEARING_RANGE(ped, 10000.0f);
	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(skinModel);
	

	if (bSetInVehicle && !bModelIsHorse)
	{
		if (bPlayerInVehicle)
		{
			Vehicle playerVehicle = PED::GET_VEHICLE_PED_IS_IN(playerPed, false);

			PED::SET_PED_INTO_VEHICLE(ped, playerVehicle, -2);
		}
		else if (PED::IS_PED_ON_MOUNT(playerPed))
		{
			Ped mount = PED::GET_MOUNT(playerPed);
			bool bIsMountSeatFree = invoke<bool>(0xAAB0FE202E9FC9F0, mount, 0);

			if (bIsMountSeatFree)
			{
				SetPedOnMount(ped, mount, 0);
			}
		}
	}

	return ped;
}

void MarkPedAsCompanion(Hash ped)
{
	Hash playerGroup = GAMEPLAY::GET_HASH_KEY((char*)"PLAYER");
	Hash companionGroup;

	PED::ADD_RELATIONSHIP_GROUP((char*)"_CHAOS_COMPANION", &companionGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, companionGroup, playerGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(0, playerGroup, companionGroup);

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, companionGroup);

	PED::SET_PED_AS_GROUP_MEMBER(ped, PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID()));

	/** BF_CanFightArmedPedsWhenNotArmed */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	/** BF_AlwaysFight */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
}

void MarkPedAsEnemy(Hash ped)
{
	Hash playerGroup = GAMEPLAY::GET_HASH_KEY((char*)"PLAYER");
	Hash enemyGroup;

	PED::ADD_RELATIONSHIP_GROUP((char*)"_CHAOS_ENEMY", &enemyGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, enemyGroup, playerGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(5, playerGroup, enemyGroup);

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, enemyGroup);

	/** BF_CanFightArmedPedsWhenNotArmed */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 5, true);
	/** BF_AlwaysFight */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 46, true);
	/** BF_CanUseVehicles */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 1, true);
	/** BF_CanLeaveVehicle */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 3, true);

	AI::TASK_COMBAT_PED(ped, PLAYER::PLAYER_PED_ID(), 0, 16);
}

void EffectSpawnSoldier::OnActivate() 
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"s_m_m_army_01");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	MarkPedAsCompanion(ped);

	RemoveAllPedWeapons(ped);

	static Hash weaponHash = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_SNIPERRIFLE_ROLLINGBLOCK");
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, weaponHash, 9999, true, 0x2cd419dc);
	WEAPON::SET_CURRENT_PED_WEAPON(ped, weaponHash, true, 0, 0, 0);
}

void EffectSpawnDrunkardJon::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_GrizzledJon");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 1000);
	ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0);

	MarkPedAsEnemy(ped);
}

void EffectSpawnLenny::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_lenny");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	MarkPedAsCompanion(ped);

	RemoveAllPedWeapons(ped);

	static Hash weaponHash = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_RIFLE_SPRINGFIELD");
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, weaponHash, 9999, true, 0x2cd419dc);
	WEAPON::SET_CURRENT_PED_WEAPON(ped, weaponHash, true, 0, 0, 0);
}

void EffectSpawnChicken::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Chicken_01");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	MarkPedAsCompanion(ped);
}

void EffectKidnapping::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_AberdeenPigFarmer");
	static Hash skinModel2 = GAMEPLAY::GET_HASH_KEY((char*)"CS_AberdeenSister");

	Ped ped = SpawnPedAroundPlayer(skinModel);
	Ped sister = SpawnPedAroundPlayer(skinModel2);

	Hash playerGroup = GAMEPLAY::GET_HASH_KEY((char*)"PLAYER");
	Hash relationshipGroup;

	PED::ADD_RELATIONSHIP_GROUP((char*)"_CHAOS_KIDNAPPERS", &relationshipGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, relationshipGroup, playerGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, playerGroup, relationshipGroup);

	auto playerGroupID = PLAYER::GET_PLAYER_GROUP(PLAYER::PLAYER_ID());

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, relationshipGroup);
	PED::SET_PED_RELATIONSHIP_GROUP_HASH(sister, relationshipGroup);

	/** BF_CanLeaveVehicle */
	PED::SET_PED_COMBAT_ATTRIBUTES(ped, 3, false);
	/** BF_CanLeaveVehicle */
	PED::SET_PED_COMBAT_ATTRIBUTES(sister, 3, false);

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerCoord = ENTITY::GET_ENTITY_COORDS(playerPed, true, false);

	Entity vehicle = 0;

	static Hash wagonModel = GAMEPLAY::GET_HASH_KEY((char*)"WAGONPRISON01X");
	LoadModel(wagonModel);

	vehicle = VEHICLE::CREATE_VEHICLE(wagonModel, playerCoord.x, playerCoord.y, playerCoord.z, rand() % 360, 0, 0, 0, 0);
	PED::SET_PED_INTO_VEHICLE(ped, vehicle, -1);
	PED::SET_PED_INTO_VEHICLE(sister, vehicle, 0);
	PED::SET_PED_INTO_VEHICLE(playerPed, vehicle, 1);

	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(wagonModel);
	
	if (vehicle)
	{
		AI::TASK_VEHICLE_DRIVE_WANDER(ped, vehicle, 100000.0f, 262144);
		PED::SET_PED_KEEP_TASK(ped, true);
	}
}

void EffectSpawnHorse::OnActivate()
{
	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Horse_Arabian_White");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	SetPedOnMount(playerPed, horse, -1);
}


std::vector<Ped> GetNearbyPeds(int32_t Max)
{
	std::vector<Ped> pedsOut;
	Ped playerPed = PLAYER::PLAYER_PED_ID();

	NearbyEntities peds;
	peds.size = 100;

	if (Max > 100)
	{
		Max = 100;
	}

	int pedsFound = PED::GET_PED_NEARBY_PEDS(playerPed, (int*)&peds, -1, 0);

	if (Max > pedsFound)
	{
		Max = pedsFound;
	}

	for (int32_t i = 0; i < Max; i++)
	{
		Entity ped = peds.entities[i];
		if (ENTITY::DOES_ENTITY_EXIST(ped) && ped != playerPed)
		{
			pedsOut.push_back(ped);
		}
	}

	return pedsOut;
}

void RemoveAllPedWeapons(Ped ped)
{
	static Hash unarmed = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_UNARMED");
	WEAPON::SET_CURRENT_PED_WEAPON(ped, unarmed, 1, 0, 0, 0);

	for (Hash weaponHash : Effect::WeaponHashes)
	{
		WEAPON::REMOVE_WEAPON_FROM_PED(ped, weaponHash, 0, 0);
	}
}

void EffectSpawnMule::OnActivate()
{
	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_HorseMule_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	SetPedOnMount(playerPed, horse, -1);
}

void EffectSpawnDonkey::OnActivate()
{
	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Donkey_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	SetPedOnMount(playerPed, horse, -1);
}

void EffectSpawnSerialKiller::OnActivate()
{
	/** Spawn donkey */

	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Donkey_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	/** Spawn killer */
	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"G_M_M_UniDuster_03");

	Ped ped = SpawnPedAroundPlayer(pedModel);

	SetPedOnMount(ped, horse, -1);

	MarkPedAsEnemy(ped);
}

void EffectSpawnVampire::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_Vampire");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 1000);
	ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0);


	MarkPedAsEnemy(ped);
}

void EffectSpawnGiantDonkey::OnActivate()
{
	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Donkey_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	/** _SET_PED_SCALE */
	invoke<Void>(0x25ACFC650B65C538, horse, 1.5f);

	SetPedOnMount(playerPed, horse, -1);
}

void EffectSpawnMiniDonkey::OnActivate()
{
	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Donkey_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	/** _SET_PED_SCALE */
	invoke<Void>(0x25ACFC650B65C538, horse, 0.42f);

	SetPedOnMount(playerPed, horse, -1);
}

void EffectSpawnGiantCop::OnActivate()
{
	Effect::OnActivate();

	static Hash copGroup = GAMEPLAY::GET_HASH_KEY((char*)"COP");
	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"S_M_M_DispatchLeaderPolice_01");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	/** _SET_PED_SCALE */
	invoke<Void>(0x25ACFC650B65C538, ped, 10.0f);

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(ped, copGroup);

	AI::TASK_LOOK_AT_ENTITY(ped, PLAYER::PLAYER_PED_ID(), -1, 2048, 3, 1);
}

void EffectSpawnAngrySkeleton::OnActivate()
{
	Effect::OnActivate();

	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"U_M_M_CircusWagon_01");
	static Hash pedModel2 = GAMEPLAY::GET_HASH_KEY((char*)"CS_ODPROSTITUTE");

	Ped ped = SpawnPedAroundPlayer(rand() % 2 ? pedModel : pedModel2);

	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 500);
	ENTITY::SET_ENTITY_HEALTH(ped, 500, 0);

	MarkPedAsEnemy(ped);
}

void EffectSpawnAngryDwarf::OnActivate()
{
	Effect::OnActivate();

	/** Spawn donkey */

	Effect::OnActivate();

	static Hash horseModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Donkey_01");

	Ped horse = SpawnPedAroundPlayer(horseModel);

	/** _SET_PED_SCALE */
	invoke<Void>(0x25ACFC650B65C538, horse, 0.5f);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	/** Spawn dwarf */
	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_Magnifico");

	Ped ped = SpawnPedAroundPlayer(pedModel, false);

	static Hash weaponHash = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_MELEE_KNIFE");
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, weaponHash, 9999, true, 0x2cd419dc);
	WEAPON::SET_CURRENT_PED_WEAPON(ped, weaponHash, true, 0, 0, 0);

	SetPedOnMount(ped, horse, -1);

	MarkPedAsEnemy(ped);
}

void EffectSpawnCompanionBertram::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_ODDFELLOWSPINHEAD");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	/** _SET_PED_SCALE */
	invoke<Void>(0x25ACFC650B65C538, ped, 1.25f);

	MarkPedAsCompanion(ped);

	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 1000);
	ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0);

	WEAPON::REMOVE_ALL_PED_WEAPONS(ped, true, 0);
}

void EffectSpawnFrozenCouple::OnActivate()
{
	Effect::OnActivate();

	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"RE_FROZENTODEATH_FEMALES_01");
	static Hash pedModel2 = GAMEPLAY::GET_HASH_KEY((char*)"RE_FROZENTODEATH_MALES_01");

	Ped ped = SpawnPedAroundPlayer(pedModel);
	Ped ped2 = SpawnPedAroundPlayer(pedModel2);

	MarkPedAsEnemy(ped);
	MarkPedAsEnemy(ped2);
}

void EffectSpawnRobot::OnActivate()
{
	Effect::OnActivate();

	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_crackpotRobot");

	Ped ped = SpawnPedAroundPlayer(pedModel);

	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 10000);
	ENTITY::SET_ENTITY_HEALTH(ped, 10000, 0);
	
	MarkPedAsCompanion(ped);
}

void EffectSpawnLassoGuy::OnActivate()
{
	Effect::OnActivate();

	static Hash pedModel = GAMEPLAY::GET_HASH_KEY((char*)"CS_EXOTICCOLLECTOR");

	Ped ped = SpawnPedAroundPlayer(pedModel);

	MarkPedAsEnemy(ped);

	static Hash weaponHash = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_LASSO");
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(ped, weaponHash, 100, 1, 0x2cd419dc);
	WEAPON::SET_CURRENT_PED_WEAPON(ped, weaponHash, 1, 0, 0, 0);

	/** TASK_LASSO_PED */
	invoke<Void>(0xC716EB2BD16370A3, ped, PLAYER::PLAYER_PED_ID());
}

void EffectSkyrimIntro::OnActivate()
{
	Effect::OnActivate();

	GRAPHICS::ANIMPOSTFX_PLAY((char*)"PlayerWakeUpInterrogation");

	Ped playerPed = PLAYER::PLAYER_PED_ID();
	Vector3 playerCoord = ENTITY::GET_ENTITY_COORDS(playerPed, true, 0);

	static Hash copModel = GAMEPLAY::GET_HASH_KEY((char*)"U_M_O_BlWPoliceChief_01");
	static Hash wagonModel = GAMEPLAY::GET_HASH_KEY((char*)"wagon03x");
	static Hash prisonerModel1 = GAMEPLAY::GET_HASH_KEY((char*)"RE_PRISONWAGON_MALES_01");
	static Hash prisonerModel2 = GAMEPLAY::GET_HASH_KEY((char*)"RE_LONEPRISONER_MALES_01");
	static Hash prisonerModel3 = GAMEPLAY::GET_HASH_KEY((char*)"CS_chainprisoner_01");

	Ped cop = SpawnPedAroundPlayer(copModel, false);
	Ped prisoner1 = SpawnPedAroundPlayer(prisonerModel1, false);
	Ped prisoner2 = SpawnPedAroundPlayer(prisonerModel2, false);
	Ped prisoner3 = SpawnPedAroundPlayer(prisonerModel3, false);

	Hash playerGroup = GAMEPLAY::GET_HASH_KEY((char*)"PLAYER");
	Hash relationshipGroup;

	PED::ADD_RELATIONSHIP_GROUP((char*)"_CHAOS_WAGON", &relationshipGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, relationshipGroup, playerGroup);
	PED::SET_RELATIONSHIP_BETWEEN_GROUPS(3, playerGroup, relationshipGroup);

	PED::SET_PED_RELATIONSHIP_GROUP_HASH(cop, relationshipGroup);
	PED::SET_PED_RELATIONSHIP_GROUP_HASH(prisoner1, relationshipGroup);
	PED::SET_PED_RELATIONSHIP_GROUP_HASH(prisoner2, relationshipGroup);
	PED::SET_PED_RELATIONSHIP_GROUP_HASH(prisoner3, relationshipGroup);

	/** BF_CanLeaveVehicle */
	PED::SET_PED_COMBAT_ATTRIBUTES(cop, 3, false);
	PED::SET_PED_COMBAT_ATTRIBUTES(prisoner1, 3, false);
	PED::SET_PED_COMBAT_ATTRIBUTES(prisoner2, 3, false);
	PED::SET_PED_COMBAT_ATTRIBUTES(prisoner3, 3, false);

	static Hash weaponHash = GAMEPLAY::GET_HASH_KEY((char*)"WEAPON_REPEATER_WINCHESTER");
	WEAPON::GIVE_DELAYED_WEAPON_TO_PED(cop, weaponHash, 100, true, 0x2cd419dc);
	WEAPON::SET_PED_AMMO(playerPed, weaponHash, 100);

	LoadModel(wagonModel);

	Vehicle vehicle = VEHICLE::CREATE_VEHICLE(wagonModel, playerCoord.x, playerCoord.y, playerCoord.z, rand() % 360, 0, 0, 0, 0);
	PED::SET_PED_INTO_VEHICLE(cop, vehicle, -1);
	PED::SET_PED_INTO_VEHICLE(playerPed, vehicle, 1);
	PED::SET_PED_INTO_VEHICLE(prisoner1, vehicle, 2);
	PED::SET_PED_INTO_VEHICLE(prisoner2, vehicle, 3);
	PED::SET_PED_INTO_VEHICLE(prisoner3, vehicle, 4);

	VEHICLE::SET_VEHICLE_ON_GROUND_PROPERLY(vehicle, 0);

	STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(wagonModel);

	AI::TASK_VEHICLE_DRIVE_WANDER(cop, vehicle, 10000.0f, 411);
	PED::SET_PED_KEEP_TASK(cop, true);

	struct
	{
		const char* speechName = "CALLOUT_CAMP_WAKE_UP";
		const char* voiceName = "0132_G_M_M_UNICRIMINALS_01_BLACK_01";
		alignas(8) int v3 = 0;
		alignas(8) Hash speechParamHash = GAMEPLAY::GET_HASH_KEY((char*)"speech_params_force");
		alignas(8) Entity entity;
		alignas(8) BOOL v6 = true;
		alignas(8) int v7 = 1;
		alignas(8) int v8 = 1;
	} speechData;

	//speechData.entity = prisoner1;


	AUDIO::_PLAY_AMBIENT_SPEECH1(playerPed, (char*)&speechData);
}

void EffectSpawnParrotCompanion::OnActivate()
{
	Effect::OnActivate();

	static Hash skinModel = GAMEPLAY::GET_HASH_KEY((char*)"A_C_Parrot_01");

	Ped ped = SpawnPedAroundPlayer(skinModel);

	MarkPedAsCompanion(ped);
	
	ENTITY::SET_ENTITY_MAX_HEALTH(ped, 1000);
	ENTITY::SET_ENTITY_HEALTH(ped, 1000, 0);

	Ped playerPed = PLAYER::PLAYER_PED_ID();

	if (!PED::IS_PED_IN_ANY_VEHICLE(playerPed, true) && !PED::IS_PED_ON_MOUNT(playerPed))
	{
		Vector3 vec = ENTITY::GET_ENTITY_COORDS(playerPed, true, 0);

		vec.z += 2.0f;

		ENTITY::SET_ENTITY_COORDS(ped, vec.x, vec.y, vec.z, false, false, false, false);
	}
}
