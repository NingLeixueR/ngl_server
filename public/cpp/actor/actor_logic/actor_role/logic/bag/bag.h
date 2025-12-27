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

#include "nactor_auto.h"
#include "remakes.h"
#include "db.pb.h"
#include "itme.h"

namespace ngl
{
	struct autoitem;

	class bag :
		public tdb_bag::db_modular
	{
		std::map<int, pbdb::item*> m_stackitems;				// key tid 可堆叠的物品
		std::map<int, pbdb::item*> m_nostackitems;				// key id  不可堆叠的物品
		std::unique_ptr<autoitem>  m_autoitem;
	public:
		bag();

		virtual void initdata();

		pbdb::item* add(pbdb::item& aitem);
	
		bool add_item(int32_t atid, int32_t acount);
		bool add_item(const std::map<int32_t, int32_t>& amap);
		bool add_item(std::vector<pbdb::item>& avec);

		// 只能删除可堆叠物品
		bool dec_item(int32_t atid, int32_t acount);
		// 删除不可堆叠物品
		bool dec_item(int32_t aid);
		
		bool checkbytid(int32_t atid, int32_t acount);
		bool checkbyid(int32_t aid);

		void sync_client();

		void print_bi(int32_t aitemid, int32_t atid, int32_t acount, bool aisadd = true);
	};
}// namespace ngl

mk_formatter(pbdb::db_bag)