#pragma once
#include "nactor_auto.h"
#include "manage_csv.h"
#include "tools.h"
#include "nlog.h"

#include <vector>
#include <string>
#include <map>

namespace ngl
{
	template <typename TACTOR>
	class drop
	{
		friend class actor_drop;
	private:
		void init(TACTOR* aactor, const std::set<i64_actorid>& adataid)
		{
			tdb_activitytimes::constcli_init<TACTOR>(aactor, adataid);
		}

		bool isactivity(const tab_random* tab)
		{
			if (tab->m_activityids.empty())
			{
				return true;
			}
			int32_t lnow = localtime::gettime();
			for (int32_t activityid : tab->m_activityids)
			{
				const pbdb::db_activitytimes* lpactivitytimes = tdb_activitytimes::get_constcli<TACTOR>().getconst(activityid);
				if (lpactivitytimes != nullptr)
				{
					if (lnow >= lpactivitytimes->m_beg()
						&& lnow < lpactivitytimes->m_beg()+ lpactivitytimes->m_duration()
						&& lpactivitytimes->m_start()
						)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool weight(int aid, std::map<int, int>& amap)
		{
			const tab_random* tab = allcsv::tab<tab_random>(aid);
			assert(tab != nullptr);
			if (!isactivity(tab))
			{
				return false;
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
				int lweightval = tools::rand() % lweight;
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
						amap[tab->m_randomdatas[j].m_id] += lmin + tools::rand() % ltemp;
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
			log_error()->print("drop:weight:{}", amap);
		}

		bool droplist(int aid, int acount, std::map<int, int>& amap)
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

		void map2goolemap(std::map<int, int>& amap1, google::protobuf::Map<int32_t, int32_t>& amap2)
		{
			for (const std::pair<const int, int>& item : amap1)
			{
				amap2.insert({ item.first, item.second });
			}
		}

		bool droplist(int aid, int acount, google::protobuf::Map<int32_t, int32_t>& amap)
		{
			std::map<int, int> lmap;
			if (droplist(aid, acount, lmap))
			{
				map2goolemap(lmap, amap);
				return true;
			}
			return false;
		}

		bool droplist(int aid, int acount, std::map<int, int>& amap1, google::protobuf::Map<int32_t, int32_t>& amap2)
		{
			if (droplist(aid, acount, amap1))
			{
				map2goolemap(amap1, amap2);
				return true;
			}
			return false;
		}

		bool use(int aid, int acount, i64_actorid aroleid,const std::string& asrc = "", std::map<int, int>* amap = nullptr, int32_t amailid = -1)
		{
			std::string lsrc = std::format("drop<{}>::use({},{},{},{}) src={}", typeid(TACTOR).name(), aid, acount, nguid(aroleid), amailid, asrc);
			log_error()->print(lsrc);
			if (amailid == -1)
			{
				auto psenditem = std::make_shared<np_actor_senditem>();
				psenditem->m_src = lsrc;
				auto pro = std::make_shared<pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE>();
				std::map<int, int> lmap;
				if (droplist(aid, acount, lmap))
				{
					map2goolemap(lmap, *pro->mutable_m_items());
					actor::static_send_actor(aroleid, nguid::make(), psenditem);
					actor::send_client(aroleid, pro);
					if (amap != nullptr)
					{
						lmap.swap(*amap);
					}
					return true;
				}
			}
			else
			{
				auto pro = std::make_shared<np_actor_addmail>();
				pro->m_roleid = aroleid;
				pro->m_tid = amailid;
				if (drop::droplist(aid, acount, pro->m_items))
				{
					actor::static_send_actor(nguid::make_self(ACTOR_MAIL), nguid::make(), pro);
					return true;
				}
			}

			return false;
		}
	};
}// namespace ngl

