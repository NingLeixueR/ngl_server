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

#include "actor/auto/nactor_auto.h"

namespace ngl
{
	class account : 
		public tdb_account::db_modular
	{
		account(const account&) = delete;
		account& operator=(const account&) = delete;

		struct accountbyname
		{
			std::map<std::string, data_modified<pbdb::db_account>*> m_data;
		};
		std::map<i16_area, accountbyname> m_areaofaccount;
		
		int32_t m_max_accountid;
	public:
		account();

		void related_actorid() final;

		virtual void initdata();

		int32_t& max_accountid();

		std::map<std::string, data_modified<pbdb::db_account>*>& accountbyarea(int area);
	};
}//namespace ngl

mk_formatter(pbdb::db_account)