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
#pragma once

#include "nactor_auto.h"
#include "ttab_random.h"
#include "ncsv.h"
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
			for (int i = 0; i < ltab->m_randomdatas.size(); ++i)
			{
				lweight += ltab->m_randomdatas[i].m_weight;
			}

			if (lweight <= 0)
			{
				return false;
			}

			int lcount = ltab->m_count;
			bool lexclusive = ltab->m_exclusive;
			std::set<int> lset;
			for (int i = 0; i < lcount; ++i)
			{
				int lweightval = tools::rand() % lweight;
				int ltempweight = 0;
				for (int j = 0; j < ltab->m_randomdatas.size(); ++j)
				{
					if (lexclusive)
					{
						if (lset.find(j) != lset.end())
						{
							continue;
						}
					}
					ltempweight += ltab->m_randomdatas[j].m_weight;
					if (lweightval <= ltempweight)
					{
						if (lexclusive)
						{
							lset.insert(j);
							lweight -= ltab->m_randomdatas[j].m_weight;
							if (lweight <= 0)
							{
								return true;
							}
						}
						int lmin = ltab->m_randomdatas[j].m_min;
						int lmax = ltab->m_randomdatas[j].m_max;
						int ltemp = lmax - lmin;
						if (ltemp <= 0)
						{
							return false;
						}
						amap[ltab->m_randomdatas[j].m_id] += lmin + tools::rand() % ltemp;
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

		bool use(int aid, int acount, i64_actorid aroleid, const std::string& asrc = "", std::map<int, int>* amap = nullptr, int32_t amailid = -1)
		{
			std::string lsrc = std::format("drop<{}>::use({},{},{},{}) src={}", typeid(TACTOR).name(), aid, acount, nguid(aroleid), amailid, asrc);
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