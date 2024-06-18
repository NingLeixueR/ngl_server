#pragma once

#include "nactor_auto.h"
#include "db.pb.h"

#include <iostream>
#include <sstream>
#include <list>

namespace ngl
{
	class actor_ranklist;

	using ranklist_db_modular = ndb_modular<
		pbdb::ENUM_DB_RANKLIST
		, pbdb::db_ranklist
		, actor_ranklist
	>;

	class ranklist : public tdb_ranklist::db_modular
	{
		ranklist(const ranklist&) = delete;
		ranklist& operator=(const ranklist&) = delete;

		template <typename T>
		class operator_value
		{
		public:
			// <
			bool operator()(const pbdb::rankitem* al, const pbdb::rankitem* ar)const
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
						return al->m_roleid() < ar->m_roleid();
					}
				}
			}
		};

		class operator_lv
		{
			
		public:
			static int64_t value(const pbdb::rankitem* av)
			{
				return av->m_lv();
			}
			static int64_t time(const pbdb::rankitem* av)
			{
				return av->m_lvtime();
			}
		};

		std::set<pbdb::rankitem*, operator_value<operator_lv>> m_ranklv;
	public:
		ranklist()
		{
		}

		virtual void set_id()
		{
			m_id = -1;
		}

		std::map<nguid, data_modified<pbdb::db_ranklist>>* get_ranklist()
		{
			return &data();
		}

		enum type
		{
			type_lv = 1,
		};

		virtual void initdata()
		{
			auto lstream = log_error();
			(*lstream) << "actor_ranklist###loaddb_finish" << std::endl;

			for (std::pair<const nguid, data_modified<pbdb::db_ranklist>>& lpair : *get_ranklist())
			{
				pbdb::db_ranklist& lranklist = lpair.second.get(false);
				(*lstream) << "ranklist###id:[" << lranklist.m_id() << "]" << std::endl;
				auto lmap = lranklist.mutable_m_item();
				for(auto itor = lmap->begin();itor != lmap->end();++itor)
				{
					//m_ranklist[lpair.first] = &lpair.second;
					switch (lpair.first)
					{
					case type_lv://lv
						m_ranklv.insert(&itor->second);
						break;
					}
					(*lstream) << "\t[" << itor->first << "]-[" << itor->second.m_name() << "]" << std::endl;
				}
			}
			(*lstream).print("");
		}

		data_modified<pbdb::db_ranklist>& find(type atype)
		{
			auto itor = get_ranklist()->find(atype);
			if (itor == get_ranklist()->end())
			{
				return (*get_ranklist())[atype];
			}
			return itor->second;
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
	};
}// namespace ngl