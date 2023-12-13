#include "actor_robot.h"
#include "actor_register.h"
#include "gameclient_forward.h"

namespace ngl
{
	actor_robot::actor_robot(i16_area aarea, i32_actordataid arobotid, void*) :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT,
					.m_area = aarea,
					.m_id = arobotid,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
			})
	{
		assert(aarea == ttab_servers::tab()->m_area);
	}

	void actor_robot::actor_register()
	{
		gameclient_forward::g2c();
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_ROLE_SYNC_RESPONSE>& adata)
	{
		Try
		{
			LogLocalError("[LOGIC_ROLE_SYNC:%:%]", adata.m_data->m_role().m_base().m_name(),  adata.m_data->m_role().m_base().m_lv());
			m_data = *adata.m_data;
		}Catch;
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_GET_TIME_RESPONSE>& adata)
	{
		char lbuff[1024] = { 0 };
		ngl::localtime::time2str(lbuff, 1024, adata.m_data->m_utc(), "%y/%m/%d %H:%M:%S");
		//LogLocalError("[%][%]", m_data.m_role().m_base().m_name(), lbuff);
		std::cout << m_data.m_role().m_base().m_name() << ":" << lbuff << std::endl;
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_CHAT_RESPONSE>& adata)
	{
		auto lrecv = adata.m_data;
		if (lrecv->m_type() == pbnet::get_chat_list)
		{
			char lbuff[1024] = { 0 };
			for (auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");

				//LogLocalError("[%][%][%] %", lbuff, 
				//	actor_guid::area(item.m_roleid()), item.m_rolename(), item.m_content());

				std::cout << actor_guid::area(item.m_roleid()) << ":" << item.m_rolename() << ":" << item.m_content() << std::endl;
			}
		}
		else if (lrecv->m_type() == pbnet::chat_speak)
		{
			//LogLocalError("%", (adata.m_stat() ? "[发言成功]" : "[发言失败] "));
			std::cout << (lrecv->m_stat() ? "[发言成功]" : "[发言失败] ") << std::endl;
		}
		else if (lrecv->m_type() == pbnet::updata_speck)
		{
			char lbuff[1024] = { 0 };
			for (auto& item : lrecv->m_chatlist())
			{
				ngl::localtime::time2str(lbuff, 1024, item.m_utc(), "%y/%m/%d %H:%M:%S");
				//LogLocalError("[%][%][%] %", lbuff,
				//	actor_guid::area(item.m_roleid()), item.m_rolename(), item.m_content());
				std::cout << actor_guid::area(item.m_roleid()) << ":" << item.m_rolename() << ":" << item.m_content() << std::endl;
			}
		}
		return true;
	}
	
	bool actor_robot::handle(message<pbnet::PROBUFF_NET_SWITCH_LINE_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_GET_NOTICE_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_LIST_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_READ_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_DRAW_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_MAIL_DEL_RESPONSE>& adata)
	{
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_DELIVER_GOODS_RECHARGE>& adata)
	{
		LogLocalError("发货成功！！！")
		return true;
	}

	bool actor_robot::handle(message<pbnet::PROBUFF_NET_ERROR_RESPONSE>& adata)
	{
		LogLocalError("[%][%]", adata.m_data->m_errnum(), adata.m_data->m_errmessage());
		return true;
	}

	void actor_manage_robot::actor_register()
	{
		register_actor<EPROTOCOL_TYPE_CUSTOM, actor_manage_robot>(
			false
			, dregister_fun_handle(actor_manage_robot, robot_pram)
		);

		register_actor<EPROTOCOL_TYPE_PROTOCOLBUFF, actor_manage_robot>(
			false
			, dregister_fun_handle(actor_manage_robot, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE)
		);
	}

}