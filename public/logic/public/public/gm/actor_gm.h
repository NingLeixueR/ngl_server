#pragma once

#include "actor_manage.h"
#include "net.h"
#include "db_data.h"
#include "db.h"
#include "db_pool.h"
#include "db_manage.h"
#include "actor_db_client.h"
#include "actor_protocol.h"
#include "actor_timer.h"
#include "actor_create.h"
#include "db_modular.h"
#include "notice.h"
#include "splite.h"
#include "manage_csv.h"
#include "gm.pb.h"
#include "init_protobuf.h"

namespace ngl
{
	class actor_gm : public actor
	{
		actor_gm();
	public:
		friend class actor_instance<actor_gm>;
		static actor_gm& getInstance()
		{
			return actor_instance<actor_gm>::instance();
		}

		virtual void init(){}

		static void actor_register();

		virtual ~actor_gm() {}

		virtual void loaddb_finish(bool adbishave) {}

		enum { ACTOR_TYPE = ACTOR_GM};


		template <typename T>
		void sendphpclient(mforward<T>& adata)
		{
			send(adata.identifier(), *adata.data(), actor_guid::make(), actor_guid::make());
		}

		template <typename T>
		void send2other(ENUM_ACTOR atype, const std::shared_ptr<pack>& apack, T& apro)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(apack->m_id, apro));
			send_actor(actor_guid::make_self(atype), pro);
		}

#define foward_module(MODULE, TYPE)														\
	bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, TYPE& adata)	\
	{																					\
		send2other(MODULE, apack, adata);												\
		return true;																	\
	}

#define foward_module_ret(MODULE, TYPE, RESPONSE) \
		foward_module(MODULE,TYPE) \
		bool handle(i32_threadid athread, const std::shared_ptr<pack>& apack, mforward<RESPONSE>& adata)\
		{\
			sendphpclient(adata);\
			return true;\
		}

		foward_module_ret(ACTOR_NOTICE, GM::PROBUFF_GM_GET_NOTICE, GM::PROBUFF_GM_GET_NOTICE_RESPONSE)
		foward_module_ret(ACTOR_NOTICE, GM::PROBUFF_GM_ADD_NOTICE, GM::PROBUFF_GM_ADD_NOTICE_RESPONSE)
		foward_module_ret(ACTOR_NOTICE, GM::PROBUFF_GM_DEL_NOTICE, GM::PROBUFF_GM_DEL_NOTICE_RESPONSE)

		private:
	};
}