#pragma once

#include "type.h"
#include "cmd.h"

#include <set>

namespace ngl
{
	enum enum_event
	{
		enum_event_death,		// 死亡
		enum_event_enterview,	// 进入视野(enterview)
		enum_event_leaveview,	// 离开视野(leaveview)
	};

	class event_parm
	{
		event_parm() = delete;
	public:
		enum_event m_type;

		event_parm(enum_event aenum):
			m_type(aenum)
		{

		}
	};

	class event_parm_death : public event_parm
	{
	public:
		event_parm_death() :
			event_parm(enum_event_death)
		{}

		int64_t m_unitid;		// 导致死亡的unitid
		int64_t m_deathunitid;	// 死亡的unitid
	};

	//i64_actorid, std::set<i64_actorid>&
	class event_parm_leaveview : public event_parm
	{
	public:
		event_parm_leaveview() :
			event_parm(enum_event_leaveview)
		{}

		i64_actorid m_leaveunitid;			// 离开视野的unity
		std::set<i64_actorid> m_unitids;	// 都离开了谁的视野
	};

	class event_parm_enterview : public event_parm
	{
	public:
		event_parm_enterview() :
			event_parm(enum_event_enterview)
		{}

		i64_actorid m_enterunitid;			// 进入视野的unity
		std::set<i64_actorid> m_unitids;	// 都进入了谁的视野
	};

	class event_tar{};

	class events
	{
		using event_cmd = cmd<event_tar, enum_event, event_parm*>;
	public:
		static void init();

		template <typename TEVEN_PARM>
		static void push(enum_event atype, const std::function<void(TEVEN_PARM*)>& afun)
		{
			event_cmd::push(atype, [afun](const event_parm* aparm)
				{
					afun((TEVEN_PARM*)aparm);
				}
			);
		}

		static bool execute(event_parm* aparm)
		{
			return event_cmd::function(aparm->m_type, aparm);
		}
	};
}// namespace ngl