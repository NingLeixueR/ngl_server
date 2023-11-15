#include "actor_role.h"
#include "actor_register.h"
#include "gameclient_forward.h"
#include "net.pb.h"
#include "actor_roleitem.h"

namespace ngl
{
	actor_role::actor_role(i16_area aarea, i32_actordataid aroleid, void* adata) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROLE,
					.m_area = aarea,
					.m_id = aroleid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			}),
		m_gatewayid(((actor_switch_process_role*)(adata))->m_gatewayid)
	{
		assert(aarea == ttab_servers::tab()->m_area);
	}

	i32_serverid actor_role::get_getwayserverid()
	{
		return m_gatewayid;
	}

	void actor_role::init()
	{
		m_info.set(this);
		m_bag.set(this);
		m_task.set(this);
	}

	actor_role::~actor_role() {}

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

	i64_actorid actor_role::roleid()
	{
		return m_info.get()->getconst().m_id();
	}

	void actor_role::sync_data_client()
	{
		auto pro = std::make_shared<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>();
		*pro->mutable_m_role() = m_info.get()->getconst();
		*pro->mutable_m_bag() = m_bag.get()->getconst();
		*pro->mutable_m_task() = m_task.get()->getconst();
		send2client(pro);
		LogLocalError("[sync]###[%]", m_info.get()->getconst().m_base().m_name());
	}

	bool actor_role::timer_handle(i32_threadid athread, const std::shared_ptr<pack>& apack, timerparm& adata)
	{
		return true;
	}
}