/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ÏîÄ¿Ãû³Æ£ºngl_server
* ÏîÄ¿µØÖ·£ºhttps://github.com/NingLeixueR/ngl_server
* 
* ±¾ÎÄ¼şÊÇ ngl_server ÏîÄ¿µÄÒ»²¿·Ö£¬×ñÑ­ MIT ¿ªÔ´Ğ­Òé·¢²¼¡£
* Äú¿ÉÒÔ°´ÕÕĞ­Òé¹æ¶¨×ÔÓÉÊ¹ÓÃ¡¢ĞŞ¸ÄºÍ·Ö·¢±¾ÏîÄ¿£¬°üÀ¨ÉÌÒµÓÃÍ¾£¬
* µ«Ğè±£ÁôÔ­Ê¼°æÈ¨ºÍĞí¿ÉÉùÃ÷¡£
* 
* Ğí¿ÉÏêÇé²Î¼ûÏîÄ¿¸ùÄ¿Â¼ÏÂµÄ LICENSE ÎÄ¼ş£º
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
ï»¿#include "actor_gateway_g2c.h"
#include "nregister.h"
#include "nforward.h"

namespace ngl
{
	actor_gateway_g2c::actor_gateway_g2c() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GATEWAY_G2C,
					.m_area = tab_self_area,
					.m_id = nconfig::m_tcount,
					.m_manage_dbclient = false
				},
				.m_weight = 0x7fffffff
			})
	{
	}

	ENUM_ACTOR actor_gateway_g2c::actor_type()
	{
		return ACTOR_GATEWAY_G2C;
	}

	i64_actorid actor_gateway_g2c::actorid(i32_actordataid aactordataid)
	{
		return nguid::make(ACTOR_GATEWAY_G2C, tab_self_area, aactordataid);
	}

	void actor_gateway_g2c::init()
	{
		// ç»‘å®šDBç»“æ„:DB.set(this);

		// è®¾ç½®timer_handleå®šæ—¶å™¨
		/*np_timerparm tparm;
		if (make_timerparm::make_interval(tparm, 2) == false)
		{
			log_error()->print("actor_gateway_g2c::init() make_timerparm::make_interval(tparm, 2) == false!!!");
			return;
		}
		set_timer(tparm);
		*/
	}

	void actor_gateway_g2c::loaddb_finish(bool adbishave)
	{
	}

	void actor_gateway_g2c::nregister()
	{
		// å®šæ—¶å™¨
		actor::register_timer<actor_gateway_g2c>(&actor_gateway_g2c::timer_handle);

		// ç»‘å®šè‡ªå®šä¹‰np_æ¶ˆæ¯
		register_handle<actor_gateway_g2c>::func<
			np_actor_gatewayinfo_updata
		>(false);

		// ç»‘å®špbæ¶ˆæ¯
		nforward::g2c();
	}

	void actor_gateway_g2c::get_allclient(std::map<i32_sessionid, i64_actorid>& amap)
	{
		for (const auto& itemfor1 : m_info.info())
		{
			for (const auto& itemfor2 : itemfor1.second)
			{
				i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor2.second.m_area, itemfor2.second.m_dataid);
				amap.insert(std::make_pair(itemfor2.second.m_socket, lactorid));
			}
		}
	}

	void actor_gateway_g2c::get_allclientbyarea(std::map<i32_sessionid, i64_actorid>& amap, i16_area aarea)
	{
		auto itemitor = m_info.info().find(aarea);
		if (itemitor != m_info.info().end())
		{
			for (const auto& itemfor1 : itemitor->second)
			{
				i64_actorid lactorid = nguid::make(ACTOR_ROBOT, itemfor1.second.m_area, itemfor1.second.m_dataid);
				amap.insert(std::make_pair(itemfor1.second.m_socket, lactorid));
			}
		}
	}

	bool actor_gateway_g2c::timer_handle(const message<np_timerparm>& adata)
	{
		return true;
	}

	bool actor_gateway_g2c::handle(const message<np_arg_null>&)
	{
		return true;
	}
}//namespace ngl