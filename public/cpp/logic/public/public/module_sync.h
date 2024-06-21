#pragma once

#include "nprotocol.h"
#include "actor.h"

namespace ngl
{
	// # 辅助某个actor的db数据发生变化需要同步给其他actor
	template <typename DB_TAB>
	class module_db_sync
	{
		actor* m_actor;
	public:
		module_db_sync(actor* aactor)
		void sync(std::shared_ptr<np_module_sync<DB_TAB>>& apro)
		{
			actor::static_send_actor()
			actor::static_send_actor();
		}
	};


}// namespace ngl