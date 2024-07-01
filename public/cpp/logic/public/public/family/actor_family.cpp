#include "actor_family.h"

namespace ngl
{
	actor_family::actor_family() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_FAMILY,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_family::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_family>::func<
			mforward<np_gm>
		>(true);

		register_handle_proto<actor_family>::func<
			mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>
			, mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LIST>
		>(true);
	}

	bool actor_family::handle(message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}
		if (handle_cmd::empty())
		{
			handle_cmd::push("change_famil", [this](int id, ngl::json_read& aos)
				{
					int64_t familid = 0;
					struct gm_changename
					{
						int64_t m_familid;
						std::string m_familname;
						jsonfunc("familid", m_familid, "familname", m_familname)
					};
					gm_changename recv;
					if (aos.read("data", recv) == false)
						return;
					gcmd<std::string> pro;
					pro.id = id;
					pro.m_operator = "change_famil_responce";

					m_family.change_familyname(-1, recv.m_familid, recv.m_familname);
				}
			);
		}

		if (handle_cmd::function(loperator, adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_mail operator[{}] ERROR", loperator);
		}
		return true;
	}
}// namespace ngl