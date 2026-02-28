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
#include "actor/actor_logic/actor_keyvalue/actor_keyvalue.h"
namespace ngl
{
	bool actor_keyvalue::handle(const message<mforward<np_gm>>& adata)
	{
		ncjson lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::pop(lojson.json(), { "operator" }, loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("set_openserver") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "set_openserver", false);
					int32_t ltime = 0;
					if (!njson::pop(aos.json(), { "data" }, ltime))
					{
						return;
					}
					log_error()->print("set server time = [{}]", localtime::time2str(ltime));
					
					int32_t lopentime = 0;
					if (m_keyvaluedb.keyvalue(pbdb::db_keyvalue::open_server, lopentime) && lopentime != 0)
					{
						log_error()->print("set server fail time = [{}]", localtime::time2str(lopentime));
						return;
					}

					m_keyvaluedb.keyvalue_add(pbdb::db_keyvalue::open_server, ltime);
					pro.m_data = true;
				};

			handle_cmd::add("get_openserver") = [this](int id, const ncjson& aos)
				{
					gcmd<std::string> pro(id, "get_openserver");
					int32_t lopentime = 0;
					if (!m_keyvaluedb.keyvalue(pbdb::db_keyvalue::open_server, lopentime) || lopentime == 0)
					{
						pro.m_data = "fail";
						return;
					}
					pro.m_data = localtime::time2str(lopentime);
				};
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl