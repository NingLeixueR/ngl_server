/*
* Copyright (c) [2020-2025] NingLeixueR
*
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
*
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
*
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once 

#include "nprotocol.h"
#include "attribute.h"
#include "unit.h"

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

		// # 移除[attribute]
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

		// # 立即触发/延时触发
		// 对应handle方法  进行触发处理
		template <typename T>
		void trigger(std::shared_ptr<T>& aparm, int32_t amilliseconds)
		{
			if (amilliseconds > 0)
			{
				wheel_parm lparm
				{
					.m_ms = amilliseconds,
					.m_intervalms = [amilliseconds](int64_t)
					{
						return amilliseconds;
					} ,
					.m_count = 1,
					.m_fun = [this,aparm](const wheel_node*)
					{
						actor::send_actor(m_actor->id_guid(), nguid::make(), aparm);
					}
				};
				twheel::wheel().addtimer(lparm);
			}
			else
			{
				m_actor->ahandle(aparm);
			}
		}
	};
}//namespace ngl