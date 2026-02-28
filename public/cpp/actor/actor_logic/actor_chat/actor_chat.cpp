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

#include "actor/actor_logic/actor_brief/actor_brief.h"
#include "actor/actor_logic/actor_chat/actor_chat.h"
#include "actor/pb/net.pb.h"

namespace ngl
{
	actor_chat::actor_chat() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_CHAT,
						.m_area = tab_self_area
					},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_chat::actor_type()
	{
		return ACTOR_CHAT;
	}

	i64_actorid actor_chat::actorid()
	{
		return nguid::make(ACTOR_CHAT, tab_self_area, nguid::none_actordataid());
	}

	void actor_chat::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);

		std::set<i32_fieldnumber> lfieldset;
		tdb_brief::nsp_cread<actor_chat>::instance_readall(this, pb_field::field_number<pbdb::db_brief>(lfieldset, "mid", "mbase"));
	}

	void actor_chat::erase_actor_before()
	{
		tdb_brief::nsp_cread<actor_chat>::instance(id_guid()).exit();
	}

	void actor_chat::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_chat::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_chat::nregister()
	{
		// 定时器
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_chat
			, mforward<pbnet::PROBUFF_NET_CHAT>
		>(e_ready_all);
	}
	
	bool actor_chat::timer_handle(const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return true;
		}
		pbnet::PROBUFF_NET_CHAT_RESPONSE pro;
		pro.set_mstat(true);
		pro.set_mtype(pbnet::ENUM_UPDATA_SPEAK);

		for (auto& [_channelid, _update] : m_update_chatitem)
		{
			pro.set_mchannelid(_channelid);
			pro.clear_mchatlist();
			for (const pbnet::chatitem& item : _update)
			{
				*pro.add_mchatlist() = item;
			}
			send_client(pro, ENET_KCP);
		}
		m_update_chatitem.clear();
		return true;
	}
}// namespace ngl