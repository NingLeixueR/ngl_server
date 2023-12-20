#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_role.h"
#include "gateway_info.h"

namespace ngl
{
	class actor_gateway_game2client : public actor
	{
		gateway_info m_info;

		actor_gateway_game2client();
	public:
		friend class actor_instance<actor_gateway_game2client>;
		static actor_gateway_game2client& getInstance()
		{
			return actor_instance<actor_gateway_game2client>::instance();
		}

		virtual ~actor_gateway_game2client();

		static void actor_register();

		template <EPROTOCOL_TYPE TYPE, typename T>
		bool handle(message<actor_forward<T, TYPE, true, ngl::forward>>& adata)
		{
			auto lparm = adata.m_data;
			auto lpack = adata.m_pack;
			//Game->Gate  需要把这个消息传递给Client服务器
			gateway_socket* info = nullptr;
			std::map<i32_sessionid, i64_actorid> lmap;
			if (lparm->m_uid.empty())
				return true;
			if (lparm->m_area[0] == -1 && lparm->m_uid[0] == -1)
			{
				// 发给所有客户端
				for (auto& itemfor1 : m_info.info())
				{
					for (auto& itemfor2 : itemfor1.second)
					{
						lmap.insert(std::make_pair(itemfor2.second.m_socket, actor_guid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid)));
					}
				}
			}
			else if (lparm->m_area[0] != -1 && lparm->m_uid[0] == -1)
			{
				// 发给指定区服上的所有客户端
				auto itemitor = m_info.info().find(lparm->m_area[0]);
				if (itemitor != m_info.info().end())
				{
					for (auto& itemfor1 : itemitor->second)
					{
						lmap.insert(std::make_pair(itemfor1.second.m_socket, actor_guid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid)));
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
					lmap.insert(std::make_pair(info->m_socket, actor_guid::make(ACTOR_ROBOT, lparm->m_area[i], lparm->m_uid[i])));
				}
			}
			actor_forward<T, TYPE, false, ngl::forward> ltemp(*lparm);
			nserver->sendmore(lmap, ltemp, lpack->m_head.get_request_actor());
			return true;
		}

		bool handle(message<actor_gateway_info_updata>& adata);
	private:
	};

}

