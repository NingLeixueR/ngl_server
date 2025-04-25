#include "actor_role.h"
#include "itme.h"

namespace ngl
{
	std::map<EItemType, item_create*> item_create::m_data;

	// 材料
	class item_create_material : public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_m_id(-1);
			litem.set_m_count(acount);
			litem.set_m_tid(atid);

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
		item_create_money(EItemType amoney) :
			item_create(amoney)
		{}
	};

	// 装备
	class item_create_equip : public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_m_id(-1);
			litem.set_m_count(acount);
			litem.set_m_tid(atid);
			litem.set_m_star(1);
			litem.set_m_lv(1);

			avec.push_back(litem);
			return true;
		}
	public:
		item_create_equip() :
			item_create(EItemType::EEquipment)
		{}
	};

	// 卡牌
	class item_create_card : public item_create
	{
		virtual bool mode(actor_role* arole, int32_t atid, int32_t acount, std::vector<pbdb::item>& avec)
		{
			pbdb::item litem;
			litem.set_m_id(-1);
			litem.set_m_count(acount);
			litem.set_m_tid(atid);
			litem.set_m_star(1);
			litem.set_m_lv(1);

			avec.push_back(litem);
			return true;
		}
	public:
		item_create_card() :
			item_create(EItemType::ECard)
		{}
	};

	item_create_material	g_item_create_material;
	item_create_money		g_item_create_moneygold(EMoneyGold);
	item_create_money		g_item_create_moneysilver(EMoneySilver);
	item_create_equip		g_item_create_equip;
	item_create_card		g_item_create_card;
}// namespace ngl