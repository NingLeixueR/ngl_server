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
#include "actor/tab/ttab_servers.h"
#include "actor/actor_base/nguid.h"

namespace ngl
{
	i64_actorid nguid::make_self(ENUM_ACTOR atype)
	{
		nguid lguid(atype, tab_self_area, none_actordataid());
		return (i64_actorid)lguid;
	}
}//namespace ngl