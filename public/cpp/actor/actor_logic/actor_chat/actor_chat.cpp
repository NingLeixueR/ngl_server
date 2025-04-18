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
		std::set<i64_actorid> ldataid;
		tdb_brief::nsp_cli<actor_chat>::init(actor_brief::actor_type(), this, ldataid);
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
	}

	void actor_chat::loaddb_finish(bool adbishave) 
	{
	}

	void actor_chat::nregister()
	{
		// 定时器
		actor::register_timer<actor_chat>(&actor_chat::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_chat>::func<
		>(true);

		// 绑定pb消息
		register_handle_proto<actor_chat>::func<
			mforward<pbnet::PROBUFF_NET_CHAT>
		>(true);
	}
	
	bool actor_chat::timer_handle(const message<np_timerparm>& adata)
	{
		if (adata.get_data()->m_type != E_ACTOR_TIMER::ET_INTERVAL_SEC)
		{
			return true;
		}
		auto pro = std::make_shared<pbnet::PROBUFF_NET_CHAT_RESPONSE>();
		pro->set_m_stat(true);
		pro->set_m_type(pbnet::updata_speck);

		for (const auto& [_channelid, _update] : m_update_chatitem)
		{
			pro->set_m_channelid(_channelid);
			pro->clear_m_chatlist();
			for (const pbnet::chatitem& item : _update)
			{
				*pro->add_m_chatlist() = item;
			}
			send_allclient(pro);
		}
		m_update_chatitem.clear();
		return true;
	}

	bool actor_chat::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl