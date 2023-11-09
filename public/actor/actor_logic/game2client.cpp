#include "gateway_game_forward.h"
#include "net.pb.h"

namespace ngl
{
	void gateway_game_forward::game2client()
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