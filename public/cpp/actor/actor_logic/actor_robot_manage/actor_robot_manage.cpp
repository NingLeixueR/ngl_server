#include "actor_robot_manage.h"


namespace ngl
{
	actor_robot_manage::actor_robot_manage() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_ROBOT_MANAGE,
					.m_area = tab_self_area,
					.m_id = nconfig::m_nodeid,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	ENUM_ACTOR actor_robot_manage::actor_type()
	{
		return ACTOR_ROBOT_MANAGE;
	}

	i64_actorid actor_robot_manage::actorid()
	{
		return nguid::make(actor_type(), tab_self_area, nguid::none_actordataid());
	}

	void actor_robot_manage::init()
	{
		// 绑定DB结构:DB.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_robot_manage::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_robot_manage::loaddb_finish(bool adbishave)
	{
	}

	void actor_robot_manage::nregister()
	{
		// 定时器
		actor::register_timer<actor_robot_manage>(&actor_robot_manage::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_robot_manage>::func<
			np_robot_pram
			, pbnet::PROBUFF_NET_ACOUNT_LOGIN_RESPONSE
		>(false);
	}

	std::shared_ptr<actor_robot> actor_robot_manage::create(i16_area aarea, i32_actordataid aroleid)
	{
		return std::dynamic_pointer_cast<actor_robot>(actor_base::create(ENUM_ACTOR::ACTOR_ROBOT, aarea, aroleid, nullptr));
	}

	void actor_robot_manage::login(const std::string& aaccount, const std::string& apasswold)
	{
		pbnet::PROBUFF_NET_ACOUNT_LOGIN pro;
		pro.set_marea(tab_self_area);
		pro.set_maccount(aaccount);
		pro.set_mpassword(apasswold);
		const tab_servers* tab = ttab_servers::instance().tab();
		if (tab == nullptr)
		{
			tools::no_core_dump();
			return;
		}

		nets::sendbyserver(tab->m_login, pro, nguid::moreactor(), instance().id_guid());
	}

	bool actor_robot_manage::check_connect(i32_serverid aserverid)const
	{
		return ttab_servers::instance().tab(aserverid) != nullptr && ttab_servers::instance().connect(aserverid) != nullptr;
	}

	void actor_robot_manage::connect(i32_serverid aserverid, const std::function<void(i32_sessionid)>& afun) const
	{
		if (check_connect(aserverid))
		{
			nets::connect(aserverid, afun, true, false);
		}
	}

	bool actor_robot_manage::parse_command(std::vector<std::string>& aparm)
	{
		auto ldata = std::make_shared<np_robot_pram>();
		ldata->m_parm.swap(aparm);
		i64_actorid lid = ngl::nguid::make(ACTOR_ROBOT_MANAGE, tab_self_area, nconfig::m_nodeid);
		handle_pram lparm = ngl::handle_pram::create<np_robot_pram, false, false>(lid, nguid::moreactor(), ldata);
		actor_manage::instance().push_task_id(lid, lparm);
		return true;
	}

	void actor_robot_manage::create_robot(const std::string& arobotname)
	{
		ngl::actor_robot_manage::login(arobotname, "123456");
	}

	void actor_robot_manage::create_robots(const std::string& arobotname, int abeg, int aend)
	{
		for (int i = abeg; i <= aend; ++i)
		{
			std::string lname(arobotname);
			lname += "";
			lname += tools::lexical_cast<std::string>(i);
			ngl::actor_robot_manage::login(lname, "123456");
		}
	}

	void actor_robot_manage::foreach(const std::function<bool(_robot&)>& afun)
	{
		for (std::pair<const std::string, _robot>& item : m_maprobot)
		{
			afun(item.second);
		}
	}

	_robot* actor_robot_manage::get_robot(std::string aacount)
	{
		auto itor = m_maprobot.find(aacount);
		if (itor == m_maprobot.end())
		{
			return nullptr;
		}
		return &itor->second;
	}

	bool actor_robot_manage::getdata(_robot* arobot)
	{
		if (arobot == nullptr)
		{
			return false;
		}
		return true;
	}

	bool actor_robot_manage::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_robot_manage::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl