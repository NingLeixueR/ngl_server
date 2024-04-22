#pragma once

#include "ndb_modular.h"
#include "db.pb.h"

namespace ngl
{
	class actor_notice;
	using notice_db_modular = ndb_modular<
		EPROTOCOL_TYPE_PROTOCOLBUFF, 
		pbdb::ENUM_DB_NOTICE,
		pbdb::db_notice,
		actor_notice>;

	class notice : public notice_db_modular
	{
		int64_t m_maxid;
	public:
		notice() :
			notice_db_modular(),
			m_maxid(0)
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		std::map<nguid, data_modified<pbdb::db_notice>>* get_notice()
		{
			return &data();
		}

		virtual void initdata()
		{
			LogLocalError("actor_notice###loaddb_finish")
			m_maxid = 0;
			//std::map<nguid, data_modified<pbdb::db_notice>>& lnotice = data();
			for (auto&& [id, dbnotice] : *get_notice())
			{
				const pbdb::db_notice& lnotice = dbnotice.getconst();
				LogLocalError("notice###id:[%] notice:[%] time[%-%]", 
					lnotice.m_id(),
					lnotice.m_notice(),
					localtime::time2msstr(lnotice.m_starttime(), "%y-%m-%d %H:%M:%S"),
					localtime::time2msstr(lnotice.m_finishtime(), "%y-%m-%d %H:%M:%S")
				)
				if (m_maxid <= lnotice.m_id())
					m_maxid = lnotice.m_id();
			}
		}

		void add_notice(const std::string& anotice, int32_t abeg, int32_t aend)
		{
			pbdb::db_notice lnotice;
			lnotice.set_m_id(++m_maxid);
			lnotice.set_m_notice(anotice);
			lnotice.set_m_starttime(abeg);
			lnotice.set_m_finishtime(aend);
			add(m_maxid, lnotice);
		}

		void del_notice(int64_t aid)
		{
			erase(aid);
		}

		void remove_notice()
		{
			static int32_t lasttime = 0;
			// 十分钟刷新一次
			int32_t lnow = localtime::gettime();
			if (lasttime == 0 || lnow > (lasttime + (10 * 60)))
			{
				lasttime = lnow;
				std::map<nguid, data_modified<pbdb::db_notice>>& lnotice = data();
				for (auto&& [id, dbnotice] : lnotice)
				{
					if (dbnotice.getconst().m_finishtime() < lnow && dbnotice.getconst().m_finishtime() != -1)
					{
						remove(dbnotice.getconst().m_id());
					}
				}
			}
		}

		std::shared_ptr<pbnet::PROBUFF_NET_NOTICE_RESPONSE> sync_notice(i64_actorid aactorid = -1);
	};
}//namespace ngl