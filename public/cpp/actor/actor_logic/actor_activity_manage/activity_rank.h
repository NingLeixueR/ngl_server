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
#include "ttab_activity_toprank.h"
#include "activity.h"
#include "actor.h"

namespace ngl
{
	class activity_rank : 
		public activity
	{
		activity_rank(const activity_rank&) = delete;
		activity_rank& operator=(const activity_rank&) = delete;

		virtual std::shared_ptr<activity> create(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb)
		{
			return std::make_shared<activity_rank>(aactivityid, atime, aduration, aactivitydb, aactivitytimedb);
		}
	public:
		//aactivityid		/*活动id*/
		//atime				/*开启时间*/
		//aduration			/*持续时间*/
		//aactivitydb		/*活动数据*/
		//aactivitytimedb	/*活动时间数据*/
		activity_rank(int32_t aactivityid, int32_t atime, int32_t aduration, activitydb& aactivitydb, activitytimedb& aactivitytimedb) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_rank():
			activity(EActivityTopRank)
		{}

		virtual void start();
		// 活动关闭
		virtual void finish();

		void handle(const np_get_rank_response& aresponse);
	};
}//namespace ngl