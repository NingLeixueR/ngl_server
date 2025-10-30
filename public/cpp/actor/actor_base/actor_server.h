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
ï»¿#pragma once

#include "actor_manage.h"
#include "naddress.h"
#include "net.h"

namespace ngl
{
	class actor_server : 
		public actor
	{
		actor_server(const actor_server&) = delete;
		actor_server& operator=(const actor_server&) = delete;

		actor_server();
	public:
		friend class actor_instance<actor_server>;
		static actor_server& instance()
		{
			return actor_instance<actor_server>::instance();
		}

		virtual ~actor_server() = default;

		static void nregister();

		static i64_actorid actorid();

		void print(const std::vector<i64_actorid>& avec)const;

		// # å›å¤/è½¬å‘[np_actornode_register]è¯·æ±‚
		void reply_np_actornode_register(const pack* apack, i32_serverid aserverid);
		void forward_np_actornode_register(
			const pack* apack, const nactornode& anode, i32_serverid aserverid, const std::vector<i64_actorid>& aadd
		);

		// # [actor client -> actor server] register
		// # æ³¨å†Œç»“ç‚¹
		bool handle(const message<np_actornode_register>& adata);

		// # æ›´æ–°ç»“ç‚¹ä¸­çš„actor
		bool handle(const message<np_actornode_update_server>& adata);

		//# å‘actorå®¢æˆ·ç«¯åŒæ­¥ç»“ç‚¹ä¿¡æ¯(ç¾¤å‘)
		bool handle(const message<np_actornode_update_mass>& adata);

		// # æ›´æ–°gatewayè¡¨ actor_role.guididä¸gateway server idå¯¹åº”å…³ç³»
		bool handle(const message<np_actor_gatewayid_updata>& adata);
	};
}//namespace ngl