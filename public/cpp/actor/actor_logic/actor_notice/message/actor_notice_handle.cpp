/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "actor/actor_logic/actor_notice/actor_notice.h"
namespace ngl
{
	struct gm_notice
	{
		int64_t		m_id = 0;// 公告id
		std::string	m_notice;
		int32_t		m_starttime = 0;// 开始时间
		int32_t		m_finishtime = 0;// 结束时间
		DPROTOCOL(gm_notice, m_id, m_notice, m_starttime, m_finishtime)
	};
	bool actor_notice::handle(const message<mforward<np_gm>>& adata)
	{
		ncjson lojson(adata.get_data()->data()->m_json.c_str());

		std::string loperator;
		if (!njson::pop(lojson.json(), { "operator" }, loperator))
		{
			return true;
		}

		if (handle_cmd::empty())
		{
			handle_cmd::add("get_notice") = [this](int id, ncjson& aos)
				{// 返回 {"notice":gm_notice[]}
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
					if (!njson::pop(aos.json(), { "data" }, recv))
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
					if (!njson::pop(aos.json(), { "data" }, lid))
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