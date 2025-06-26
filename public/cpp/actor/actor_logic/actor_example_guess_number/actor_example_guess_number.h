#pragma once

#include "ttab_specialid.h"
#include "actor_example.h"
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
	enum eactor_example_guess_number
	{
		guess_number_esetp_maxtime = 30, // ������ִ�е��������
		guess_number_equit_time = 10,	// �˳����������
		guess_number_max_number = 101,
		guess_number_min_number = 0,
	};

	class actor_example_guess_number : 
		public actor_example<actor_example_guess_number>
	{
		actor_example_guess_number(const actor_example_guess_number&) = delete;
		actor_example_guess_number& operator=(const actor_example_guess_number&) = delete;

		int32_t m_maxnumber		= 1000;	// ��Χ���ֵ
		int32_t m_minnumber		= 0;	// ��Χ��Сֵ
		int32_t m_index			= 0;	// �²�����(����)
		int32_t m_setputc		= 0;	// ����ʱ��
		int32_t m_bombvalues	= 0;	// ը��ֵ
		int32_t m_bombutc		= 0;	// ����ը��ֵ��ʱ��
		i64_actorid m_bombrole	= 0;	// ����ը��ֵ�����
	public:
		actor_example_guess_number(const std::map<int32_t, i64_actorid>& aroleids, int32_t aindex);

		virtual ~actor_example_guess_number() = default;

		static ENUM_ACTOR actor_type();

		static i64_actorid actorid();

		virtual void init();

		virtual void loaddb_finish(bool adbishave);

		void next_setp();

		void bomb(i64_actorid aroleid = nguid::make());

		bool isfinish();

		void set_finish(i64_actorid abombrole);

		i64_actorid next_guess_role();

		void send_error(i64_actorid aroleid, pbexample::E_GUESS_NUMBER_ERROR aerr);

		static void nregister();

		bool timer_handle(const message<np_timerparm>& adata);

		bool handle(const message<np_arg_null>&);
		bool handle(const message<mforward<pbexample::PROBUFF_EXAMPLE_GUESS_NUMBER>>& adata);
		bool handle(const message<np_example_entergame_ready>& adata);
	};

}//namespace ngl
