#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		const pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE& pro = *adata.get_data();
		char lbuff[1024] = { 0 };
		auto lstream = log_error();
		for (const auto& [_id, _mail] : pro.m_mail())
		{
			(*lstream) << "[####################################################################]" << std::endl;
			(*lstream) << "[mailid:	" << _mail.m_id() << "]" << std::endl;
			std::string lcontent;
			tools::to_asscii(_mail.m_content(), lcontent);
			(*lstream) << "[content:	" << lcontent << "]" << std::endl;
			ngl::localtime::time2str(lbuff, 1024, _mail.m_createutc(), "%y/%m/%d %H:%M:%S");
			(*lstream) << "[createutc:	" << lbuff << "]" << std::endl;
			(*lstream) << "[read:		" << (_mail.m_read() ? "yes" : "no") << "]" << std::endl;
			(*lstream) << "[draw:		" << (_mail.m_draw() ? "yes" : "no") << "]" << std::endl;
			(*lstream) << "[####################################################################]" << std::endl;
		}
		(*lstream).print("");
		return true;
	}
}//namespace ngl