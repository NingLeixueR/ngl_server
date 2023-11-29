#include "actor_matching.h"
#include <map>
#include <list>
#include <forward_list>
#include "type.h"
#include "localtime.h"
#include "net.pb.h"
#include "matching.h"

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

	void actor_matching::actor_register()
	{
		// 定时器
		actor::register_timer<actor_matching>(&actor_matching::timer_handle);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_matching>(
			true
			, dregister_fun_handle(actor_matching, mforward<pbnet::PROBUFF_NET_MATCHING>)
			, dregister_fun_handle(actor_matching, mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>)
			, dregister_fun_handle(actor_matching, mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>)
			, dregister_fun_handle(actor_matching, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE)
		);
	}

	bool actor_matching::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MATCHING>& adata)
	{
		pbnet::PROBUFF_NET_MATCHING* lpdata = adata.data();
		m_matching.add_member(lpdata->m_type(), lpdata->m_tid(), adata.identifier());
		return true;
	}

	bool actor_matching::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MATCHING_CANCEL>& adata)
	{
		pbnet::PROBUFF_NET_MATCHING_CANCEL* lpdata = adata.data();

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_CANCEL_RESPONSE>();
		pro->set_m_type(lpdata->m_type());

		pbnet::ematching_cancel_stat lstat = m_matching.try_cancel(lpdata->m_type(), adata.identifier());
		pro->set_m_stat(lstat);
		send_client(adata.identifier(), pro);
		return true;
	}

	bool actor_matching::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<pbnet::PROBUFF_NET_MATCHING_CONFIRM>& adata)
	{
		pbnet::PROBUFF_NET_MATCHING_CONFIRM* lpdata = adata.data();
		pbnet::ematching_confirm_stat lstat = m_matching.confirm(lpdata->m_roomid(), adata.identifier());

		auto pro = std::make_shared<pbnet::PROBUFF_NET_MATCHING_CONFIRM_RESPONSE>();
		pro->set_m_stat(lstat);
		pro->set_m_type(lpdata->m_type());
		send_client(adata.identifier(), pro);
		return true;
	}

	bool actor_matching::handle(i32_threadid athread, const std::shared_ptr<pack>& apack, pbnet::PROBUFF_NET_MATCHING_SUCCESS_RESPONSE& adata)
	{
		// 删除房间
		m_matching.remove_success(adata.m_type(), adata.m_roomid(), adata.m_playsactorid());
		return true;
	}

	bool actor_matching::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		m_matching.do_match();
		return true;
	}
}