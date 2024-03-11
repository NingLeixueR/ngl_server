#pragma once

#include "actor_manage.h"
#include "actor_role.h"
#include "gateway_info.h"
#include "db_manage.h"
#include "ndbclient.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.h"
#include "db.h"

namespace ngl
{
	class actor_gatewayg2c : public actor
	{
		gateway_info m_info;

		actor_gatewayg2c();
	public:
		friend class actor_instance<actor_gatewayg2c>;
		static actor_gatewayg2c& getInstance()
		{
			return actor_instance<actor_gatewayg2c>::instance();
		}

		virtual ~actor_gatewayg2c();

		static void nregister();

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(message<np_actor_forward<T, TYPE, true, ngl::forward>>& adata)
		{
			auto lparm = adata.m_data;
			auto lpack = adata.m_pack;
			//Game->Gate  ��Ҫ�������Ϣ���ݸ�Client������
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (lparm->m_uid.empty())
				return true;
			if (lparm->m_area[0] == -1 && lparm->m_uid[0] == -1)
			{
				// �������пͻ���
				for (auto& itemfor1 : m_info.info())
				{
					for (auto& itemfor2 : itemfor1.second)
					{
						lmap.insert(std::make_pair(itemfor2.second.m_socket, nguid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid)));
					}
				}
			}
			else if (lparm->m_area[0] != -1 && lparm->m_uid[0] == -1)
			{
				// ����ָ�������ϵ����пͻ���
				auto itemitor = m_info.info().find(lparm->m_area[0]);
				if (itemitor != m_info.info().end())
				{
					for (auto& itemfor1 : itemitor->second)
					{
						lmap.insert(std::make_pair(itemfor1.second.m_socket, nguid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid)));
					}
				}
			}
			else
			{
				for (int i = 0; i < lparm->m_uid.size() && i < lparm->m_area.size(); ++i)
				{
					info = m_info.get(lparm->m_area[i], lparm->m_uid[i]);
					if (info == nullptr)
						continue;
					lmap.insert(std::make_pair(info->m_socket, nguid::make(ACTOR_ROBOT, lparm->m_area[i], lparm->m_uid[i])));
				}
			}
			np_actor_forward<T, TYPE, false, ngl::forward> ltemp(*lparm);
			nserver->sendmore(lmap, ltemp, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(message<np_actor_gatewayinfo_updata>& adata);
	};

}//namespace ngl
