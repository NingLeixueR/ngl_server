#include "nforward.h"
#include "net.pb.h"

namespace ngl
{
	// # 注册client到game的消息 
	// # 会注册gateway的转发与game的处理
	void nforward::c2g()
	{
		register_c2g<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_SWITCH_LINE
		>();

		//// ACTOR_MAIL 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_LIST
			, pbnet::PROBUFF_NET_MAIL_READ
			, pbnet::PROBUFF_NET_MAIL_DRAW
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// ACTOR_NOTICE 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// ACTOR_CHAT 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// ACTOR_FAMILY 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_FAMILY
			, pbnet::PROBUFF_NET_JOIN_FAMIL
			, pbnet::PROBUFF_NET_LEAVE_FAMIL
			, pbnet::PROBUFF_NET_FAMIL_LIST
			, pbnet::PROBUFF_NET_CHANGE_FAMILNAME
			, pbnet::PROBUFF_NET_CREATE_FAMIL
		>();

		// ACTOR_RANKLIST 模块二次转发
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();
	}
}// namespace ngl