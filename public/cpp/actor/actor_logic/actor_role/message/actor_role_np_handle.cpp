#include "ttab_specialid.h"
#include "actor_events.h"
#include "manage_curl.h"
#include "nsp_server.h"
#include "nsp_client.h"
#include "actor_role.h"
#include "json_write.h"
#include "json_read.h"
#include "nregister.h"
#include "nforward.h"
#include "net.pb.h"
#include "drop.h"
#include "gcmd.h"


namespace ngl
{
	bool actor_role::handle(const message<np_eevents_logic_rolelogin>& adata)
	{
		time_t llastloginutc = 0;
		time_t lnow = localtime::gettime();
		if (m_rolekv.value("lastloginutc", llastloginutc))
		{
			if (localtime::issameday(lnow, llastloginutc))
			{
				return true;
			}
		}
		m_rolekv.set_value("lastloginutc", lnow);
		static_task::update_change(this, ETaskRoleLogin, 1);
		return true;
	}

	bool actor_role::handle(const message<np_example_actorid>& adata)
	{
		m_example.first = adata.get_data()->m_type;
		m_example.second = adata.get_data()->m_actorexampleid;
		return true;
	}

	bool actor_role::handle(const message<np_actor_senditem>& adata)
	{
		auto lparm = adata.get_data();
		local_remakes(this, lparm->m_src);
		m_bag.add_item(lparm->m_item);
		return true;
	}

	bool actor_role::handle(const message<np_actor_disconnect_close>& adata)
	{
		erase_actor_byid();
		return true;
	}

	bool actor_role::handle(const message<mforward<np_gm>>& adata)
	{
		ngl::json_read lojson(adata.get_data()->data()->m_json.c_str());
		std::string loperator;
		if (lojson.read("operator", loperator) == false)
		{
			return true;
		}

		if (handle_gm::empty())
		{
			handle_gm::push("pay", [this](int id, const ngl::json_read& aos)
				{
					gcmd<int32_t> pro(id, "pay");
					struct pay
					{
						std::string m_orderid;
						int32_t m_rechargeid;
						jsonfunc("orderid", m_orderid, "rechargeid", m_rechargeid)
					};
					pay lpay;
					if (aos.read("data", lpay) == false)
					{
						return;
					}
					pro.m_data = rechange(lpay.m_orderid, lpay.m_rechargeid, false, true);
				}
			);
			handle_gm::push("gmrechange", [this](int id, const ngl::json_read& aos)
				{
					gcmd<int32_t> pro(id, "gmrechange");
					int32_t lrechargeid;
					if (aos.read("data", lrechargeid) == false)
					{
						return;
					}
					std::string lorder;
					createorder(lorder, lrechargeid);
					pro.m_data = rechange(lorder, lrechargeid, true, true);
				});
			handle_gm::push("rechange", [this](int id, const ngl::json_read& aos)
				{//actor_role::loginpay() callback
					gcmd<int32_t> pro(id, "rechange");
					prorechange lrechange;
					if (aos.read("data", lrechange) == false)
					{
						return;
					}

					pro.m_data = rechange(lrechange.m_orderid, lrechange.m_rechargeid, false, true);
				});
			// ½ûÑÔ lduration=0½â·â
			handle_gm::push("bantalk", [this](int id, const ngl::json_read& aos)
				{
					gcmd<bool> pro(id, "bantalk", false);
					int32_t lduration;
					if (aos.read("data", lduration) == false)
					{
						return;
					}

					int lnow = (int)localtime::gettime();
					m_info.change_notalkutc(lnow + lduration);
					pro.m_data = true;
					log_error()->print("[{}] bantalk [{}]", id_guid(), tools::time2str(lnow + lduration));
				});
		}

		if (handle_gm::function(loperator, (int)adata.get_data()->identifier(), lojson) == false)
		{
			log_error()->print("GM actor_role php operator[{}] ERROR", loperator);
		}
		return true;
	}

}