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
	struct thruput
	{
		std::atomic<int32_t> m_count = 0;
		int32_t m_maxcount = 0;
		time_t m_beg = 0;
		time_t m_end = 0;
	};
	class test_thruput
	{
		std::vector<thruput*> m_rounds;
	public:
		static test_thruput& instance()
		{
			static test_thruput ltemp;
			return ltemp;
		}

		void change_rounds(int arounds)
		{
			m_rounds[arounds]->m_count.fetch_add(1, std::memory_order_relaxed);
			if (m_rounds[arounds]->m_count >= m_rounds[arounds]->m_maxcount)
			{
				m_rounds[arounds]->m_end = ngl::time_wheel::getms();
				test_thruput::instance().print(arounds);
			}
		}

		void add_rounds(int32_t aactorcount, int32_t aeverycount)
		{
			m_rounds.push_back(new thruput
			{
				.m_count = 0,
				.m_maxcount = aactorcount * aeverycount,
				.m_beg = ngl::time_wheel::getms(),
				.m_end = 0
			});

			auto pro = std::make_shared<np_thruput_test>();
			pro->m_rounds = (int32_t)m_rounds.size() - 1;
			for (int i = 0; i < aeverycount; ++i)
			{
				actor::send_actor(ACTOR_ROBOT, pro);
			}
		}

		void print(int arounds)
		{
			std::cout << std::format("rounds:{} m_count:{} m_maxcount:{} beg:{} end:{} space:{}\n", 
				arounds, m_rounds[arounds]->m_count.load(), 
				m_rounds[arounds]->m_maxcount, 
				m_rounds[arounds]->m_beg, m_rounds[arounds]->m_end, 
				m_rounds[arounds]->m_end - m_rounds[arounds]->m_beg
			);
		}

		void release()
		{
			for (int32_t i = 0; i < m_rounds.size(); ++i)
			{
				delete m_rounds[i];
			}
			m_rounds.clear();
		}
	};

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
		bool handle(const message<np_thruput_test>& adata);
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
		// [�ظ�]��ȡ������Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_BAG_SYNC_RESPONSE>& adata);
		// [�ظ�]���±�����Ϣ
		bool handle(const message<pbnet::PROBUFF_NET_BAG_UPDATE>& adata);
		// [�ظ�]����
		bool handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata);
		// [�ظ�]֪ͨ�ͻ��˳�ֵ����
		bool handle(const message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata);
		// ������Ұ
		bool handle(const message<pbnet::PROBUFF_NET_ENTER_LEAVE_VIEW>& adata);
		// [����]������
		bool handle(const message<pbnet::PROBUFF_NET_ERROR>& adata);
		// [�ظ�]���ų�ת�þ���
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CEDE_RESPONSE>& adata);
		// [�ظ�]�޸ľ�������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CHANGENAME_RESPONSE>& adata);
		// [�ظ�]��������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_CREATE_RESPONSE>& adata);
		// [�ظ�]��������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_INFO_RESPONSE>& adata);
		// [�ظ�]�������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_JOIN_RESPONSE>& adata);
		// [�ظ�]�뿪����
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LEAVE_RESPONSE>& adata);
		// [�ظ�]��ȡ�����б�
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_LIST_RESPONSE>& adata);
		// [�ظ�]���ų�ͬ��/��ͬ����Ҽ������
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_RATIFY_JOIN_RESPONSE>& adata);
		// [�ظ�]����ǩ��
		bool handle(const message<pbnet::PROBUFF_NET_FAMIL_SIGN_RESPONSE>& adata);
		// [�ظ�]��Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_ADD_RESPONSE>& adata);
		// [�ظ�]ɾ������
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_ERASE_RESPONSE>& adata);
		// [�ظ�]ͬ����Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RATIFY_ADD_RESPONSE>& adata);
		// [�ظ�]��Ӻ���
		bool handle(const message<pbnet::PROBUFF_NET_FRIEND_RESPONSE>& adata);
		// [�ظ�]������ʱ��
		bool handle(const message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata);
		// [�ظ�]kcp-session
		bool handle(const message<pbnet::PROBUFF_NET_KCPSESSION_RESPONSE>& adata);
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
