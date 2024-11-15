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
					.m_area = tab_self_area,
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
			mforward<np_gm>,
			np_eevents_logic_rolelogin
		>(true);

		register_handle_proto<actor_family>::func<
			mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>
			, mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>
			, mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>
			, mforward<pbnet::PROBUFF_NET_FAMIL_LIST>
			, mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>
			, mforward<pbnet::PROBUFF_NET_FAMILSIGN>
		>(true);
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CREATE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();

		auto pro = std::make_shared<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>();
		int32_t lstat = m_family.create_family(lroleid, adata.get_data()->data()->m_name());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_JOIN_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>();
		int32_t lstat = 0;
		if (adata.get_data()->data()->m_apply())
		{
			lstat = m_family.join_family(lroleid, familid);
		}
		else
		{
			lstat = m_family.cancel_join_family(lroleid, familid);
		}
		
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid ljoinroleid = adata.get_data()->data()->m_roleid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>();
		int32_t lstat = m_family.ratify_join_family(lroleid, ljoinroleid, adata.get_data()->data()->m_ratify());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CEDE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid lcederoleid = adata.get_data()->data()->m_roleid();

		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_LEAVE_FAMIL>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>();
		int32_t lstat = m_family.leave_family(lroleid, familid);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMIL_LIST>>& adata)
	{
		m_family.sync_family(adata.get_data()->identifier(), adata.get_data()->data()->m_familid());
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_CHANGE_FAMILNAME>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>();
		int32_t lstat = m_family.change_familyname(lroleid, familid, adata.get_data()->data()->m_name());
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

	bool actor_family::handle(const message<mforward<pbnet::PROBUFF_NET_FAMILSIGN>>& adata)
	{
		i64_actorid lroleid = adata.get_data()->identifier();
		i64_actorid familid = adata.get_data()->data()->m_familid();

		auto pro = std::shared_ptr<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>();
		int32_t lstat = m_family.sign_family(lroleid, familid);
		pro->set_m_stat(lstat);
		send_client(lroleid, pro);
		return true;
	}

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
					struct gm_changename
					{
						int64_t m_familid;
						std::string m_familname;
						jsonfunc("familid", m_familid, "familname", m_familname)
					};
					gm_changename recv;
					if (aos.read("data", recv) == false)
						return;
					gcmd<bool> pro;
					pro.id = id;
					pro.m_operator = "change_familyname_responce";
					pro.m_data = true;
					m_family.change_familyname(-1, recv.m_familid, recv.m_familname);
				}
			);

			handle_cmd::push("get_family", [this](int id, const ngl::json_read& aos)
				{
					int64_t familid = 0;
					if (aos.read("data", familid) == false)
						return;
					gcmd<std::string> pro;
					pro.id = id;
					pro.m_operator = "get_family_responce";
					auto profamilylist = m_family.get_familylist(familid);
					if (tools::protojson<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>(*profamilylist, pro.m_data) == false)
					{
						return;
					}
				}
			);
		}

		if (handle_cmd::function(loperator, adata.get_data()->identifier(), lojson) == false)
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
			auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLELOGIN>();
			pro->set_m_stat(pbnet::PROBUFF_NET_ROLELOGIN::familyer);
			pro->set_m_roleid(adata.get_data()->m_actorid);
			send_client(lfamilyers, pro);
		}
		return true;
	}
}// namespace ngl