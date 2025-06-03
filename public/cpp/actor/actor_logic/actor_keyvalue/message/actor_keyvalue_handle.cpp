#include "actor_keyvalue.h"

namespace ngl
{
	bool actor_keyvalue::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::add("set_openserver") = [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "set_openserver", false);
					int32_t ltime;
					if (aos.read("data", ltime) == false)
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

			handle_cmd::add("get_openserver") = [this](int id, const ngl::json_read& aos)
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