/*
* Copyright (c) [2020-2025] NingLeixueR
*
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
*
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
*
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for attribute.

#pragma once 

#include "actor/actor_logic/actor_role/logic/attribute/attribute.h"
#include "actor/actor_logic/actor_role/logic/attribute/unit.h"
#include "actor/protocol/nprotocol.h"

namespace ngl
{
	class fight_example
	{
		static int32_t m_increment;
		static std::map<int32_t, fight_example> m_fightdata;

		int32_t m_fightid = 0;
		std::map<int64_t, unit> m_unit;
		actor* m_actor = nullptr;
	public:
		static fight_example* fight_create()
		{
			fight_example& ltemp = m_fightdata[++m_increment];
			ltemp.m_fightid = m_increment;
			return &ltemp;
		}

		static fight_example* fight_get(int32_t aid)
		{
			auto itor = m_fightdata.find(aid);
			if (itor == m_fightdata.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		template <typename TDerived>
		TDerived* get_actor()
		{
			return (TDerived*)m_actor;
		}

		unit* get_unit(int32_t aunit)
		{
			auto itor = m_unit.find(aunit);
			if (itor == m_unit.end())
			{
				return nullptr;
			}
			return &itor->second;
		}

		template <typename TDerived>
		void init(TDerived* aactor)
		{
			m_actor = aactor;
			actor::register_actor_s<TDerived, np_attribute_remove>(e_ready_all, [](TDerived* aactor, const ngl::message<np_attribute_remove>& adata)
				{
					auto recv = adata.get_data();
					fight_example* lpfight = fight_get(recv->m_fightid);
					lpfight->handle(aactor, recv);
				}
			);
		}

		// # Remove[attribute]
		template <typename TDerived>
		void handle(TDerived* aactor, const np_attribute_remove* adata)
		{
			auto lunit = get_unit(adata->m_unitid);
			if (lunit == nullptr)
			{
				return;
			}
			lunit->get_attribute().remove(adata->m_module);
		}

		// Translated comment.
		// Correspondinghandle handle
		template <typename T>
		void trigger(std::shared_ptr<T>& aparm, int32_t amilliseconds)
		{
			if (amilliseconds > 0)
			{
				tools::wheel_parm lparm
				{
					.m_ms = amilliseconds,
					.m_intervalms = [amilliseconds](int64_t)
					{
						return amilliseconds;
					} ,
					.m_count = 1,
					.m_fun = [this,aparm](const tools::wheel_node*)
					{
						actor::send_actor(m_actor->id_guid(), nguid::make(), aparm);
					}
				};
				tools::twheel::wheel().addtimer(lparm);
			}
			else
			{
				m_actor->ahandle(aparm);
			}
		}
	};
}//namespace ngl