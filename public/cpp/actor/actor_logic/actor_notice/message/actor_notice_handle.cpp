#include "actor_notice.h"
namespace ngl
{
	struct gm_notice
	{
		int64_t		m_id;// 公告id
		std::string	m_notice;
		int32_t		m_starttime;// 开始时间
		int32_t		m_finishtime;// 结束时间
		def_jsonfunction(m_id, m_notice, m_starttime, m_finishtime)
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
			handle_cmd::add("get_notice") = [this](int id, const ngl::json_read& aos)
				{// 返回 {"notice":gm_notice[]}
					gcmd<std::vector<std::string>> pro(id, "get_notice");
					std::map<nguid, data_modified<pbdb::db_notice>>& lmapdb = m_notice.data();
					for (const auto& [_guid, _data] : lmapdb)
					{
						pro.m_data.resize(pro.m_data.size() + 1);
						tools::proto2json(_data.getconst(), *pro.m_data.rbegin());
					}
					pro.m_istoutf8 = false;
				};

			handle_cmd::add("add_notice") = [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "add_notice", false);
					gm_notice recv;
					if (aos.read("data", recv) == false)
					{
						return;
					}
					m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
					pro.m_data = true;
				};

			handle_cmd::add("del_notice") = [this](int id, const ngl::json_read& aos)
				{
					// 返回 bool
					gcmd<bool> pro(id, "del_notice", false);
					int64_t lid = 0;
					if (aos.read("data", lid) == false)
					{
						return;
					}
					pro.m_data = true;
					m_notice.del_notice(lid);
				};
		}
		if (handle_cmd::function(loperator, (int32_t)adata.get_data()->identifier(), lojson) == false)
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
	bool actor_notice::handle(const message<np_actor_addnotice>& adata)
	{
		auto& recv = *adata.get_data();
		m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
		return true;
	}
}//namespace ngl
