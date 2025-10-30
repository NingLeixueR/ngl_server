/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "example.pb.h"
#include "net.pb.h"

#include "nforward.h"

namespace ngl
{
	// # ע��client��game����Ϣ 
	// # ��ע��gateway��ת����game�Ĵ���
	void nforward::c2g()
	{
		register_c2g<
			pbnet::PROBUFF_NET_BAG_SYNC
			, pbnet::PROBUFF_NET_CHANGE_ANGLE
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_RECHARGE
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_SWITCH_LINE
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>();

		// actor_chat ģ�����ת��
		register_c2g_2< ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// actor_example_guess_number ģ�����ת��
		register_c2g_2< ACTOR_EXAMPLE_GUESS_NUMBER
			, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
		>();

		// actor_example_manage ģ�����ת��
		register_c2g_2< ACTOR_EXAMPLE_MANAGE
			, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
		>();

		// actor_example_match ģ�����ת��
		register_c2g_2< ACTOR_EXAMPLE_MATCH
			, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
		>();

		// actor_family ģ�����ת��
		register_c2g_2< ACTOR_FAMILY
			, pbnet::PROBUFF_NET_FAMIL_CEDE
		>();

		// actor_friends ģ�����ת��
		register_c2g_2< ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
		>();

		// actor_mail ģ�����ת��
		register_c2g_2< ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// actor_notice ģ�����ת��
		register_c2g_2< ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// actor_ranklist ģ�����ת��
		register_c2g_2< ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

		// actor_testlua ģ�����ת��
		register_c2g_2< ACTOR_TESTLUA
			, pbnet::PROBUFF_NET_TESTLUA
		>();

	}
}// namespace ngl