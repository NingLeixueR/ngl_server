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

#include "actor/tab/ttab_item.h"
#include "tools/tab/csv/ncsv.h"
#include "actor/pb/db.pb.h"

namespace ngl
{
	class actor_role;
	
	class item_create
	{
		static std::map<EItemType, item_create*> m_data;
	protected:
		explicit item_create(EItemType atype)
		{
			m_data[atype] = this;
		}

		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec) = 0;
	public:
		static bool create(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			auto tab = ttab_item::instance().tab(atid);
			if (tab == nullptr)
			{
				return false;
			}
			auto itor = m_data.find((EItemType)tab->m_type);
			if (itor == m_data.end())
			{
				return false;
			}
			return itor->second->mode(arole, atid, acount, avec);
		}
	};
}// namespace ngl