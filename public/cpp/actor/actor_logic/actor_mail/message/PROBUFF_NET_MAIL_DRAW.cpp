#include "actor_mail.h"

namespace ngl
{
	bool actor_mail::handle(const message<mforward<pbnet::PROBUFF_NET_MAIL_DRAW>>& adata)
	{
		auto lparm = adata.get_data();
		const pbnet::PROBUFF_NET_MAIL_DRAW* lpdata = lparm->data();
		if (lpdata == nullptr)
		{
			return true;
		}
		pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE pro;
		pro.set_m_mailid(lpdata->m_mailid());
		pro.set_m_stat(m_mails.drawmail(lparm->identifier(), lpdata->m_mailid()));
		return true;
	}
}//namespace ngl