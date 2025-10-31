/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
#pragma once
#include "ttab_activity_drawcompliance.h"
#include "activity.h"

namespace ngl
{
	class activity_drawcompliance : 
		public activity
	{
		activity_drawcompliance(const activity_drawcompliance&) = delete;
		activity_drawcompliance& operator=(const activity_drawcompliance&) = delete;

		virtual std::shared_ptr<activity> create(
			int32_t aactivityid
			, int32_t atime
			, int32_t aduration
			, activitydb& aactivitydb
			, activitytimedb& aactivitytimedb
		)
		{
			return std::make_shared<activity_drawcompliance>(
				aactivityid, atime, aduration, aactivitydb, aactivitytimedb
			);
		}

	public:
		activity_drawcompliance(
			int32_t aactivityid					/*�id*/
			, int32_t atime						/*����ʱ��*/
			, int32_t aduration					/*����ʱ��*/
			, activitydb& aactivitydb			/*�����*/
			, activitytimedb& aactivitytimedb	/*�ʱ������*/
		) :
			activity(aactivityid, atime, aduration, aactivitydb, aactivitytimedb)
		{}

		activity_drawcompliance():
			activity(EActivityDrawCompliance)
		{}

		// �����
		virtual void start()
		{
			return;
		}

		// ��ر�
		virtual void finish()
		{
			return;
		}

		void update(i64_actorid aroleid, int acount)
		{
			pbdb::db_activity* lpdbactivity = m_activity->get();
			if (lpdbactivity == nullptr)
			{
				return;
			}
			auto lmap = lpdbactivity->mutable_mdrawcompliance();
			auto itor = lmap->find(aroleid);
			if (itor == lmap->end())
			{
				pbdb::activity_drawcompliance ltemp;
				ltemp.set_mcount(acount);
				itor = lmap->insert({ aroleid, ltemp }).first;
			}

			for (const auto& [_id, _data] : ttab_activity_drawcompliance::instance().tablecsv())
			{
				if (itor->second.mcount() > _id)
				{
					auto itorreward = itor->second.mutable_mreward()->find(_id);
					if (itorreward == itor->second.mutable_mreward()->end())
					{
						// ����
						std::string lsrc = std::format(
							"activity_drawcompliance role=[{}] mail=[{}] drop=[{}]", 
							aroleid, _data.m_mailid, _data.m_dropid
						);
						if (actor_activity_manage::get_drop().use(_data.m_dropid, 1, aroleid, lsrc, nullptr, _data.m_mailid))
						{
							// ��¼����ȡ
							itor->second.mutable_mreward()->insert({ _id, true });
						}
					}
				}
			}
		}
	};
}//namespace ngl