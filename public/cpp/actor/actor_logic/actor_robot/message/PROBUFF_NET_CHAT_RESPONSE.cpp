#include "actor_robot.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	bool actor_robot::handle(const message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.get_data();
		if (lrecv->m_type() == pbnet::get_chat_list)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		else if (lrecv->m_type() == pbnet::chat_speak)
		{
			//log_error()->print("{}", (lrecv->m_stat() ? "[发言成功]" : "[发言失败] "));
		}
		else if (lrecv->m_type() == pbnet::updata_speck)
		{
			char lbuff[1024] = { 0 };
			for (const auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				log_error()->print("[{}:{}:{}] {}", nguid::area(item.m_roleid()), item.m_rolename(), lbuff, item.m_content());
			}
		}
		return true;
	}

}//namespace ngl