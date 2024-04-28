#include "actor_matching.h"
#include "localtime.h"
#include "matching.h"
#include "net.pb.h"
#include "type.h"

#include <forward_list>
#include <list>
#include <map>

namespace ngl
{
	actor_matching::actor_matching() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_MATCHING,
						.m_area = ttab_servers::tab()->m_area
					},
				.m_weight = 0x7fffffff,
			})
	{
		type_roleitems::init(
			[](std::map<i64_actorid, pbdb::db_brief>& amap, pbdb::db_brief& ainfo)
			{
				amap[ainfo.m_id()] = ainfo;
			}
		);
	}

	void actor_matching::nregister()
	{
		// 定时器
		actor::register_timer<actor_matching>(&actor_matching::timer_handle);
		register_actor_handle<EPROTOCOL_TYPE_PROTOCOLBUFF
			, actor_matching
			, mforward<pbnet::PROBUFF_NET_MATCHING>
			, mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>
			, mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>
			, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE
		>(true);
	}

	bool actor_matching::handle(message<mforward<pbnet::PROBUFF_NET_MATCHING>>& adata)
	{
		auto lparm = adata.m_data;
		pbnet::PROBUFF_NET_MATCHING* lpdata = lparm->data();
		m_matching.add_member(lpdata->m_type(), lpdata->m_tid(), lparm->identifier());
		return true;
	}
	
	bool actor_matching::handle(message<mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>>& adata)
	{
		auto lparm = adata.m_data;
		pbnet::PROBUFF_NET_MATCHING_CANCEL* lpdata = lparm->data();

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_CANCEL_RESPONSE>();
		pro->set_m_type(lpdata->m_type());

		pbnet::ematching_cancel_stat lstat = m_matching.try_cancel(lpdata->m_type(), lparm->identifier());
		pro->set_m_stat(lstat);
		send_client(lparm->identifier(), pro);
		return true;
	}
	
	bool actor_matching::handle(message<mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>>& adata)
	{
		auto lparm = adata.m_data;
		pbnet::PROBUFF_NET_MATCHING_CONFIRM* lpdata = lparm->data();
		pbnet::ematching_confirm_stat lstat = m_matching.confirm(lpdata->m_roomid(), lparm->identifier());

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_CONFIRM_RESPONSE>();
		pro->set_m_stat(lstat);
		pro->set_m_type(lpdata->m_type());
		send_client(lparm->identifier(), pro);
		return true;
	}
	
	bool actor_matching::handle(message<pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE>& adata)
	{
		auto lparm = adata.m_data;
		// 删除房间
		m_matching.remove_success(lparm->m_type(), lparm->m_roomid(), lparm->m_playsactorid());
		return true;
	}
	
	bool actor_matching::timer_handle(message<timerparm>& adata)
	{
		m_matching.do_match();
		return true;
	}
}// namespace ngl