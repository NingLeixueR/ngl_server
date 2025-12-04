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

namespace ngl
{
	actor_mail::actor_mail() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_MAIL,
					.m_area = tab_self_area,
					.m_manage_dbclient = true
				},
				.m_weight = 0x7fffffff,
				.m_broadcast = true,
			})
	{
	}

	ENUM_ACTOR actor_mail::actor_type()
	{
		return ACTOR_MAIL;
	}

	i64_actorid actor_mail::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_mail::init()
	{
		// 绑定DB结构:DB.set(this);
		m_mails.set(this);

		// 设置timer_handle定时器
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_mail::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/

		m_drop.init(this, {});
	}

	void actor_mail::erase_actor_before()
	{
		m_drop.exit();
	}
	
	void actor_mail::loaddb_finish(pbdb::ENUM_DB atype, enum_dbstat astat)
	{
		if (atype != pbdb::ENUM_DB::ENUM_DB_ALL)
		{
			log_error()->print("actor_mail::loaddb_finish [{}:{}]", (int32_t)atype, (int32_t)astat);
			return;
		}
	}

	void actor_mail::nregister()
	{
		// 定时器
		actor::register_timer<actor_mail>(&actor_mail::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_mail>::func<
			np_actor_addmail
			, mforward<np_gm>
			, mforward<pbnet::PROBUFF_NET_MAIL_LIST>
			, mforward<pbnet::PROBUFF_NET_MAIL_READ>
			, mforward<pbnet::PROBUFF_NET_MAIL_DRAW>
			, mforward<pbnet::PROBUFF_NET_MAIL_DEL>
		>(nready::e_ready_all);
	}

	bool actor_mail::sendmail(i64_actorid aactorid, int32_t amailid, int32_t adropid, const std::string& aparm)
	{
		auto pro = std::make_shared<np_actor_addmail>();
		pro->m_roleid = aactorid;
		pro->m_tid = amailid;
		pro->m_dropid = adropid;
		pro->m_parm = aparm;
		actor::send_actor(actorid(nguid::area(aactorid)), nguid::make(), pro);
		return true;
	}

	bool actor_mail::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_mail::handle(const message<np_arg_null>&)
	{
		return true;
	}
}// namespace ngl