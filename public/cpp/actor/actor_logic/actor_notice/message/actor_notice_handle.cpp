/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_notice/actor_notice.h"
namespace ngl
{
	struct gm_notice
	{
		int64_t		m_id = 0;// Noticeid
		std::string	m_notice;
		int32_t		m_starttime = 0;// Time
		int32_t		m_finishtime = 0;// Endtime
		DPROTOCOL(gm_notice, m_id, m_notice, m_starttime, m_finishtime)
	};
	bool actor_notice::handle(const message<mforward<np_gm>>& adata)
	{
		ncjson lojson(adata.get_data()->data()->m_json.c_str());

		std::string loperator;
		if (!njson::pop(lojson, { "operator" }, loperator))
		{
			return true;
		}

		if (handle_cmd::empty())
		{
			handle_cmd::add("get_notice") = [this](int id, ncjson& aos)
				{// Return {"notice":gm_notice[]}
					gcmd<std::vector<std::string>> pro(id, "get_notice");
					for (auto& [_guid, _data] : m_notice.data())
					{
						pro.m_data.resize(pro.m_data.size() + 1);
						MODIFIED_CONTINUE_CONST(lpdbnoticeconst, _data);
						tools::proto2json(*lpdbnoticeconst, *pro.m_data.rbegin());
					}
					pro.m_istoutf8 = false;
				};

			handle_cmd::add("notice_add") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "notice_add", false);
					gm_notice recv;
					if (!njson::pop(aos, { "data" }, recv))
					{
						return;
					}
					m_notice.notice_add(recv.m_notice, recv.m_starttime, recv.m_finishtime);
					pro.m_data = true;
				};

			handle_cmd::add("notice_del") = [this](int id, ncjson& aos)
				{
					gcmd<bool> pro(id, "notice_del", false);
					int64_t lid = 0;
					if (!njson::pop(aos, { "data" }, lid))
					{
						return;
					}
					pro.m_data = true;
					m_notice.notice_del(lid);
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
		pbnet::PROBUFF_NET_NOTICE_RESPONSE pro;
		m_notice.sync_notice(-1, pro);
		send_client(adata.get_data()->identifier(), pro);
		return true;
	}
	bool actor_notice::handle(const message<np_actor_addnotice>& adata)
	{
		auto& recv = *adata.get_data();
		m_notice.notice_add(recv.m_notice, recv.m_starttime, recv.m_finishtime);
		return true;
	}
}//namespace ngl