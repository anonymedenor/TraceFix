#include "precompiled.h"

CTraceFix gTraceFix;

void CTraceFix::ServerActivate()
{
	for (int WeaponID = WEAPON_P228; WeaponID <= WEAPON_P90; WeaponID++)
	{
		auto SlotInfo = g_ReGameAPI_Api->GetWeaponSlot((WeaponIdType)WeaponID);

		if (SlotInfo)
		{
			if ((SlotInfo->slot == PRIMARY_WEAPON_SLOT) || (SlotInfo->slot == PISTOL_SLOT))
			{
				if (SlotInfo->weaponName)
				{
					if (SlotInfo->weaponName[0u] != '\0')
					{
						if (this->m_tf_distance.find(WeaponID) == this->m_tf_distance.end())
						{
							std::string cvarName = "tf_";

							cvarName.append(SlotInfo->weaponName);

							this->m_tf_distance[WeaponID] = gTraceUtil.CvarRegister(cvarName.c_str(), "8192.0");
						}
					}
				}
			}
		}
	}

	gTraceUtil.ServerCommand("exec %s/tracefix.cfg", gTraceUtil.GetPath());
}

void CTraceFix::TraceLine(const float* vStart, const float* vEnd, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr)
{
	auto EntityIndex = ENTINDEX(pentToSkip);

	if (EntityIndex > 0 && EntityIndex <= gpGlobals->maxClients)
	{
		auto Player = UTIL_PlayerByIndexSafe(EntityIndex);

		if (fNoMonsters == dont_ignore_monsters && Player && Player->IsAlive() && Player->m_pActiveItem)	
		{
			if ((Player->m_pActiveItem->iItemSlot() == PRIMARY_WEAPON_SLOT) || (Player->m_pActiveItem->iItemSlot() == PISTOL_SLOT))	
			{
				auto DistanceLimit = this->m_tf_distance[Player->m_pActiveItem->m_iId]->value;

				if (DistanceLimit > 0.0f)
				{
					bool OnGround = (Player->pev->flags & FL_ONGROUND) != 0;
					auto trResult = gTraceUtil.GetUserAiming(pentToSkip, DistanceLimit);
					float ffAccuracy = OnGround ? 9999.0f : 0.0f;

					if (!FNullEnt(trResult.pHit))
					{
						g_engfuncs.pfnMakeVectors(pentToSkip->v.v_angle);

						auto vEndRes = Vector
						(
							(vStart[0] + (gpGlobals->v_forward[0] * 9999.0f)),
							(vStart[1] + (gpGlobals->v_forward[1] * 9999.0f)),
							(vStart[2] + (gpGlobals->v_forward[2] * 9999.0f))
						);

						g_engfuncs.pfnTraceLine(vStart, vEndRes, fNoMonsters, pentToSkip, ptr);
					}
				}
			}
		}
	}
}
