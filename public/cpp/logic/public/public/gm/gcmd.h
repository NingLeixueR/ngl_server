#pragma once

#include "define_json.h"
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
	template <typename T>
	struct gcmd
	{
		std::string m_operator;
		T m_data;
		int id;

		~gcmd()
		{
			ngl::ijson lwrite;
			lwrite.write("operator", m_operator);
			lwrite.write("data", m_data);

			auto pro = std::make_shared<mforward<ngl::np_gm_response>>(id);
			std::string ljson;
			lwrite.get(ljson);
			ngl::conversion::to_utf8(ljson, pro->add_data()->m_json);			
			actor::static_send_actor(nguid::make_self(ACTOR_GM), -1, pro);
		}

	};
}