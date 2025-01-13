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
					.m_area = tab_self_area,
					.m_manage_dbclient = true,
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	void actor_notice::nregister()
	{
		// Э��ע��
		register_handle_custom<actor_notice>::func<
			mforward<np_gm>
		>(true);

		register_handle_proto<actor_notice>::func<
			mforward<pbnet::PROBUFF_NET_NOTICE>
		>(true);
	}

	bool actor_notice::handle(const message<np_actor_addnotice>& adata)
	{
		auto& recv = *adata.get_data();
		m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
		return true;
	}

	struct gm_notice
	{
		int64_t		m_id;// ����id
		std::string	m_notice;
		int32_t		m_starttime;// ��ʼʱ��
		int32_t		m_finishtime;// ����ʱ��

		jsonfunc("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
	};

	bool actor_notice::handle(const message<mforward<np_gm>>& adata)
	{
		//using type = mforward<np_gm_response>;
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());

		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		if (handle_cmd::empty())
		{
			handle_cmd::push("get_notice", [this](int id, const ngl::json_read& aos)
				{// ���� {"notice":gm_notice[]}
					gcmd<std::vector<std::string>> pro(id);
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
			handle_cmd::push("add_notice", [this](int id, const ngl::json_read& aos)
				{
					// ���� bool
					gm_notice recv;
					if (aos.read("data", recv) == false)
					{
						return;
					}
					gcmd<bool> pro(id);
					pro.m_operator = "add_notice_responce";
					pro.m_data = localtime::checkutc(recv.m_finishtime);
					if (pro.m_data)
					{
						m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
					}
				}
			);
			handle_cmd::push("del_notice", [this](int id, const ngl::json_read& aos)
				{
					// ���� bool
					int64_t lid = 0;
					if (aos.read("data", lid) == false)
					{
						return;
					}
					gcmd<bool> pro(id);
					pro.m_operator = "del_notice_responce";
					pro.m_data = true;
					m_notice.del_notice(lid);
				}
			);
		}
		if (handle_cmd::function(loperator, adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_notice operator[{}] ERROR", loperator);
		}
		return true;
	}

	bool actor_notice::handle(const message<mforward<pbnet::PROBUFF_NET_NOTICE>>& adata)
	{
		auto pro = m_notice.sync_notice(-1);
		send_client(adata.get_data()->identifier(), pro);
		return true;
	}
}//namespace ngl