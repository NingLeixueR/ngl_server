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
#include "actor_mail.h"
#include "mail.h"

namespace ngl
{
	bool mails::drawmail(i64_actorid aroleid, int64_t aid)
	{
		if (aid != -1)
		{
			pbdb::mail* ltemp = get_mail(aroleid, aid);
			if (ltemp == nullptr)
			{
				return false;
			}
			if (ltemp->mdraw() == true)
			{
				return false;
			}
			auto pro = std::make_shared<np_actor_senditem>();
			for (int i = 0; i < ltemp->mitems_size(); ++i)
			{
				const pbdb::mailitem&  lmailitem = ltemp->mitems()[i];
				pro->m_item.insert({ lmailitem.mitemtid(),lmailitem.mcount() });
			}
			actor::send_actor(aroleid, get_actor()->id_guid(), pro);
		}
		else
		{// 一键领取
			one_touch(aroleid, [](const pbdb::mail& amail)
				{
					return amail.mdraw() == false;
				}, [aroleid, this](int32_t aid)
				{
					drawmail(aroleid, aid);
				});
		}
		return true;
	}
}//namespace ngl