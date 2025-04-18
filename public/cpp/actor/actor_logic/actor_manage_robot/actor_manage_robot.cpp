#include "actor_manage_robot.h"


namespace ngl
{
	actor_manage_robot::actor_manage_robot() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MANAGE_ROBOT,
					.m_area = tab_self_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_manage_robot::actor_type()
	{
		return ACTOR_MANAGE_ROBOT;
	}

	i64_actorid actor_manage_robot::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_manage_robot::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_chat::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_manage_robot::loaddb_finish(bool adbishave)
	{
	}

	void actor_manage_robot::nregister()
	{
		// 定时器
		actor::register_timer<actor_manage_robot>(&actor_manage_robot::timer_handle);

		// 绑定自定义np_消息
		register_handle_custom<actor_manage_robot>::func<
			np_robot_pram
		>(false);

		// 绑定pb消息
		register_handle_proto<actor_manage_robot>::func<
			pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE
		>(false);
	}

	std::shared_ptr<actor_robot> actor_manage_robot::create(i16_area aarea, i32_actordataid aroleid)
	{
		return std::dynamic_pointer_cast<actor_robot>(actor_base::create(ENUM_ACTOR::ACTOR_ROBOT, aarea, aroleid, nullptr));
	}

	void actor_manage_robot::login(const std::string& aaccount, const std::string& apasswold)
	{
		pbnet::PROBUFF_NET_ACOUNT_LOGIN pro;
		pro.set_m_area(tab_self_area);
		pro.set_m_account(aaccount);
		pro.set_m_password(apasswold);
		const tab_servers* tab = ttab_servers::tab();
		assert(tab != nullptr);
		nets::sendbyserver(tab->m_login, pro, nguid::moreactor(), getInstance().id_guid());
	}

	bool actor_manage_robot::check_connect(i32_serverid aserverid)const
	{
		return ttab_servers::tab(aserverid) != nullptr && ttab_servers::connect(aserverid) != nullptr;
	}

	void actor_manage_robot::connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const
	{
		if (check_connect(aserverid))
		{
			nets::connect(aserverid, afun, true, false);
		}
	}

	bool actor_manage_robot::parse_command(std::vector<std::string>& aparm)
	{
		auto ldata = std::make_shared<np_robot_pram>();
		ldata->m_parm.swap(aparm);
		i64_actorid lid = ngl::nguid::make(ACTOR_MANAGE_ROBOT, tab_self_area, nconfig::m_nodeid);
		handle_pram lparm = ngl::handle_pram::create<np_robot_pram, false, false>(lid, nguid::moreactor(), ldata);
		actor_manage::getInstance().push_task_id(lid, lparm, false);
		return true;
	}

	void actor_manage_robot::create_robot(const std::string& arobotname)
	{
		ngl::actor_manage_robot::login(arobotname, "123456");
	}

	void actor_manage_robot::create_robots(const std::string& arobotname, int abeg, int aend)
	{
		for (int i = abeg; i <= aend; ++i)
		{
			std::string lname(arobotname);
			lname += "";
			lname += tools::lexical_cast<std::string>(i);
			ngl::actor_manage_robot::login(lname, "123456");
		}
	}

	void actor_manage_robot::foreach(const std::function<bool(_robot&)>& afun)
	{
		for (std::pair<const std::string, _robot>& item : m_maprobot)
		{
			afun(item.second);
		}
	}

	_robot* actor_manage_robot::get_robot(std::string aacount)
	{
		auto itor = m_maprobot.find(aacount);
		if (itor == m_maprobot.end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	bool actor_manage_robot::getdata(_robot* arobot)
	{
		if (arobot == nullptr)
		{
			return false;
		}
		return true;
	}

	bool actor_manage_robot::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_manage_robot::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl