#include "actor_gmclient.h"
namespace ngl
{
	bool actor_gmclient::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("all_protocol", [this](int id, const ngl::json_read& aos)
				{
					gcmd<protocols> lpro(id, "all_protocol");
					get_allprotocol(lpro.m_data);
				}
			);
			handle_cmd::push("server_stat", [this](int id, const ngl::json_read& aos)
				{
					gcmd<actor_manage::msg_actor_stat> lpro(id, "server_stat");
					actor_manage::getInstance().get_actor_stat(lpro.m_data);
				}
			);
			handle_cmd::push("set_time", [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> lpro(id, "set_time");
					struct operator_set_time
					{
						int32_t m_time = 0;
						jsonfunc("time", m_time)
					};
					operator_set_time ltime;
					if (aos.read("data", ltime))
					{
						bool lbool = localtime::settime(ltime.m_time);
						lpro.m_data = std::format("set time {} # {}", localtime::time2str("%Y-%m-%d %H:%M:%S"), lbool ? "success" : "fail");
					}
				}
			);
			handle_cmd::push("get_time", [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> lpro(id, "get_time", localtime::time2str("%Y-%m-%d %H:%M:%S"));
				}
			);
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_gmclient operator[{}] ERROR", loperator);
		}
		return true;
	}
}//namespace ngl
