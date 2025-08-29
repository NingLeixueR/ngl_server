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
	
	void actor_mail::loaddb_finish(bool adbishave)
	{
	}

	void actor_mail::nregister()
	{
		// 定时器
		actor::register_timer<actor_mail>(&actor_mail::timer_handle);

		// 绑定自定义np_消息
		register_handle<actor_mail>::func<
			np_actor_addmail
			, mforward<np_gm>
		>(true);

		// 绑定pb消息
		register_handle<actor_mail>::func<
			mforward<pbnet::PROBUFF_NET_MAIL_LIST>
			, mforward<pbnet::PROBUFF_NET_MAIL_READ>
			, mforward<pbnet::PROBUFF_NET_MAIL_DRAW>
			, mforward<pbnet::PROBUFF_NET_MAIL_DEL>
		>(true);
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