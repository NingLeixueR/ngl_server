#include "gameclient_forward.h"
#include "net.pb.h"

namespace ngl
{
	void gameclient_forward::c2g()
	{
		register_recvforward<EPROTOCOL_TYPE_PROTOCOLBUFF>(
			null<pbnet::PROBUFF_NET_GET_TIME>
			, null<pbnet::PROBUFF_NET_CHAT>
			, null<pbnet::PROBUFF_NET_CMD>
			, null<pbnet::PROBUFF_NET_SWITCH_LINE>
			, null<pbnet::PROBUFF_NET_GET_NOTICE>
			, null<pbnet::PROBUFF_NET_MAIL_LIST>
			, null<pbnet::PROBUFF_NET_MAIL_READ>
			, null<pbnet::PROBUFF_NET_MAIL_DRAW>
			, null<pbnet::PROBUFF_NET_MAIL_DEL>
		);
	}


	void gameclient_forward::g2c()
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
		);
	}
}