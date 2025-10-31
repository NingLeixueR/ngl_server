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
#include "actor_role.h"
#include "autoitem.h"

namespace ngl
{
	void autoitem::add(int32_t atid, int32_t acount)
	{
		m_addstackitems[atid] += acount;
	}

	void autoitem::add(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}

	void autoitem::del(int32_t aid, int32_t acount)
	{
		m_delstackitems[aid] += acount;
	}

	void autoitem::del(int32_t aid)
	{
		m_addnostackitems.push_back(aid);
	}
}// namespace ngl