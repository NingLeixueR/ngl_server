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
	// # ע��game��client����Ϣ
	// # ��ע��gateway��ת����client�Ĵ���
	void nforward::g2c()
	{
		register_g2c<
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB,
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR,
			pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO,
			pbexample::PROBUFF_EXAMPLE_PLAY_CREATE,
			pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE,
			pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE,
			pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING,
			pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT,
			pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE,
			pbnet::PROBUFF_NET_BAG_UPDATE,
			pbnet::PROBUFF_NET_CHAT_RESPONSE,
			pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE,
			pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW,
			pbnet::PROBUFF_NET_ERROR,
			pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE,
			pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE,
			pbnet::PROBUFF_NET_FRIEND_RESPONSE,
			pbnet::PROBUFF_NET_GET_TIME_RESPONSE,
			pbnet::PROBUFF_NET_KCPSESSION_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE,
			pbnet::PROBUFF_NET_MAIL_READ_RESPONSE,
			pbnet::PROBUFF_NET_MSG_RESPONSE,
			pbnet::PROBUFF_NET_NOTICE_RESPONSE,
			pbnet::PROBUFF_NET_RANKLIST_RESPONSE,
			pbnet::PROBUFF_NET_RECHARGE_RESPONSE,
			pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE,
			pbnet::PROBUFF_NET_ROLESTAT,
			pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE,
			pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE,
			pbnet::PROBUFF_NET_SYNC_ATTRIBUTE,
			pbnet::PROBUFF_NET_SYNC_POSITION,
			pbnet::PROBUFF_NET_SYNC_UNIT,
			pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE
		>();
	}
}//namespace ngl