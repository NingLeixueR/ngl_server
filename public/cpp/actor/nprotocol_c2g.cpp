#include "nforward.h"
#include "net.pb.h"

namespace ngl
{
	// # ע��client��game����Ϣ 
	// # ��ע��gateway��ת����game�Ĵ���
	void nforward::c2g()
	{
		// actor_role�������Ϣ
		register_c2g<EPROTOCOL_TYPE_PROTOCOLBUFF
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_SWITCH_LINE
		>();

		// ����ת����Ҫ[tprotocol_forward_pb]���Զ�ע��Э���
		// 
		// ACTOR_MAIL ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_LIST
			, pbnet::PROBUFF_NET_MAIL_READ
			, pbnet::PROBUFF_NET_MAIL_DRAW
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// ACTOR_NOTICE ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// ACTOR_CHAT ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// ACTOR_FAMILY ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_FAMILY
			, pbnet::PROBUFF_NET_CREATE_FAMIL
			, pbnet::PROBUFF_NET_JOIN_FAMIL
			, pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL
			, pbnet::PROBUFF_NET_CEDE_FAMIL
			, pbnet::PROBUFF_NET_LEAVE_FAMIL
			, pbnet::PROBUFF_NET_FAMIL_LIST
			, pbnet::PROBUFF_NET_CHANGE_FAMILNAME
			, pbnet::PROBUFF_NET_FAMILSIGN
		>();

		// ACTOR_RANKLIST ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

		// ACTOR_RANKLIST ģ�����ת��
		register_c2g_2<EPROTOCOL_TYPE_PROTOCOLBUFF
			, ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
			, pbnet::PROBUFF_NET_ADDFRIEND
			, pbnet::PROBUFF_NET_RATIFY_ADDFRIEND
			, pbnet::PROBUFF_NET_ERASEFRIEND
		>();
	}
}// namespace ngl