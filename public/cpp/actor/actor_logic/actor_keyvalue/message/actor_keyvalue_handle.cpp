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
#include "actor_keyvalue.h"
namespace ngl
{
	bool actor_keyvalue::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::njson_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (!njson::read(lojson, "operator", loperator))
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("set_openserver") = [this](int id, ngl::njson_read& aos)
				{
					gcmd<bool> pro(id, "set_openserver", false);
					int32_t ltime;
					if (!njson::read(aos, "data", ltime))
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

					m_keyvaluedb.add_keyvalue(pbdb::db_keyvalue::open_server, ltime);
					pro.m_data = true;
				};

			handle_cmd::add("get_openserver") = [this](int id, const ngl::njson_read& aos)
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
