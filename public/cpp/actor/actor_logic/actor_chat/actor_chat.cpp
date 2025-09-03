#include "actor_brief.h"
#include "actor_chat.h"
#include "net.pb.h"

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

		tdb_brief::nsp_cli<actor_chat>::instance(id_guid(), true).init_onlyread(this);
	}

	void actor_chat::erase_actor_before()
	{
		tdb_brief::nsp_cli<actor_chat>::instance(id_guid()).exit();
		tdb_brief::nsp_cli<actor_chat>::freensp(id_guid());
	}

	void actor_chat::loaddb_finish(bool adbishave) 
	{
	}

	void actor_chat::nregister()
	{
		// 定时器
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_chat>::func<
		>(false);
	}
	
	bool actor_chat::timer_handle(const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return true;
		}
		auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
		pro->set_mstat(true);
		pro->set_mtype(pbnet::ENUM_UPDATA_SPEAK);

		for (const auto& [_channelid, _update] : m_update_chatitem)
		{
			pro->set_mchannelid(_channelid);
			pro->clear_mchatlist();
			for (const pbnet::chatitem& item : _update)
			{
				*pro->add_mchatlist() = item;
			}
			send_client(pro);
		}
		m_update_chatitem.clear();
		return true;
	}

	bool actor_chat::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl