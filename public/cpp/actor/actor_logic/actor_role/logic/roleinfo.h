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
#include "net.pb.h"
#include "db.pb.h"
#include "ncsv.h"

namespace ngl
{
	class roleinfo : 
		public tdb_role::db_modular
	{
		roleinfo(const roleinfo&) = delete;
		roleinfo& operator=(const roleinfo&) = delete;
	public:
		roleinfo() = default;

		virtual void initdata();

		const i64_actorid roleid();

		pbdb::db_brief* get_brief();
		const pbdb::db_brief* get_constbrief();

		void change_event(eevents_logic type, int abegvalue, int32_t anowvalue);

		// # 等级
		int32_t lv();
		void change_lv(int avalues);

		// # vip
		int32_t vip();
		void change_vip(int avalues);

		// # 名字
		const char* name();
		void rename(const char* aname);

		// # 金币
		int32_t gold();
		void change_gold(int avalues);

		// # 银币
		int32_t silver();
		void change_silver(int avalues);

		// # 禁言
		int32_t notalkutc();
		void change_notalkutc(int avalues);
		bool bantalk();
	};
}//namespace ngl

mk_formatter(pbdb::db_role)