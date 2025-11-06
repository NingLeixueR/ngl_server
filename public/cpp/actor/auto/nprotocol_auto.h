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
// 注意【IDL 工具生成文件，不要手动修改】
// 创建时间 // 创建时间 25-11-06 17:49:11
#pragma once

#include "ndb_modular.h"
#include "nsp_read.h"
#include "nsp_write.h"
#include "nsp_server.h"
#include "actor_db.h"
#include "db.pb.h"


namespace ngl
{
	void tprotocol_customs_200000000()
	{
		tprotocol::set_customs_index(200000000);
		// 新增内部协议需要补充
		// 注册T与"np_mass_actor<T>"
		tprotocol::tp_customs_script::template func <
		>();
		tprotocol::set_customs_index(210000000);
		tprotocol::tp_customs_script::template func<
		>();
	}
}//namespace ngl
