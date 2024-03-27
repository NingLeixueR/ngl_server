#pragma once

#include "actor_manage.h"
#include "actor_create.h"
#include "db_manage.h"
#include "db_data.h"
#include "db_pool.h"
#include "db.h"
#include "ndb_modular.h"
#include "define_json.h"
#include "conversion.h"
#include "nroleitems.h"
#include "ndbclient.h"
#include "nprotocol.h"
#include "notice.h"
#include "ntimer.h"
#include "gcmd.h"
#include "net.h"

namespace ngl
{
	class actor_notice : public actor
	{
		notice m_notice;

		actor_notice();
	public:
		friend class actor_instance<actor_notice>;
		static actor_notice& getInstance()
		{
			return actor_instance<actor_notice>::instance();
		}

		virtual void init()
		{
			m_notice.set(this);
		}

		static void nregister();

		virtual ~actor_notice() {}

		virtual void loaddb_finish(bool adbishave) {}

		struct gm_notice
		{
			int64_t		m_id;// 公告id
			std::string	m_notice;
			int32_t		m_starttime;// 开始时间
			int32_t		m_finishtime;// 结束时间

			jsonfunc("id", m_id, "notice", m_notice, "starttime", m_starttime, "finishtime", m_finishtime)
		};
		struct get_notice_responce
		{
			std::vector<gm_notice> m_notice;

			jsonfunc("notice", m_notice)
		};

		bool handle(message<mforward<np_gm>>& adata)
		{
			using type = mforward<np_gm_response>;
			ngl::ojson lojson(adata.m_data->data()->m_json.c_str());

			std::string loperator;
			if (lojson.read("operator", loperator) == false)
			{
				return true;
			}

			static std::map<std::string, std::function<void(int,ngl::ojson&)>> lcmd;
			if (lcmd.empty())
			{
				lcmd["get_notice"] = [this](int id, ngl::ojson& aos)
					{
						// 返回 {"notice":gm_notice[]}
						gcmd<std::vector<gm_notice>> pro;
						pro.id = id;
						pro.m_operator = "get_notice_responce";
						for (auto& [key, value] : m_notice.data())
						{
							gm_notice ltemp;
							ltemp.m_id = value.getconst().m_id();
							ngl::conversion::to_asscii(value.getconst().m_notice(), ltemp.m_notice);
							//ltemp.m_notice = value.getconst().m_notice();
							ltemp.m_starttime = value.getconst().m_starttime();
							ltemp.m_finishtime = value.getconst().m_finishtime();
							pro.m_data.push_back(ltemp);
						}
					};
				lcmd["add_notice"] = [this](int id, ngl::ojson& aos)
					{
						// 返回 bool
						gm_notice recv;
						if (aos.read("data", recv) == false)
							return;
						gcmd<bool> pro;
						pro.id = id;
						pro.m_operator = "add_notice_responce";
						pro.m_data = true;
						m_notice.add_notice(recv.m_notice, recv.m_starttime, recv.m_finishtime);
					};

				lcmd["del_notice"] = [this](int id, ngl::ojson& aos)
					{
						// 返回 bool
						int64_t lid = 0;
						if (aos.read("data", lid) == false)
							return;
						gcmd<bool> pro;
						pro.id = id;
						pro.m_operator = "del_notice_responce";
						pro.m_data = true;
						m_notice.del_notice(lid);
					};

			}

			auto itor = lcmd.find(loperator);
			if (itor == lcmd.end())
			{
				LogLocalError("GM actor_notice operator[%] ERROR", loperator);
				return true;
			}
			itor->second(adata.m_data->identifier(), lojson);
			return true;
		}
	};
}