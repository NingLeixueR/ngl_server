#pragma once

#include "actor_manage.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_robot : public actor
	{
		// ----- Data Begin -----
		pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE	m_data;
		std::string								m_kcpsessionmd5;
	public:
		i32_session								m_session;
		int16_t									m_kcp;
		// ----- Data End   -----
	public:
		actor_robot() = delete;
		actor_robot(const actor_robot&) = delete;
		actor_robot& operator=(const actor_robot&) = delete;

		actor_robot(i16_area aarea, i32_actordataid arobotid, void*);

		virtual ~actor_robot() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid(int32_t adataid);

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		static void nregister();

		virtual const char* kcp_session();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_BOMB>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_ERROR>& adata);
		// ƥ����
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER_INFO>& adata);
		// ����������Ϸ�ɹ�
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_CREATE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_ENTER_EXAMPLE_RESPONSE>& adata);
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN_RESPONSE>& adata);
		// ͬ��ƥ����Ϣ
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING>& adata);
		// ƥ����
		bool handle(const message<pbexample::PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT>& adata);
		// [�ظ�]��Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_ADDFRIEND_RESPONSE>& adata);
		// [�ظ�]��ȡ������Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata);
		// [�ظ�]���±�����Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata);
		// [�ظ�]���ų�ת�þ���
		bool handle(const message<pbnet::PROBUFF_NET_CEDE_FAMIL_RESPONSE>& adata);
		// [�ظ�]�޸ľ�������
		bool handle(const message<pbnet::PROBUFF_NET_CHANGE_FAMILNAME_RESPONSE>& adata);
		// [�ظ�]����
		bool handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		// [�ظ�]֪ͨ�ͻ��˳�ֵ����
		bool handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		// ������Ұ
		bool handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata);
		// [�ظ�]ɾ������
		bool handle(const message<pbnet::PROBUFF_NET_ERASEFRIEND_RESPONSE>& adata);
		// [����]������
		bool handle(const message<pbnet::PROBUFF_NET_ERROR>& adata);
		// [�ظ�]����ǩ��
		bool handle(const message<pbnet::PROBUFF_NET_FAMILSIGN_RESPONSE>& adata);
		// [�ظ�]��������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata);
		// [�ظ�]�������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata);
		// [�ظ�]��ȡ�����б�
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		// [�ظ�]��Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		// [�ظ�]������ʱ��
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		bool handle(const message<pbnet::PROBUFF_NET_JOIN_FAMIL_RESPONSE>& adata);
		// [�ظ�]kcp-session
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
		// [�ظ�]�뿪����
		bool handle(const message<pbnet::PROBUFF_NET_LEAVE_FAMIL_RESPONSE>& adata);
		// [�ظ�]��ȡ�ʼ�����
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata);
		// [�ظ�]��ȡ�ʼ�����
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata);
		// [�ظ�]��ȡ�ʼ��б�
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata);
		// [�ظ�]��ȡ�ʼ�
		bool handle(const message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata);
		// [�ظ�] �����������ظ��ˣ�һ������cmdЭ�飬��������һ���ַ�������
		bool handle(const message<pbnet::PROBUFF_NET_MSG_RESPONSE>& adata);
		// [�ظ�]��ȡ�����б�
		bool handle(const message<pbnet::PROBUFF_NET_NOTICE_RESPONSE>& adata);
		// [�ظ�]��ȡ���а�
		bool handle(const message<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>& adata);
		// [�ظ�]ͬ����Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_ADDFRIEND_RESPONSE>& adata);
		// [�ظ�]���ų�ͬ��/��ͬ����Ҽ������
		bool handle(const message<pbnet::PROBUFF_NET_RATIFY_JOIN_FAMIL_RESPONSE>& adata);
		// [�ظ�]��ֵ
		bool handle(const message<pbnet::PROBUFF_NET_RECHARGE_RESPONSE>& adata);
		// [�ظ�]��ϲ���x��Ʒ
		bool handle(const message<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>& adata);
		// [�ظ�]�������
		bool handle(const message<pbnet::PROBUFF_NET_ROLESTAT>& adata);
		// [�ظ�]��ȡ��ɫ��Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata);
		// [�ظ�]�л���·
		bool handle(const message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata);
		// ͬ������ֵ
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_ATTRIBUTE>& adata);
		// ͬ��λ�÷�����Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_POSITION>& adata);
		// ͬ�������Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_SYNC_UNIT>& adata);
		// [�ظ�]��ȡ������
		bool handle(const message<pbnet::PROBUFF_NET_TASK_RECEIVE_AWARD_RESPONSE>& adata);
	};
}//namespace ngl
	
