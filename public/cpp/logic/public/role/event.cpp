#include "event.h"
#include "map.h"

namespace ngl
{
	void init_event()
	{
		// 初始化为enum_event_death添加触发事件
		event_death::init();
		event_death::push([](ngl::event_parm_death* aparm)
			{
				std::cout << "ngl::event_parm_death" << std::endl;
			}
		);
	}
}