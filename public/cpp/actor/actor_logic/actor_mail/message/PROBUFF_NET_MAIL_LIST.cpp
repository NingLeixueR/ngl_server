#include "actor_mail.h"

namespace ngl
{
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_LIST>>& adata)
	{
		auto lparm = adata.get_data();
		const pbnet::PROBUFF_NET_MAIL_LIST* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		i64_actorid roleid = lparm->identifier();
		auto pro = m_mails.sync_mail(roleid);
		if (pro == nullptr)
		{
			return true;
		}
		send_client(roleid, pro);
		return true;
	}
}//namespace ngl