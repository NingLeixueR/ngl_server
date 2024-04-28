#include "actor_notice.h"

namespace ngl
{
	actor_notice::actor_notice() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_NOTICE,
					.m_area = ttab_servers::tab()->m_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_notice::nregister()
	{
		// 协议注册
		register_actor_handle<EPROTOCOL_TYPE_CUSTOM
			, actor_notice
			, mforward<np_gm>
		>(true);
		register_actor_handle<EPROTOCOL_TYPE_PROTOCOLBUFF
			, actor_notice
			, mforward<pbnet::PROBUFF_NET_NOTICE>
		>(true);
	}

	bool actor_notice::handle(message<np_actor_addnotice>& adata)
	{
		auto& recv = *adata.m_data;
		m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
		return true;
	}

	struct gm_notice
	{
		int64_t		m_id;// 公告id
		std::string	m_notice;
		int32_t		m_starttime;// 开始时间
		int32_t		m_finishtime;// 结束时间

		jsonfunc("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
	};

	bool actor_notice::handle(message<mforward<np_gm>>& adata)
	{
		//using type = mforward<np_gm_response>;
		ngl::ojson lojson(adata.m_data->data()->m_json.c_str());

		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		if (handle_cmd::empty())
		{
			handle_cmd::push("get_notice", [this](int id, ngl::ojson& aos)
				{// 返回 {"notice":gm_notice[]}
					gcmd<std::vector<std::string>> pro;
					pro.id = id;
					pro.m_operator = "get_notice_responce";
					
					std::map<nguid, data_modified<pbdb::db_notice>>& lmapdb = m_notice.data();
					for (const auto& [_guid, _data] : lmapdb)
					{
						pro.m_data.resize(pro.m_data.size() + 1);
						serialize::proto_json(_data.getconst(), *pro.m_data.rbegin());
					}
					pro.m_istoutf8 = false;
				}
			);
			handle_cmd::push("add_notice", [this](int id, ngl::ojson& aos)
				{
					// 返回 bool
					gm_notice recv;
					if (aos.read("data", recv) == false)
						return;
					gcmd<bool> pro;
					pro.id = id;
					pro.m_operator = "add_notice_responce";
					pro.m_data = true;
					m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
				}
			);
			handle_cmd::push("del_notice", [this](int id, ngl::ojson& aos)
				{
					// 返回 bool
					int64_t lid = 0;
					if (aos.read("data", lid) == false)
						return;
					gcmd<bool> pro;
					pro.id = id;
					pro.m_operator = "del_notice_responce";
					pro.m_data = true;
					m_notice.del_notice(lid);
				}
			);
		}
		if (handle_cmd::function(loperator, adata.m_data->identifier(), lojson) == false)
		{
			LogLocalError("GM actor_notice operator[%] ERROR", loperator);
		}
		return true;
	}

	bool actor_notice::handle(message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata)
	{
		auto pro = m_notice.sync_notice(-1);
		send_client(adata.m_data->identifier(), pro);
		return true;
	}
}