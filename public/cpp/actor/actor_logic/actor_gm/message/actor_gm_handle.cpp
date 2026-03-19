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
// File overview: Implements message handlers for message.
#include "actor/actor_logic/actor_gmclient/actor_gmclient.h"
#include "actor/actor_logic/actor_gm/actor_gm.h"
namespace ngl
{
	// To request
	class distribute_gmclient
	{
		distribute_gmclient() = default;
	public:
		static distribute_gmclient& instance()
		{
			static distribute_gmclient ltemp;
			return ltemp;
		}
		bool sendtogmclient(NODE_TYPE atype, const message<ngl::np_gm>* adata, actor_gm* agm)
		{
			const tab_servers* tab = ttab_servers::instance().const_tab(atype, nconfig.area());
			if (tab == nullptr)
			{
				return false;
			}
			if (ttab_servers::instance().const_tab()->m_id != tab->m_id)
			{
				int32_t lcount = sysconfig::node_count(atype);
				for (int i = 1;i <= lcount;++i)
				{
					i64_actorid lactorid = nguid::make(ACTOR_GMCLIENT, tab_self_area, nnodeid::nodeid(tab->m_id, i));
					agm->sendbyactorid(lactorid, adata->get_pack(), *adata->get_data());
				}				
				return true;
			}
			return false;
		}
		// Return:whether need toactor_gmhandle
		bool distribute(std::string akey, ncjson& aos, const message<ngl::np_gm>* adata, actor_gm* agm)
		{
			struct servertype
			{
				std::vector<int32_t> servertype;
				DPROTOCOL(servertype, servertype)
			};
			servertype lservertype;
			if (njson::pop(aos, { "data" }, lservertype))
			{
				bool lret = false;
				for (std::size_t i = 0; i < lservertype.servertype.size(); ++i)
				{
					NODE_TYPE lstype = (NODE_TYPE)lservertype.servertype[i];
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
				std::string actor_name;
				int16_t area = 0;
				int32_t dataid = 0;
				DPROTOCOL(gm_guid, actor_name, area, dataid)
			};
			handle_cmd::add("server_stat") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<msg_actor_stat> lpro(adata->get_pack()->m_id, "server_stat", this);
					actor_manage::instance().get_actor_stat(lpro.m_data);
				};
			handle_cmd::add("guid") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<std::string> lresponse(adata->get_pack()->m_id, "guid", this);
					gm_guid lguid;
					if (njson::pop(aos, { "data" }, lguid))
					{
						ENUM_ACTOR ltype;
						ltype = em<ENUM_ACTOR>::get_enum(lguid.actor_name.c_str());
						if (ltype == em<ENUM_ACTOR>::enum_null())
						{
							return;
						}
						lresponse.m_data = tools::lexical_cast<std::string>(nguid::make(ltype, lguid.area, lguid.dataid));
					}
				};
			handle_cmd::add("all_protocol") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<protocols> lresponse(adata->get_pack()->m_id, "all_protocol", this);
					actor_gmclient::allprotocol(lresponse.m_data);
				};
			handle_cmd::add("close_actor") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->get_pack()->m_id, "close_actor", false, this);
					gm_guid lguid;
					if (!njson::pop(aos, { "data" }, lguid))
					{
						ENUM_ACTOR ltype;
						ltype = em<ENUM_ACTOR>::get_enum(lguid.actor_name.c_str());
						if (ltype == em<ENUM_ACTOR>::enum_null())
						{
							return;
						}
						nguid::make(ltype, lguid.area, lguid.dataid);
						auto pro = std::make_shared<np_actor_close>();
						actor::send_actor(nguid::make(ltype, lguid.area, lguid.dataid), id_guid(), pro);
						lresponse.m_data = true;
					}
				};
			handle_cmd::add("get_time") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<std::string> lresponse(adata->get_pack()->m_id, "get_time", localtime::time2str("%Y-%m-%d %H:%M:%S"), this);
					return;
				};
			handle_cmd::add("set_time") = [this](ncjson& aos, const message<ngl::np_gm>* adata)
				{
					gcmd<bool> lresponse(adata->get_pack()->m_id, "set_time", false, this);
					struct operator_set_time
					{
						int32_t time = 0;
						DPROTOCOL(operator_set_time, time)
					};
					operator_set_time ltime;
					if (njson::pop(aos, { "data" }, ltime))
					{
						localtime::settime(ltime.time);
						lresponse.m_data = true;
					}
				};
		}
	}
	bool actor_gm::handle(const message<mforward<ngl::np_gm_response>>& adata)
	{
		return sendtophp((i32_sessionid)adata.get_data()->identifier(), *adata.get_data()->data());
	}
	bool actor_gm::handle(const message<ngl::np_gm>& adata)
	{
		log_error()->print("php2gm [{}]", adata.get_data()->m_json);
		ncjson lreadjson(adata.get_data()->m_json.c_str());

		std::string lactorname;
		i64_actorid lactorid = -1;
		if (njson::pop(lreadjson, { "actor_name" }, lactorname))
		{
			// ### Singleton
			if (lactorname == "ACTOR_GM")
			{
				std::string loperator;
				if (!njson::pop(lreadjson, { "operator" }, loperator))
				{
					return true;
				}
				init_handle_cmd();
				if (distribute_gmclient::instance().distribute(loperator, lreadjson, &adata, this) == false)
				{
					return true;
				}
				if (handle_cmd::function(loperator, lreadjson, &adata) == false)
				{
					log_error()->print("GM actor_gm operator[{}] ERROR", loperator);
				}
				return true;
			}

			ENUM_ACTOR ltype = em<ENUM_ACTOR>::get_enum(lactorname.c_str());
			if (ltype == em<ENUM_ACTOR>::enum_null())
			{
				if (lactorname == "ACTOR_DB")
				{
					int32_t ltype = 0;
					if (!njson::pop(lreadjson, { "db" }, ltype))
					{
						return true;
					}
					ENUM_ACTOR lactortype = db_enum((pbdb::ENUM_DB)(ltype));
					i64_actorid ldbactorid = nguid::make(lactortype, tab_self_area, nguid::none_actordataid());
					sendbyactorid(ldbactorid, adata.get_pack(), *adata.get_data());
				}
				return true;
			}

			sendbytype(ltype, adata.get_pack(), *adata.get_data());
			return true;
		}
		else if (njson::pop(lreadjson, { "actor_id" }, lactorid))
		{// ### Singleton
			sendbyactorid(lactorid, adata.get_pack(), *adata.get_data());
			return true;
		}

		return true;
	}
}//namespace ngl