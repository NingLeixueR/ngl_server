/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor_gmclient.h"
namespace ngl
{
	bool actor_gmclient::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("all_protocol") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<protocols> lpro(id, "all_protocol");
					get_allprotocol(lpro.m_data);
				};

			handle_cmd::add("server_stat") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<actor_manage::msg_actor_stat> lpro(id, "server_stat");
					actor_manage::instance().get_actor_stat(lpro.m_data);
				};

			handle_cmd::add("set_time") = [this](int id, ngl::njson_read& aos)
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

			handle_cmd::add("get_time") = [this](int id, ngl::njson_read& aos)
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
