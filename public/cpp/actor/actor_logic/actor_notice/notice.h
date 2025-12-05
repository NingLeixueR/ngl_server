/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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
		notice() = default;

		void related_actorid() final
		{
			set_actorid(nguid::make());
		}

		virtual void initdata()
		{
			log_error()->print("actor_notice###loaddb_finish {}", data());
			m_maxid = 0;

			for (const auto& lpair : data())
			{
				const pbdb::db_notice* lpdata = lpair.second.getconst();
				if (lpdata == nullptr)
				{
					continue;
				}
				m_maxid = std::max(m_maxid, lpdata->mid());
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

			pbnet::PROBUFF_NET_NOTICE_RESPONSE pro;
			if (!sync_notice(m_maxid, pro))
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

		bool sync_notice(i64_actorid aactorid, pbnet::PROBUFF_NET_NOTICE_RESPONSE& apro);
	};
}//namespace ngl

mk_formatter(pbdb::db_notice)