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

#include "actor_example_manage.h"
#include "actor_brief.h"

namespace ngl
{
	template <typename TACTOR>
	class actor_example : public actor
	{
		actor_example(const actor_example&) = delete;
		actor_example& operator=(const actor_example&) = delete;
	protected:
		std::vector<i64_actorid> m_rolesds;
		pbexample::EPLAY_TYPE m_playertype;
	public:
		actor_example(pbexample::EPLAY_TYPE aplayertype, ENUM_ACTOR atype, int32_t aindex, const std::map<int32_t, i64_actorid>& aroleids) :actor(
			actorparm
			{
				.m_parm
					{
						.m_type = atype,
						.m_area = tab_self_area,
						.m_id = aindex
					},
				.m_weight = 0x7fffffff,
			}),
			m_playertype(aplayertype)
		{
			m_rolesds.reserve(aroleids.size());
			std::set<i64_actorid> lset;
			for (const std::pair<const int32_t, i64_actorid>& item : aroleids)
			{
				m_rolesds.push_back(item.second);
				lset.insert(item.second);
			}
			tdb_brief::nsp_cli<TACTOR>::init(actor_brief::actor_type(), (TACTOR*)this, lset);
		}

		virtual void erase_actor_before()
		{
			auto pro = std::make_shared<np_example_equit>();
			pro->m_exampleid = id_guid();
			pro->m_type = m_playertype;
			send_actor(actor_example_manage::actorid(), pro);
			tdb_brief::nsp_cli<TACTOR>::exit();
		}
	};
}