#pragma once

#include "ttab_specialid.h"
#include "actor_manage.h"
#include "actor_create.h"
#include "ndb_modular.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "manage_csv.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "ntimer.h"
#include "net.h"
#include "db.h"

#include "example.pb.h"
#include "net.pb.h"

namespace ngl
{
	class actor_example_match : public actor
	{
		actor_example_match(const actor_example_match&) = delete;
		actor_example_match& operator=(const actor_example_match&) = delete;

		actor_example_match();
	public:
		friend class actor_instance<actor_example_match>;
		static actor_example_match& getInstance()
		{
			return actor_instance<actor_example_match>::instance();
		}

		static ENUM_ACTOR actor_type()
		{
			return ACTOR_EXAMPLE_MATCH;
		}

		static i64_actorid actorid()
		{
			return nguid::make(ACTOR_EXAMPLE_MATCH, tab_self_area, nguid::none_actordataid());
		}

		virtual ~actor_example_match() = default;

		static void nregister()
		{
			// ��ʱ��
			actor::register_timer<actor_example_match>(&actor_example_match::timer_handle);
			// Э��ע��
			register_handle_proto<actor_example_match>::func<
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>,
				mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>
			>(false);

			register_handle_custom<actor_example_match>::func<
				np_login_request_info
			>(true);
		}

		virtual void loaddb_finish(bool adbishave) {}

		union nroomid
		{
		private:
			int32_t m_id;
			int16_t m_value[2];// m_value2[0] = type		m_value2[1] = room index
		public:
			nroomid(int32_t aid) :
				m_id(aid)
			{}

			nroomid(pbexample::EPLAY_TYPE atype, int16_t aindex)
			{
				m_value[0] = atype;
				m_value[1] = aindex;
			}

			int32_t roomid()
			{
				return m_id;
			}

			pbexample::EPLAY_TYPE type()
			{
				return (pbexample::EPLAY_TYPE)m_value[0];
			}

			int16_t index()
			{
				return m_value[1];
			}

			operator int32_t ()const
			{
				return m_id;
			}
		};

		struct player
		{
			i64_actorid		m_roleid = 0;
			bool			m_isconfirm = false; // �Ƿ�ȷ��
			int32_t			m_index = 0;
		};

		struct room
		{
			pbexample::EPLAY_TYPE			m_type = pbexample::EPLAY_TYPE::EPLAY_NULL;
			int32_t							m_roomid = 0;
			int32_t							m_totalnumber = 0;		// ��Ҫ��������
			std::map<i64_actorid, player>	m_players;				// ����ƥ������
			std::set<i64_actorid>			m_playersset;			// ����ƥ������
			time_t							m_roomcreate = 0;		// ���䴴��ʱ��
			time_t							m_roomready = 0;		// �������ʱ��
			int32_t							m_index = 0;			// ���뷿������
		};

		std::map<i64_actorid, int32_t>				m_matching;				// key:roleid value:room id
		struct room_index
		{
			int32_t				m_index = 0;
			enum eroom_stat
			{
				eroom_matching,		// ��δ��ƥ����
				eroom_ready,		// ��������
			};
			std::map<int32_t, eroom_stat>	m_roomlist;
			std::list<int32_t>				m_readyroomlist;	// ���������б�(˳���б�)
		};
		std::map<pbexample::EPLAY_TYPE, room_index>	m_roomindex;			// ���������
										/////////roomid////////
		std::map<pbexample::EPLAY_TYPE, std::map<int32_t, room>>		m_room;

		// # ͬ��������Ϣ
		void sync_match_info(room* aroom, i64_actorid aroleid = nguid::make());

		// # ͬ��[PROBUFF_EXAMPLE_PLAY_MATCHING_RESULT]
		void sync_response(room* aroom, pbexample::PLAY_EERROR_CODE acode, i64_actorid aroleid = nguid::make());

		// # �����Ƿ���������
		bool room_count_ready(room* aroom);

		// # ���������������ѡ��ƥ�䷿��
		room* matching_room(i64_actorid aroleid, pbexample::EPLAY_TYPE atype);

		// # ���ҷ���
		room* find_room(int32_t aroomid);

		// # ���ҷ���
		room* find_room(pbexample::EPLAY_TYPE atype, int32_t aroomid);

		// # ���ӷ���
		room* add_room(pbexample::EPLAY_TYPE atype);

		// # ɾ������
		void erase_room(room* aroom, pbexample::PLAY_EERROR_CODE aerrorcode = pbexample::PLAY_EERROR_CODE::EERROR_CODE_ROOM_DESTORY);

		// # ���ȡ��ƥ��
		void erase_player_room(room* aroom, i64_actorid aroleid);

		virtual void init();

		// # ����Ƿ�ʱ
		bool check_timeout(time_t atime, int32_t ainterval);

		// # ƥ��ɹ�
		void matching_finish(room* aroom);

		// # ������
		bool check_ready(room* aroom);

		bool timer_handle(const message<timerparm>& adata);

		// # ����ƥ��
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_JOIN>>& adata);
		
		// # ȷ��׼���ý���������Ϸ
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_PLAYER_CONFIRM>>& adata);

		// # ������ߺ�����ƥ����Ϣ
		bool handle(const message<np_login_request_info>& adata);

		// # ȡ��ƥ��
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_PLAY_CANCEL>>& adata);
	};
}//namespace ngl