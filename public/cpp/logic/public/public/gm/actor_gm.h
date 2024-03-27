#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "manage_curl.h"
#include "manage_csv.h"
#include "ndb_modular.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "initproto.h"
#include "ntimer.h"
#include "notice.h"
#include "splite.h"
#include "gm.pb.h"
#include "net.h"
#include "db_manage.h"
#include "db_pool.h"
#include "db_data.h"
#include "db.h"

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

		static void nregister();

		virtual ~actor_gm() {}

		virtual void loaddb_finish(bool adbishave) {}

		//bool handle_role_fail(const message<GM::PROBUFF_GM_RECHARGE>& adata)
		//{
		//	// ### 玩家不在线 记录订单
		//	ngl::_http* lhttp = ngl::manage_curl::make_http();
		//	ngl::manage_curl::set_mode(*lhttp, ngl::ENUM_MODE_HTTP);
		//	ngl::manage_curl::set_type(*lhttp, ngl::ENUM_TYPE_GET);
		//	ngl::manage_curl::set_url(*lhttp, "http://127.0.0.1:800/pay/pay_update.php");

		//	std::stringstream lstream;
		//	lstream
		//		<< "orderid=" << adata.m_data->m_orderid()
		//		<< "&gm=0"
		//		<< "&stat=1";

		//	ngl::manage_curl::set_param(*lhttp, lstream.str());
		//	ngl::manage_curl::getInstance().send(lhttp);
		//	return true;
		//}

		void sendsign(ENUM_ACTOR atype, const pack* apack, ngl::np_gm& apro)
		{
			auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
			send_actor(nguid::make_self(atype), pro);
		}

		void sendnosign(i64_actorid aactorid, const pack* apack, ngl::np_gm& apro)
		{
			auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
			send_actor(aactorid, pro);
		}

		bool handle(message<ngl::np_gm>& adata);

		bool handle(message<mforward<ngl::np_gm_response>>& adata);
	};
}// namespace ngl