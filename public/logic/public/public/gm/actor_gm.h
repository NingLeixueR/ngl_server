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
#include "manage_curl.h"

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
		void send2other(ENUM_ACTOR atype, const pack* apack, T& apro)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(apack->m_id, apro));
			send_actor(actor_guid::make_self(atype), pro);
		}

		template <typename T>
		void send2role(const pack* apack, T& apro, const std::function<void()>& afailfun)
		{
			std::shared_ptr<mforward<T>> pro(new mforward<T>(apack->m_id, apro));
			send_actor(apro.m_roleid(), pro, afailfun);
		}

		template <ENUM_ACTOR MODULE,typename T>
		bool handle(message<T>& adata)
		{
			send2other(MODULE, adata.m_pack, *adata.m_data);
			return true;
		}

		template <typename T>
		bool handle_role_fail(const message<T>& adata)
		{
			return true;
		}

		bool handle_role_fail(const message<GM::PROBUFF_GM_RECHARGE>& adata)
		{
			// ### 玩家不在线 记录订单
			ngl::_http* lhttp = ngl::manage_curl::make_http();
			ngl::manage_curl::set_mode(*lhttp, ngl::ENUM_MODE_HTTP);
			ngl::manage_curl::set_type(*lhttp, ngl::ENUM_TYPE_GET);
			ngl::manage_curl::set_url(*lhttp, "http://127.0.0.1:800/pay/pay_update.php");

			std::stringstream lstream;
			lstream
				<< "orderid=" << adata.m_data->m_orderid()
				<< "&gm=0"
				<< "&stat=1";

			ngl::manage_curl::set_param(*lhttp, lstream.str());
			ngl::manage_curl::getInstance().send(lhttp);
			return true;
		}

		// 发送给actor_role
		template <typename T>
		bool handle_role(message<T>& adata)
		{
			// ### actor_role 可能不在线
			send2role(adata.m_pack, *adata.m_data, [this, adata]()
				{
					handle_role_fail(adata);
				});
			return true;
		}
		
		template <typename T>
		bool handle(message<mforward<T>>& adata)
		{
			sendphpclient(*adata.m_data);
			return true;
		}

		private:
	};
}