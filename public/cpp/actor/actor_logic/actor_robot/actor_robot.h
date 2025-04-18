#pragma once

#include "actor_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_robot : public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
		std::string								m_kcpsessionmd5;
	public:
		i32_session								m_session;
		int16_t									m_kcp;
		// ----- Data End   -----
	public:
		actor_robot() = delete;
		actor_robot(const actor_robot&) = delete;
		actor_robot& operator=(const actor_robot&) = delete;

		actor_robot(i16_area aarea, i32_actordataid arobotid, void*);

		virtual ~actor_robot() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		virtual const char* kcp_session();

		bool timer_handle(const message<np_timerparm>& adata);
		bool handle(const message<np_arg_null>&);
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ERROR>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata);
	};
}//namespace ngl
	
