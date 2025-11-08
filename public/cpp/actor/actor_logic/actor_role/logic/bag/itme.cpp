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
#include "actor_role.h"
#include "itme.h"

namespace ngl
{
	std::map<EItemType, item_create*> item_create::m_data;

	// 材料
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

	// 金币
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

	// 装备
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

	// 卡牌
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