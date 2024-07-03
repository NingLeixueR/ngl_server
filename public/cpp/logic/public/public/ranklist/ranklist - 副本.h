#pragma once

#include "nactor_auto.h"
#include "net.pb.h"
#include "db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	struct rank_item
	{
		i64_actorid m_actorid;
		int32_t		m_time;

		rank_item() :
			m_actorid(0),
			m_time(0)
		{}

		const pbdb::db_brief* get()const
		{
			return tdb_brief::nsp_cli<actor_ranklist>::getconst(m_actorid);
		}
	};


	class ranklist
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		template <typename T>
		struct operator_value
		{
			// <
			bool operator()(const rank_item& al, const rank_item& ar)const
			{
				if (T::value(al) > T::value(ar))
				{
					return true;
				}
				else if (T::value(al) < T::value(ar))
				{
					return false;
				}
				else
				{
					if (T::time(al) < T::time(ar))
						return true;
					else if (T::time(al) > T::time(ar))
						return false;
					else
					{
						return al.m_actorid < ar.m_actorid;
					}
				}
			}
		};

		struct operator_lv
		{
			static int64_t value(const rank_item& av)
			{
				return av.get()->m_lv();
			}
			static int64_t time(const rank_item& av)
			{
				return av.m_time;
			}
		};

		std::set<rank_item, operator_value<operator_lv>> m_ranklv;
	public:
		ranklist()
		{
		}

		void initdata()
		{
			//std::map<i64_actorid, pbdb::db_brief>;
			for (const auto& [_actorid, _brief] : tdb_brief::nsp_cli<actor_ranklist>::m_data)
			{
				rank_item litem;
				litem.m_actorid = _actorid;
				litem.m_time = 
			}
		}

		void update_lv(i64_actorid aroleid, int alv)
		{
			data_modified<pbdb::db_ranklist>& llist = find(type_lv);
			pbdb::db_ranklist& lranklist = llist.get(false);
			pbdb::rankitem& litem = (*lranklist.mutable_m_item())[aroleid];
			m_ranklv.erase(&litem);
			litem.set_m_lv(alv);
			litem.set_m_lvtime(localtime::gettime());
			m_ranklv.insert(&litem);
		}

		template <typename OPERATOR_T>
		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_rankset(std::set<pbdb::rankitem*, operator_value<OPERATOR_T>>& aset)
		{
			auto pro = std::make_shared<pbnet::PROBUFF_NET_RANKLIST_RESPONSE>();
			for (const pbdb::rankitem* item : aset)
			{
				*pro->add_m_items() = *item;
			}
			return pro;
		}


		std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> get_ranklist(pbnet::eranklist atype)
		{
			std::shared_ptr<pbnet::PROBUFF_NET_RANKLIST_RESPONSE> pro;
			switch (atype)
			{
			case pbnet::eranklist::lv:
			{
				pro = get_rankset(m_ranklv);
			}
			break;
			default:
				return nullptr;
			}
			pro->set_m_type(atype);
			return pro;
		}

		void sync_ranklist(i64_actorid aroleid, pbnet::eranklist atype)
		{
			auto pro = get_ranklist(atype);
			actor::send_client(aroleid, pro);
		}
	};
}// namespace ngl