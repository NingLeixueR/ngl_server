#pragma once

#include "actor_robot.h"

#include <string>
#include <vector>

enum ROBOT_CMD
{
	ROBOT_CMD_CREATE_ROBOT, // 创建机器人
};

namespace ngl
{
	class robot_cmd
	{
	public:
		static bool parse_command(std::vector<std::string>& aparm)
		{
			std::shared_ptr<np_robot_pram> ldata(new np_robot_pram());
			ldata->m_parm.swap(aparm);
			i64_actorid lid = ngl::nguid::make(ACTOR_MANAGE_ROBOT, tab_self_area, nconfig::m_nodeid);
			handle_pram lparm;
			ngl::handle_pram::create<np_robot_pram, false, false>(lparm, lid, nguid::moreactor(), ldata);
			actor_manage::getInstance().push_task_id(lid, lparm, false);
			return true;
		}
	};
}//namespace ngl