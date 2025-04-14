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

	void actor_gm::loaddb_finish(bool adbishave) 
	{
	}

	ENUM_ACTOR actor_gm::actor_type()
	{
		return ACTOR_GM;
	}

	i64_actorid actor_gm::actorid(i16_area area)
	{
		return nguid::make(actor_type(), area, nguid::none_actordataid());
	}

	void actor_gm::init() 
	{
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
		distribute_gmclient() = default;
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

	void actor_gm::init_handle_cmd()
	{
		if (handle_cmd::empty())
		{
			struct gm_guid
			{
				std::string m_actor_name;
				int16_t m_area;
				int32_t m_dataid;
				jsonfunc("actor_name", m_actor_name, "area", m_area, "dataid", m_dataid)
			};

			handle_cmd::push("server_stat", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<actor_manage::msg_actor_stat> lpro(adata->m_pack->m_id, "server_stat", this);
					actor_manage::getInstance().get_actor_stat(lpro.m_data);
				}
			);

			handle_cmd::push("guid", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<std::string> lresponse(adata->m_pack->m_id, "guid", this);
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

			handle_cmd::push("all_protocol", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<actor_gmclient::protocols> lresponse(adata->m_pack->m_id, "all_protocol", this);
					actor_gmclient::get_allprotocol(lresponse.m_data);
				}
			);

			handle_cmd::push("close_actor", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->m_pack->m_id, "close_actor", false, this);
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
						send_actor(nguid::make(ltype, lguid.m_area, lguid.m_dataid), pro);
						lresponse.m_data = true;
					}
				});

			handle_cmd::push("get_time", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<std::string> lresponse(adata->m_pack->m_id, "get_time", localtime::time2str("%Y-%m-%d %H:%M:%S"), this);
					return;
				});

			handle_cmd::push("set_time", [this](const json_read& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->m_pack->m_id, "set_time", false, this);
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

	void actor_gm::nregister()
	{
		// 协议注册
		register_handle_custom<actor_gm>::func<
			ngl::np_gm
			, mforward<ngl::np_gm_response>
		>(false);
	}

	bool actor_gm::sendtophp(i32_sessionid aidentifier, const ngl::np_gm_response& adata)
	{
		::ngl::log_error()->print("gm2php [{}]", adata.m_json);
		actor::send(aidentifier, adata, nguid::make(), nguid::make());
		return true;
	}

	template <typename T>
	void gcmd<T>::execute(std::shared_ptr<mforward<ngl::np_gm_response>>& apro)
	{
		assert(m_actor != nullptr);
		assert(m_actor->id_guid() == actor_gm::getInstance().id_guid());
		actor_gm::sendtophp(apro->identifier(), *apro->data());
	}
}// namespace ngl