/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#include "example.pb.h"
#include "nforward.h"
#include "net.pb.h"

namespace ngl
{
	// # 注册client到game的消息 
	// # 会注册gateway的转发与game的处理
	void nforward::c2g()
	{
		register_c2g<
			pbnet::PROBUFF_NET_BAG_SYNC
			, pbnet::PROBUFF_NET_CHANGE_ANGLE
			, pbnet::PROBUFF_NET_CMD
			, pbnet::PROBUFF_NET_GET_TIME
			, pbnet::PROBUFF_NET_RECHARGE
			, pbnet::PROBUFF_NET_ROLE_CREATE
			, pbnet::PROBUFF_NET_ROLE_SYNC
			, pbnet::PROBUFF_NET_SWITCH_LINE
			, pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD
		>();

		// actor_chat 模块二次转发
		register_c2g_2< ACTOR_CHAT
			, pbnet::PROBUFF_NET_CHAT
		>();

		// actor_example_guess_number 模块二次转发
		register_c2g_2< ACTOR_EXAMPLE_GUESS_NUMBER
			, pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER
		>();

		// actor_example_manage 模块二次转发
		register_c2g_2< ACTOR_EXAMPLE_MANAGE
			, pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE
		>();

		// actor_example_match 模块二次转发
		register_c2g_2< ACTOR_EXAMPLE_MATCH
			, pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL
		>();

		// actor_family 模块二次转发
		register_c2g_2< ACTOR_FAMILY
			, pbnet::PROBUFF_NET_FAMIL_CEDE
		>();

		// actor_friends 模块二次转发
		register_c2g_2< ACTOR_FRIENDS
			, pbnet::PROBUFF_NET_FRIEND
		>();

		// actor_mail 模块二次转发
		register_c2g_2< ACTOR_MAIL
			, pbnet::PROBUFF_NET_MAIL_DEL
		>();

		// actor_notice 模块二次转发
		register_c2g_2< ACTOR_NOTICE
			, pbnet::PROBUFF_NET_NOTICE
		>();

		// actor_ranklist 模块二次转发
		register_c2g_2< ACTOR_RANKLIST
			, pbnet::PROBUFF_NET_RANKLIST
		>();

		// actor_testlua 模块二次转发
		register_c2g_2< ACTOR_TESTLUA
			, pbnet::PROBUFF_NET_TESTLUA
		>();

	}
}// namespace ngl