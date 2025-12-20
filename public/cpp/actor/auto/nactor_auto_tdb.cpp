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
 // 注意【makeproto 工具生成文件，不要手动修改】
 // 创建时间【2025-12-20 11:03:02】
#include "auto_actor_enum.h"
#include "nprotocol_auto.h"
#include "nactor_auto.h"

namespace ngl
{
	void tdb::tdb_init(bool ainstance)
	{
		tdb_account::init(ainstance);
		tdb_activity::init(ainstance);
		tdb_activitytimes::init(ainstance);
		tdb_bag::init(ainstance);
		tdb_brief::init(ainstance);
		tdb_family::init(ainstance);
		tdb_familyer::init(ainstance);
		tdb_friends::init(ainstance);
		tdb_keyvalue::init(ainstance);
		tdb_mail::init(ainstance);
		tdb_notice::init(ainstance);
		tdb_ranklist::init(ainstance);
		tdb_role::init(ainstance);
		tdb_rolekv::init(ainstance);
		tdb_task::init(ainstance);
		tdb_testlua::init(ainstance);
    }
}//namespace ngl