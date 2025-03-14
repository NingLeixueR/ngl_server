#include "example.pb.h"
#include "net.pb.h"

#include "nforward.h"

namespace ngl
{
	// # 注册game到client的消息
	// # 会注册gateway的转发与client的处理
	void nforward::g2c()
	{
		register_g2c<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_GET_TIME_RESPONSE
			, pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE
			, pbnet::PROBUFF_NET_CHAT_RESPONSE
			, pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE
			, pbnet::PROBUFF_NET_NOTICE_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_READ_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE
			, pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE
			, pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE
			, pbnet::PROBUFF_NET_ERROR
			, pbnet::PROBUFF_NET_CREATE_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE
			, pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE
			, pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE
			, pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE
			, pbnet::PROBUFF_NET_RANKLIST_RESPONSE
			, pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE
			, pbnet::PROBUFF_NET_FRIEND_RESPONSE
			, pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE
			, pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE
			, pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE
			, pbnet::PROBUFF_NET_ROLESTAT
			, pbnet::PROBUFF_NET_MSG_RESPONSE
		>();

		// # 例子游戏匹配
		register_g2c<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE
			, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING
			, pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT
		>();
	}
}//namespace ngl