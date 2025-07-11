#pragma once

#include "nactor_auto.h"
#include "db.pb.h"

namespace ngl
{
	class notice : 
		public tdb_notice::db_modular
	{
		notice(const notice&) = delete;
		notice& operator=(const notice&) = delete;

		int64_t m_maxid = -1;
		int64_t m_removetimer = -1;
	public:
		notice()
		{
		}

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		virtual void initdata()
		{
			log_error()->print("actor_notice###loaddb_finish {}", data());
			m_maxid = 0;
			//std::map<nguid, data_modified<TDATA>>
			
			for (std::pair<const nguid, data_modified<pbdb::db_notice>>& lpair : data())
			{
				const pbdb::db_notice& lnotice = lpair.second.getconst();
				m_maxid = std::max(m_maxid, lnotice.mid());
			}

			remove_notice();

			wheel_parm lparm
			{
				.m_ms = 10* localtime::MINUTES_SECOND,
				.m_intervalms = [](int64_t)
				{
					return 10 * localtime::MINUTES_SECOND;
				} ,
				.m_count = 0x7fffffff,
				.m_fun = [this](const wheel_node*)
				{
					remove_notice();
				}
			};
			m_removetimer = (int32_t)twheel::wheel().addtimer(lparm);
		}

		void add_notice(const std::string& anotice, int32_t abeg, int32_t aend)
		{
			pbdb::db_notice lnotice;
			lnotice.set_mid(++m_maxid);
			lnotice.set_mnotice(anotice);
			lnotice.set_mstarttime(abeg);
			lnotice.set_mfinishtime(aend);
			add(m_maxid, lnotice);

			auto pro = sync_notice(m_maxid);
			if (pro == nullptr)
			{
				return;
			}
			actor::send_client(pro);
		}

		void del_notice(int64_t aid)
		{
			erase(aid);
		}

		void remove_notice();

		std::shared_ptr<pbnet::PROBUFF_NET_NOTICE_RESPONSE> sync_notice(i64_actorid aactorid = -1);
	};
}//namespace ngl

mk_formatter(pbdb::db_notice)