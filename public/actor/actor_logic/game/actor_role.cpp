#include "actor_role.h"
//#include "countgroup.h"
//#include "bag.h"
//#include "mail.h"
#include "actor_register.h"
#include "gateway_game_forward.h"
#include "net.pb.h"
#include "actor_roleitem.h"

namespace ngl
{
	void actor_role::actor_register()
	{
		// 定时器
		actor_role::register_timer<actor_role>();
		
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_role>(
			true,
			null<actor_disconnect_close>
			);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_role>(
			true,
			null<pbnet::PROBUFF_NET_ROLE_SYNC>
		);

		// 协议注册
		gateway_game_forward::client2game();
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_role###loaddb_finish#[%]", actor_guid(id_guid()));
		sync_data_client();

		{// 玩家信息需要同步到每个game和world
			m_info.sync_actor_roleinfo();
		}
		

		//// ## 活动签到
		//std::shared_ptr<actor_everydaysign> prosign(new actor_everydaysign());
		//send_actor(actor_guid::make_nonearea(ACTOR_ACTIVITY, enum_activity_everydaysign), prosign);
	}

	bool actor_role::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}