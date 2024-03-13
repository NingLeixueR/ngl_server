#include "nforward.h"
#include "net.pb.h"

namespace ngl
{
	//# client到game服务器 会注册gateway的转发与game的处理
	void nforward::c2g()
	{
		register_recvforward<EPROTOCOL_TYPE_PROTOCOLBUFF>(
			null<pbnet::PROBUFF_NET_GET_TIME>
			, null<pbnet::PROBUFF_NET_CMD>
			, null<pbnet::PROBUFF_NET_SWITCH_LINE>
		);

		// ACTOR_MAIL 模块二次转发
		register_recvforward2<EPROTOCOL_TYPE_PROTOCOLBUFF,ACTOR_MAIL>(
			null<pbnet::PROBUFF_NET_MAIL_LIST>
			, null<pbnet::PROBUFF_NET_MAIL_READ>
			, null<pbnet::PROBUFF_NET_MAIL_DRAW>
			, null<pbnet::PROBUFF_NET_MAIL_DEL>
		);

		// ACTOR_NOTICE 模块二次转发
		register_recvforward2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_NOTICE>(
			null<pbnet::PROBUFF_NET_GET_NOTICE>
		);

		// ACTOR_CHAT 模块二次转发
		register_recvforward2<EPROTOCOL_TYPE_PROTOCOLBUFF, ACTOR_CHAT>(
			null<pbnet::PROBUFF_NET_CHAT>
		);
	}

	// ### game到client服务器 会注册gateway的转发与client的处理
	void nforward::g2c()
	{
		register_forward<EPROTOCOL_TYPE_PROTOCOLBUFF>(
			null<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>
			, null<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>
			, null<pbnet::PROBUFF_NET_CHAT_RESPONSE>
			, null<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>
			, null<pbnet::PROBUFF_NET_GET_NOTICE_RESPONSE>
			, null<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>
			, null<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>
			, null<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>
			, null<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>
			, null<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>
			, null<pbnet::PROBUFF_NET_ERROR_RESPONSE>
		);
	}
}//namespace ngl