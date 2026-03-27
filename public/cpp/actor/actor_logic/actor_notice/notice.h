/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Declares interfaces for actor notice.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/generated/pb/db.pb.h"

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
		notice() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		virtual void initdata()
		{
			log_error()->print("actor_notice###loaddb_finish {}", data());
			m_maxid = 0;

			for (auto& lpair : data())
			{
				MODIFIED_CONTINUE_CONST(lpdata, lpair.second);
				m_maxid = std::max(m_maxid, lpdata->mid());
			}

			remove_notice();

			wheel_parm lparm
			{
				.m_ms = 10* tools::localtime::MINUTES_SECOND,
				.m_intervalms = [](int64_t)
				{
					return 10 * tools::localtime::MINUTES_SECOND;
				} ,
				.m_count = 0x7fffffff,
				.m_fun = [this](const wheel_node*)
				{
					remove_notice();
				}
			};
			m_removetimer = (int32_t)twheel::wheel().addtimer(lparm);
		}

		void notice_add(const std::string& anotice, int32_t abeg, int32_t aend)
		{
			pbdb::db_notice lnotice;
			lnotice.set_mid(++m_maxid);
			lnotice.set_mnotice(anotice);
			lnotice.set_mstarttime(abeg);
			lnotice.set_mfinishtime(aend);
			add(m_maxid, lnotice);

			pbnet::PROBUFF_NET_NOTICE_RESPONSE pro;
			if (!sync_notice(m_maxid, pro))
			{
				return;
			}
			actor::send_client(pro);
		}

		void notice_del(int64_t aid)
		{
			erase(aid);
		}

		void remove_notice();

		bool sync_notice(i64_actorid aactorid, pbnet::PROBUFF_NET_NOTICE_RESPONSE& apro);
	};
}//namespace ngl

mk_formatter(pbdb::db_notice)
