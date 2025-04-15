#include "actor_family.h"

namespace ngl
{
	bool actor_family::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("change_familyname", [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "change_familyname", false);
					struct gm_changename
					{
						int64_t m_familid;
						std::string m_familname;
						jsonfunc("familid", m_familid, "familname", m_familname)
					};
					gm_changename recv;
					if (aos.read("data", recv) == false)
					{
						return;
					}
					pro.m_data = true;
					m_family.change_familyname(-1, recv.m_familid, recv.m_familname);
				}
			);

			handle_cmd::push("get_family", [this](int id, const ngl::json_read& aos)
				{
					gcmd<std::string> pro(id, "get_family");
					int64_t familid = 0;
					if (aos.read("data", familid) == false)
					{
						return;
					}
					auto profamilylist = m_family.get_familylist(familid);
					if (tools::protojson<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>(*profamilylist, pro.m_data) == false)
					{
						return;
					}
				}
			);
		}

		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}

	bool actor_family::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(adata.get_data()->m_actorid, lfamilyers))
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLESTAT>();
			pro->set_m_stat(pbnet::PROBUFF_NET_ROLESTAT::online);
			pro->set_m_logicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro->set_m_roleid(adata.get_data()->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}

	bool actor_family::handle(const message<np_eevents_logic_roleoffline>& adata)
	{
		std::vector<i64_actorid> lfamilyers;
		if (m_family.get_familyers(adata.get_data()->m_actorid, lfamilyers))
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLESTAT>();
			pro->set_m_stat(pbnet::PROBUFF_NET_ROLESTAT::offline);
			pro->set_m_logicstat(pbnet::PROBUFF_NET_ROLESTAT::familyer);
			pro->set_m_roleid(adata.get_data()->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
}//namespace ngl