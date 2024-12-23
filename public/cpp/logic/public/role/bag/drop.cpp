#include "ttab_calendar.h"
#include "actor_role.h"
#include "manage_csv.h"
#include "drop.h"
#include "bag.h"

namespace ngl
{
	void drop::init()
	{
		std::srand(std::time(nullptr));
	}

	bool drop::weight(int aid, std::map<int, int>& amap)
	{
		const tab_random* tab = allcsv::tab<tab_random>(aid);
		if (tab == nullptr)
		{
			return false;
		}
		if (tab->m_calendarids > 0)
		{
			if (ttab_calendar::is_period(tab->m_calendarids) == false)
			{
				return true;
			}
		}

		int lweight = 0;
		for (int i = 0; i < tab->m_randomdatas.size(); ++i)
		{
			lweight += tab->m_randomdatas[i].m_weight;
		}

		if (lweight <= 0)
		{
			return false;
		}

		int lcount = tab->m_count;
		bool lexclusive = tab->m_exclusive;
		std::set<int> lset;
		for (int i = 0; i < lcount; ++i)
		{
			int lweightval = std::rand() % lweight;
			int ltempweight = 0;
			for (int j = 0; j < tab->m_randomdatas.size(); ++j)
			{
				if (lexclusive)
				{
					if (lset.find(j) != lset.end())
					{
						continue;
					}
				}
				ltempweight += tab->m_randomdatas[j].m_weight;
				if (lweightval <= ltempweight)
				{
					if (lexclusive)
					{
						lset.insert(j);
						lweight -= tab->m_randomdatas[j].m_weight;
						if (lweight <= 0)
						{
							return true;
						}
					}
					int lmin = tab->m_randomdatas[j].m_min;
					int lmax = tab->m_randomdatas[j].m_max;
					int ltemp = lmax - lmin;
					if (ltemp <= 0)
					{
						return false;
					}
					amap[tab->m_randomdatas[j].m_id] += lmin + std::rand() % ltemp;
					break;
				}
			}
		}
		for (int32_t childid : tab->m_childrandomids)
		{
			if (weight(childid, amap) == false)
			{
				return false;
			}
		}
		return true;
	}

	void print_weight(const std::map<int, int>& amap)
	{
		auto lstream = log_error();
		(*lstream) << "weight[";
		for (const auto& [key, value] : amap)
		{
			(*lstream) << key << ":" << value << ",";
		}
		(*lstream) << "]" << std::endl;
		(*lstream).print("");
	}

	bool drop::droplist(int aid, int acount, std::map<int, int>& amap)
	{
		for (int i = 0; i < acount; ++i)
		{
			std::map<int, int> lmap;
			if (weight(aid, lmap) == false)
			{
				return false;
			}
			print_weight(lmap);
		}			
		return true;
	}

	bool drop::droplist(int aid, int acount, google::protobuf::Map<int32_t, int32_t>& amap)
	{
		if (std::map<int, int> lmap; droplist(aid, acount, lmap))
		{
			for (const std::pair<const int, int>& item : lmap)
			{
				amap.insert({ item.first, item.second });
			}
			return true;
		}
		return false;		
	}

	bool drop::droplist(int aid, int acount, std::map<int, int>& amap1, google::protobuf::Map<int32_t, int32_t>& amap2)
	{
		if (droplist(aid, acount, amap1))
		{
			for (const std::pair<const int, int>& item : amap1)
			{
				amap2.insert({ item.first, item.second });
			}
			return true;
		}
		return false;
	}

	bool drop::use(actor_role* arole, int aid, int acount)
	{
		std::map<int, int> lmap;
		return droplist(aid, acount, lmap) && arole->m_bag.add_item(lmap)? true : false;
	}

	bool drop::use(i64_actorid aactorid, int aid, int acount, i64_actorid aactorid2 /*= nguid::make()*/)
	{
		auto psenditem = std::make_shared<np_actor_senditem>();
		auto pro = std::make_shared<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>();
		
		if (drop::droplist(aid, acount, psenditem->m_item, *pro->mutable_m_items()))
		{
			actor::static_send_actor(aactorid, aactorid2, psenditem);
			actor::send_client(aactorid, pro);
			return true;
		}
		return false;
	}

}// namespace ngl