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
// File overview: Declares interfaces for bag.

#pragma once

#include "actor/generated/auto/nactor_auto.h"
#include "actor/tab/ttab_random.h"
#include "tools/tab/csv/ncsv.h"
#include "tools/log/nlog.h"
#include "tools/tools.h"

#include <vector>
#include <string>
#include <map>

namespace ngl
{
	template <typename TACTOR>
	class drop
	{
		friend class actor_drop;
		TACTOR* m_actor = nullptr;
	public:
		void init(TACTOR* aactor, const std::set<i64_actorid>& adataid)
		{
			m_actor = aactor;
			if (adataid.empty())
			{
				tdb_activitytimes::nsp_cread<TACTOR>::instance_readall(m_actor, {});
			}
			else
			{
				tdb_activitytimes::nsp_cread<TACTOR>::instance_readpart(m_actor, {}, adataid);
			}
		}

		void exit()
		{
			tdb_activitytimes::nsp_cread<TACTOR>::instance(m_actor->id_guid()).exit();
		}

		bool isactivity(const tab_random* tab)
		{
			if (tab->m_activityids.empty())
			{
				return true;
			}
			int32_t lnow = (int32_t)localtime::gettime();
			for (int32_t activityid : tab->m_activityids)
			{
				const pbdb::db_activitytimes* lpactivitytimes = tdb_activitytimes::nsp_cread<TACTOR>::instance(m_actor->id_guid()).getconst(activityid);
				if (lpactivitytimes != nullptr)
				{
					if (lnow >= lpactivitytimes->mbeg() && lnow < lpactivitytimes->mbeg() + lpactivitytimes->mduration() && lpactivitytimes->mstart())
					{
						return true;
					}
				}
			}
			return false;
		}

		bool weight(int aid, std::map<int, int>& amap)
		{
			auto ltab = ttab_random::instance().tab(aid);
			if (ltab == nullptr)
			{
				tools::no_core_dump();
				return false;
			}

			if (!isactivity(ltab))
			{
				return false;
			}

			int lweight = 0;
			const auto& lrand = ltab->m_randomdatas;
			for (const auto& ldata : lrand)
			{
				lweight += ldata.m_weight;
			}

			if (lweight <= 0)
			{
				return false;
			}

			int lcount = ltab->m_count;
			bool lexclusive = ltab->m_exclusive;
			std::set<std::size_t> lset;
			for (int li = 0; li < lcount; ++li)
			{
				int lweightval = tools::rand() % lweight;
				int ltempweight = 0;
				for (std::size_t lj = 0; lj < lrand.size(); ++lj)
				{
					if (lexclusive && lset.find(lj) != lset.end())
					{
						continue;
					}
					const auto& ldata = lrand[lj];
					ltempweight += ldata.m_weight;
					if (lweightval <= ltempweight)
					{
						if (lexclusive)
						{
							lset.insert(lj);
							lweight -= ldata.m_weight;
							if (lweight <= 0)
							{
								return true;
							}
						}
						int lmin = ldata.m_min;
						int lmax = ldata.m_max;
						int ltemp = lmax - lmin;
						if (ltemp <= 0)
						{
							return false;
						}
						amap[ldata.m_id] += lmin + tools::rand() % ltemp;
						break;
					}
				}
			}
			for (int32_t childid : ltab->m_childrandomids)
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
			for (int li = 0; li < acount; ++li)
			{
				std::map<int, int> lmap;
				if (weight(aid, lmap) == false)
				{
					return false;
				}
				print_weight(lmap);
				for (const auto& [litemid, lcount] : lmap)
				{
					amap[litemid] += lcount;
				}
			}
			return true;
		}

		void map2goolemap(std::map<int, int>& amap1, google::protobuf::Map<int32_t, int32_t>& amap2)
		{
			for (const auto& [lkey, lval] : amap1)
			{
				amap2.insert({ lkey, lval });
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

		bool use(int aid, int acount, i64_actorid aroleid, const std::string& asrc = "", std::map<int, int>* amap = nullptr, int32_t amailid = -1)
		{
			std::string lsrc = std::format("drop<{}>::use({},{},{},{}) src={}", tools::type_name<TACTOR>(), aid, acount, nguid(aroleid), amailid, asrc);
			log_error()->print(lsrc);
			if (amailid == -1)
			{
				auto psenditem = std::make_shared<np_actor_senditem>();
				psenditem->m_src = lsrc;
				pbnet::PROBUFF_NET_REWARD_ITEM_RESPONSE pro;
				std::map<int, int> lmap;
				if (droplist(aid, acount, lmap))
				{
					map2goolemap(lmap, *pro.mutable_mitems());
					actor::send_actor(aroleid, nguid::make(), psenditem);
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
				pro->m_dropid = aid;
				pro->m_dropcount = acount;
				actor::send_actor(nguid::make_self(ACTOR_MAIL), nguid::make(), pro);
				return true;
			}
			return false;
		}
	};
}// namespace ngl
