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
#include "actor_gmclient.h"
namespace ngl
{
	bool actor_gmclient::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njread lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("all_protocol") = [this](int id, ngl::njread& aos)
				{
					gcmd<protocols> lpro(id, "all_protocol");
					allprotocol(lpro.m_data);
				};

			handle_cmd::add("server_stat") = [this](int id, ngl::njread& aos)
				{
					gcmd<msg_actor_stat> lpro(id, "server_stat");
					actor_manage::instance().get_actor_stat(lpro.m_data);
				};

			handle_cmd::add("set_time") = [this](int id, ngl::njread& aos)
				{
					gcmd<std::string> lpro(id, "set_time");
					struct operator_set_time
					{
						int32_t m_time = 0;
						dprotocol(operator_set_time, m_time)
					};
					operator_set_time ltime;
					if (njson::read(aos, "data", ltime))
					{
						bool lbool = localtime::settime(ltime.m_time);
						lpro.m_data = std::format("set time {} # {}", localtime::time2str("%Y-%m-%d %H:%M:%S"), lbool ? "success" : "fail");
					}
				};

			handle_cmd::add("get_time") = [this](int id, ngl::njread& aos)
				{
					gcmd<std::string> lpro(id, "get_time", localtime::time2str("%Y-%m-%d %H:%M:%S"));
				};
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_gmclient operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl
