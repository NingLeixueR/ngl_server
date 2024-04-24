#pragma once

#include "type.h"
#include "cmd.h"

namespace ngl
{
	enum enum_event
	{
		enum_event_death,
	};

	class event_parm
	{};

	class event_parm_death : public event_parm
	{
	public:
		int64_t m_unitid;		// µ¼ÖÂËÀÍöµÄunitid
		int64_t m_deathunitid;	// ËÀÍöµÄunitid
	};

	class event_tar
	{};

	template <enum_event ENUM, typename TEVEN_PARM>
	class event
	{
		using event_cmd = cmd<event_tar, enum_event, event_parm*>;
		static std::function<void(TEVEN_PARM*)> m_fun;
	public:
		static void init()
		{
			event_cmd::push(ENUM, [](const event_parm* aparm)
				{
					TEVEN_PARM* lparm = (TEVEN_PARM*)aparm;
					m_fun(lparm);
					return true;
				}
			);
		}

		static void push(const std::function<void(TEVEN_PARM*)>& afun)
		{
			m_fun = afun;
		}

		static bool execute(event_parm* aparm)
		{
			return event_cmd::function(ENUM, aparm);
		}
	};

	template <enum_event ENUM, typename TEVEN_PARM>
	std::function<void(TEVEN_PARM*)> event<ENUM, TEVEN_PARM>::m_fun;

	using event_death = event<ngl::enum_event_death, ngl::event_parm_death>;

	extern void init_event();
}// namespace ngl