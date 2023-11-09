#include "gateway_game_forward.h"
#include "net.pb.h"

namespace ngl
{
	void gateway_game_forward::client2game()
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
}