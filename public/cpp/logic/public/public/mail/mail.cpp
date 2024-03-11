#include "mail.h"
#include "actor_mail.h"

namespace ngl
{
	bool mails::drawmail(int64_t aroleid, int64_t aid)
	{
		if (aid != -1)
		{
			pbdb::mail* ltemp = get_mail(aroleid, aid);
			if (ltemp == nullptr)
				return false;
			if (ltemp->m_draw() == true)
				return false;
			std::shared_ptr<np_actor_senditem> pro(new np_actor_senditem());
			for (int i = 0; i < ltemp->m_items_size(); ++i)
			{
				pro->m_item.insert({ ltemp->m_items()[i].m_itemtid(), ltemp->m_items()[i].m_count()});
			}
			actor_base::static_send_actor(aroleid, actor()->id_guid(), pro);
		}
		else
		{// Ò»¼üÁìÈ¡
			one_touch(aroleid, [aroleid, this](int32_t aid)
				{
					drawmail(aroleid, aid);
				});
		}
		return true;
	}
}