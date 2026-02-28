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

#include "actor/actor_logic/actor_role/logic/rolekv.h"
#include "actor/actor_logic/actor_role/actor_role.h"

namespace ngl
{
	void rolekv::initdata()
	{
		log_error()->print("[db_rolekeyvalue load finish] {}", data());
	}

	const char* rolekv::get_value(const char* akey)
	{
		MODIFIED_RETUAN(lpdbrolekeyvalue, get(get_actor()->id_guid()), nullptr);
		return (*lpdbrolekeyvalue->mutable_mdata())[akey].c_str();
	}

	void rolekv::set_value(const char* akey, const char* adata)
	{
		MODIFIED_RETUAN(lpdbrolekeyvalue, get(get_actor()->id_guid()));
		(*lpdbrolekeyvalue->mutable_mdata())[akey] = adata;
	}
}//namespace ngl