#include "actor_gmclient.h"
#include "actor_gm.h"

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

		// 返回值:是否还需要actor_gm处理
		bool distribute(std::string akey, const json_read& aos, const message<ngl::np_gm>* adata, actor_gm* agm)
		{
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
					if (actor_gm::checklocalbytype(lstype))
					{
						lret = true;
						continue;
					}
					sendtogmclient(lstype, adata, agm);
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
					gcmd<std::string> lresponse(adata->m_pack->m_id, "guid");
					gm_guid lguid;
					if (aos.read("data", lguid))
					{
						ENUM_ACTOR ltype;
						ltype = em<ENUM_ACTOR>::get_enum(lguid.m_actor_name.c_str());
						if (ltype == em<ENUM_ACTOR>::enum_null())
						{
							return;
						}
						lresponse.m_data = tools::lexical_cast<std::string>(nguid::make(ltype, lguid.m_area, lguid.m_dataid));
					}
				}
			);

			actor_gm::handle_cmd::push("all_protocol", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<actor_gmclient::protocols> lresponse(adata->m_pack->m_id, "all_protocol");
					actor_gmclient::get_allprotocol(lresponse.m_data);
				}
			);

			actor_gm::handle_cmd::push("close_actor", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->m_pack->m_id, "close_actor", false);
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
						lresponse.m_data = true;
					}
				});

			actor_gm::handle_cmd::push("get_time", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<std::string> lresponse(adata->m_pack->m_id, "get_time", localtime::time2str("%Y-%m-%d %H:%M:%S"));
					return;
				});

			actor_gm::handle_cmd::push("set_time", [agm](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->m_pack->m_id, "set_time", false);
					struct operator_set_time
					{
						int32_t m_time = 0;
						jsonfunc("time", m_time)
					};
					operator_set_time ltime;
					if (aos.read("data", ltime))
					{
						localtime::settime(ltime.m_time);
						lresponse.m_data = true;
					}
				});
		}
	}

	bool actor_gm::checklocalbytype(NODE_TYPE atype)
	{
		return ttab_servers::tab()->m_type == atype;
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

	bool actor_gm::sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata)
	{
		log_error()->print("gm2php [{}]", adata.m_json);
		send(aidentifier, adata, nguid::make(), nguid::make());
		return true;
	}

	bool actor_gm::handle(const message<mforward<ngl::np_gm_response>>& adata)
	{
		return sendtophp((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data());
	}
}// namespace ngl