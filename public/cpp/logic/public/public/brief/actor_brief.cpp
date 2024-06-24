#include "actor_brief.h"
#include "net.pb.h"
#include "nsp_server.h"

namespace ngl
{
	actor_brief::actor_brief() :
		actor(
			actorparm
			{
				.m_parm
					{
						.m_type = ACTOR_BRIEF,
						.m_area = ttab_servers::tab()->m_area
					},
				.m_weight = 0x7fffffff,
			})
	{
		tdb_brief::nsp_ser::init(&m_briefdb);
	}

	void actor_brief::nregister()
	{
		// Ð­Òé×¢²á
		register_handle_custom<actor_brief>::func<
			np_actor_roleinfo
		>(false);
		
	}

	bool actor_brief::handle(message<np_actor_roleinfo>& adata)
	{
		std::vector<pbdb::db_brief>& linfovec = *adata.get_data()->m_vecinfo.m_data.get();
		m_briefdb.update(linfovec);

		std::vector<i64_actorid> lactoridvec;
		std::for_each(
			linfovec.begin(), linfovec.end(), 
			[&lactoridvec](pbdb::db_brief& abrief)
			{
				lactoridvec.push_back(abrief.m_id());
			});
		tdb_brief::nsp_ser::publish(lactoridvec);
		return true;
	}
}//namespace ngl