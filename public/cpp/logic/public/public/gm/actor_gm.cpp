#include "actor_gm.h"
#include "actor_gmclient.h"

namespace ngl
{
	actor_gm::actor_gm() :
		actor(
			actorparm
			{
				.m_parm
				{
					.m_type = ACTOR_GM,
					.m_area = tab_self_area,
				},
				.m_weight = 0x7fffffff,
			})
	{
	}

	void actor_gm::nregister()
	{
		// 协议注册
		register_handle_custom<actor_gm>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(false);
	}

	void actor_gm::sendbytype(ENUM_ACTOR atype, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(atype, pro, true);
	}

	void actor_gm::sendbyactorid(i64_actorid aactorid, const pack* apack, const ngl::np_gm& apro)
	{
		auto pro = std::make_shared<mforward<ngl::np_gm>>(apack->m_id, apro);
		send_actor(aactorid, pro);
	}

	bool actor_gm::reply_php(const pack* apack, ngl::np_gm_response& adata)const
	{
		log_error()->print("gm2php [{}]", adata.m_json);
		send(apack->m_id, adata, nguid::make(), nguid::make());
		return true;
	}

	// 分发给独立进程的请求
	class distribute_gmclient
	{
		distribute_gmclient(){}
	public:
		static distribute_gmclient& getInstance()
		{
			static distribute_gmclient ltemp;
			return ltemp;
		}

		bool sendtogmclient(NODE_TYPE atype, const message<ngl::np_gm>* adata, actor_gm* agm)
		{
			const tab_servers* tab = ttab_servers::node_tnumber(atype, 1);
			if (tab == nullptr)
			{
				return false;
			}
			if (ttab_servers::tab()->m_id != tab->m_id)
			{
				i64_actorid lactorid = nguid::make(ACTOR_GMCLIENT, tab_self_area, tab->m_id);
				agm->sendbyactorid(lactorid, adata->m_pack, *adata->get_data());
				return true;
			}
			return false;
		}

		bool distribute(std::string akey, const json_read& aos, const message<ngl::np_gm>* adata, actor_gm* agm)
		{
			// # distribute = all 分发给所有actor_gmclient
			struct servertype
			{
				std::vector<int32_t> m_servertype;

				jsonfunc("servertype", m_servertype)
			};

			servertype lservertype;
			if (aos.read("data", lservertype))
			{
				bool lret = false;
				for (int i = 0; i < lservertype.m_servertype.size(); ++i)
				{
					NODE_TYPE lstype = (NODE_TYPE)lservertype.m_servertype[i];
					if (actor_gm::checklocalbytype(lstype) == false)
					{
						sendtogmclient(lstype, adata, agm);
					}
					else
					{
						lret = true;
					}
				}
				return lret;
			}
			return false;
		}
	};

	void init_handle_cmd(actor_gm* agm)
	{
		if (actor_gm::handle_cmd::empty())
		{
			struct gm_guid
			{
				std::string m_actor_name;
				int16_t m_area;
				int32_t m_dataid;

				jsonfunc("actor_name", m_actor_name, "area", m_area, "dataid", m_dataid)
			};

			actor_gm::handle_cmd::push("guid", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gm_guid lguid;
					if (aos.read("data", lguid))
					{
						ENUM_ACTOR ltype;
						ltype = em<ENUM_ACTOR>::get_enum(lguid.m_actor_name.c_str());
						if (ltype == em<ENUM_ACTOR>::enum_null())
						{
							return;
						}
						ngl::json_write lwritejson;
						lwritejson.write("guid", nguid::make(ltype, lguid.m_area, lguid.m_dataid));
						ngl::np_gm_response lresponse;
						lwritejson.get(lresponse.m_json);
						agm->reply_php(adata->m_pack, lresponse);
						return;
					}
				}
			);

			actor_gm::handle_cmd::push("all_protocol", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					int lservertype = 0;
					if (aos.read("data", lservertype))
					{
						const tab_servers* tab = ttab_servers::node_tnumber((NODE_TYPE)lservertype, 1);
						if (tab == nullptr)
						{
							return;
						}
						assert(tab != nullptr && ttab_servers::tab()->m_id == tab->m_id);

						actor_gmclient::protocols pro;
						actor_gmclient::get_allprotocol(pro);

						json_write ljson;
						ljson.write("all_protocol", pro);
						ngl::np_gm_response lresponse;
						ljson.get(lresponse.m_json);
						agm->reply_php(adata->m_pack, lresponse);
					}
					return;
				}
			);

			actor_gm::handle_cmd::push("close_actor", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gm_guid lguid;
					if (aos.read("data", lguid))
					{
						ENUM_ACTOR ltype;
						ltype = em<ENUM_ACTOR>::get_enum(lguid.m_actor_name.c_str());
						if (ltype == em<ENUM_ACTOR>::enum_null())
						{
							return;
						}
						nguid::make(ltype, lguid.m_area, lguid.m_dataid);
						auto pro = std::make_shared<np_actor_close>();
						agm->send_actor(nguid::make(ltype, lguid.m_area, lguid.m_dataid), pro);
						return;
					}
				});

			actor_gm::handle_cmd::push("get_time", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					ngl::json_write lwritejson;
					lwritejson.write("time", localtime::time2str("%Y-%m-%d %H:%M:%S"));
					ngl::np_gm_response lresponse;
					lwritejson.get(lresponse.m_json);
					agm->reply_php(adata->m_pack, lresponse);
					return;
				});

			actor_gm::handle_cmd::push("set_time", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					ngl::json_write lwritejson;
					lwritejson.write("operator", "set_time");

					struct operator_set_time
					{
						int32_t m_time = 0;
						jsonfunc("time", m_time)
					};
					operator_set_time ltime;
					if (aos.read("data", ltime))
					{
						localtime::settime(ltime.m_time);
						lwritejson.write("data", true);
					}
					else
					{
						lwritejson.write("data", false);
					}

					ngl::np_gm_response lresponse;
					lwritejson.get(lresponse.m_json);
					agm->reply_php(adata->m_pack, lresponse);

				});

		}
	}

	//assert(tab != nullptr && ttab_servers::tab()->m_id == tab->m_id);
	bool actor_gm::checklocalbytype(NODE_TYPE atype)
	{
		const tab_servers* tab = ttab_servers::node_tnumber((NODE_TYPE)atype, 1);
		if (tab == nullptr)
		{
			return false;
		}
		return ttab_servers::tab()->m_id == tab->m_id;
	}

	bool actor_gm::handle(const message<ngl::np_gm>& adata)
	{
		log_error()->print("php2gm [{}]", adata.get_data()->m_json);
		ngl::json_read lreadjson(adata.get_data()->m_json.c_str());

		std::string lactorname;
		i64_actorid lactorid = -1;
		if (lreadjson.read("actor_name", lactorname))
		{
			// ### 单例
			if (lactorname == "ACTOR_GM")
			{
				std::string loperator;
				if (lreadjson.read("operator", loperator) == false)
				{
					return true;
				}

				init_handle_cmd(this);
				if (distribute_gmclient::getInstance().distribute(loperator, lreadjson, &adata, this) == false)
				{
					return true;
				}
				if (handle_cmd::function(loperator, lreadjson, &adata) == false)
				{
					log_error()->print("GM actor_gm operator[{}] ERROR", loperator);
				}
				return true;
			}

			ENUM_ACTOR ltype;
			ltype = em<ENUM_ACTOR>::get_enum(lactorname.c_str());
			if (ltype == em<ENUM_ACTOR>::enum_null())
			{
				if (lactorname == "ACTOR_DB")
				{
					int32_t ltype = 0;
					if (lreadjson.read("db", ltype) == false)
					{
						return true;
					}
					i64_actorid ldbactorid = nguid::make(db_enum((pbdb::ENUM_DB)(ltype)), tab_self_area, nguid::none_actordataid());
					sendbyactorid(ldbactorid, adata.m_pack, *adata.get_data());
				}
				return true;
			}

			sendbytype(ltype, adata.m_pack, *adata.get_data());
			return true;
		}
		else if (lreadjson.read("actor_id", lactorid))
		{// ### 非单例
			sendbyactorid(lactorid, adata.m_pack, *adata.get_data());
			return true;
		}

		return true;
	}

	bool actor_gm::handle(const message<mforward<ngl::np_gm_response>>& adata)
	{
		log_error()->print("gm2php [{}]", adata.get_data()->data()->m_json);
		send((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data(), nguid::make(), nguid::make());
		return true;
	}
}// namespace ngl