#pragma once

#include "nactor_auto.h"
#include "db.pb.h"

namespace ngl
{
	class notice : public tdb_notice::db_modular
	{
		notice(const notice&) = delete;
		notice& operator=(const notice&) = delete;

		int64_t m_maxid;
	public:
		notice() :
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
			auto lstream = log_error();
			(*lstream) << "actor_notice###loaddb_finish" << std::endl;
			m_maxid = 0;
			for (const auto& [id, dbnotice] : *get_notice())
			{
				const pbdb::db_notice& lnotice = dbnotice.getconst();
				std::string lstart = localtime::time2str(lnotice.m_starttime(), "%y/%m/%d %H:%M:%S");
				std::string lfinish = localtime::time2str(lnotice.m_finishtime(), "%y/%m/%d %H:%M:%S");
				(*lstream)<< std::format(
					"notice###id:{} notice:{} start:{} finish:{}",
					lnotice.m_id(), lnotice.m_notice(), lstart, lfinish
				) << std::endl;
				m_maxid = std::max(m_maxid, lnotice.m_id());
			}
			(*lstream).print("");
		}

		void add_notice(const std::string& anotice, int32_t abeg, int32_t aend)
		{
			pbdb::db_notice lnotice;
			lnotice.set_m_id(++m_maxid);
			lnotice.set_m_notice(anotice);
			lnotice.set_m_starttime(abeg);
			lnotice.set_m_finishtime(aend);
			add(m_maxid, lnotice);

			auto pro = sync_notice(m_maxid);
			if (pro == nullptr)
				return;
			actor::send_allclient(pro);
		}

		void del_notice(int64_t aid)
		{
			erase(aid);
		}

		void remove_notice()
		{
			static int32_t lasttime = 0;
			// 十分钟刷新一次
			int32_t lnow = (int32_t)localtime::gettime();
			if (lasttime == 0 || lnow > (lasttime + (10 * localtime::MINUTES_SECOND)))
			{
				lasttime = lnow;
				std::map<nguid, data_modified<pbdb::db_notice>>& lnotice = data();
				for (const auto& [id, dbnotice] : lnotice)
				{
					const pbdb::db_notice& lnotice = dbnotice.getconst();
					if (lnotice.m_finishtime() < lnow && lnotice.m_finishtime() != -1)
					{
						remove(lnotice.m_id());
					}
				}
			}
		}

		std::shared_ptr<pbnet::PROBUFF_NET_NOTICE_RESPONSE> sync_notice(i64_actorid aactorid = -1);
	};
}//namespace ngl