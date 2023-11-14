#include "actor_role.h"
#include "actor_register.h"
#include "gameclient_forward.h"
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
		gameclient_forward::c2g();
	}

	void actor_role::loaddb_finish(bool adbishave)
	{
		LogLocalError("actor_role###loaddb_finish#[%]", actor_guid(id_guid()));
		sync_data_client();

		m_info.sync_actor_roleinfo();
	}

	void actor_role::handle_after()
	{
		// ### 同步这次消息的背包变动
		m_bag.sync_client();
	}

	bool actor_role::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}