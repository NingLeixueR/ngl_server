/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* Project name: ngl_server
* Project URL: https://github.com/NingLeixueR/ngl_server
* 
* This file is part of the ngl_server project and is distributed under the MIT License.
* You may use, modify, and distribute this project under the license, including commercial use,
* but you must retain the original copyright and license notice.
* 
* For license details, see the LICENSE file in the project root:
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
// File overview: Implements logic for bag.


#include "actor/actor_logic/actor_role/logic/bag/itme.h"
#include "actor/actor_logic/actor_role/actor_role.h"

namespace ngl
{
	std::map<EItemType, item_create*> item_create::m_data;

	// Translated comment.
	class item_create_material : 
		public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_mid(-1);
			litem.set_mcount(acount);
			litem.set_mtid(atid);

			avec.push_back(litem);
			return true;
		}
	public:
		item_create_material() :
			item_create(EItemType::EMaterial)
		{}
	};

	// Translated comment.
	class item_create_money : public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			switch (atid)
			{
			case EMoneyGold:
				arole->m_info.change_gold(acount);
				break;
			case EMoneySilver:
				arole->m_info.change_silver(acount);
				break;
			}			
			return true;
		}
	public:
		explicit item_create_money(EItemType amoney) :
			item_create(amoney)
		{}
	};

	// Equipment
	class item_create_equip : 
		public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_mid(-1);
			litem.set_mcount(acount);
			litem.set_mtid(atid);
			litem.set_mstar(1);
			litem.set_mlv(1);

			avec.push_back(litem);
			return true;
		}
	public:
		item_create_equip() :
			item_create(EItemType::EEquipment)
		{}
	};

	// Card
	class item_create_card : 
		public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_mid(-1);
			litem.set_mcount(acount);
			litem.set_mtid(atid);
			litem.set_mstar(1);
			litem.set_mlv(1);

			avec.push_back(litem);
			return true;
		}
	public:
		item_create_card() :
			item_create(EItemType::ECard)
		{}
	};

	const item_create_material	g_item_create_material;
	const item_create_money		g_item_create_moneygold(EMoneyGold);
	const item_create_money		g_item_create_moneysilver(EMoneySilver);
	const item_create_equip		g_item_create_equip;
	const item_create_card		g_item_create_card;
}// namespace ngl