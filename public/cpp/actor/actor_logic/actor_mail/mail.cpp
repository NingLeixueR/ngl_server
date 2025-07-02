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
		{// Ò»¼üÁìÈ¡
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