#include "nsp_server.h"

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::init(ndb_modular<ENUMDB, T, TDerived>* adbmodule)
	{
		m_dbmodule = adbmodule;

		ttab_servers::instance().get_arealist_nonrepet(nconfig::m_tid, m_areaset);

		// # 订阅注册处理
		actor::register_actor_s<TDerived, np_channel_register<T>>(
			[](TDerived* aactor, const message<np_channel_register<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);

		// # 订阅数据被修改
		actor::register_actor_s<TDerived, np_channel_data<T>>(
			[](TDerived* aactor, const message<np_channel_data<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);

		// # 退出订阅
		actor::register_actor_s<TDerived, np_channel_exit<T>>(
			[](TDerived* aactor, const message<np_channel_exit<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_register<T>>& adata)
	{
		const np_channel_register<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;
		if (m_care.contains(lactorid))
		{
			return;
		}
		if (recv->m_readall && m_nodereadalls.contains(lactorid))
		{
			return;
		}

		i16_area larea = nguid::area(lactorid);
		if (recv->m_read)
		{
			if (recv->m_all)
			{
				m_nodereadalls.insert(lactorid);
			}
			else
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(recv->m_readids);
			}
		}
		else
		{
			if (recv->m_all)
			{
				m_nodereadalls.insert(lactorid);
			}
			else
			{
				care_data& lcare = m_care[lactorid];
				lcare.init(recv->m_readids, recv->m_writeids);
			}
		}
		
		for (i32_fieldnumber fieldnumber : recv->m_fieldnumbers)
		{
			m_operator_field.add_field(larea, fieldnumber, epb_field_read);
		}

		m_nodepart.insert(lactorid);

		i64_actorid lnspserver = m_dbmodule->get_actor()->id_guid();

		{// 回复
			auto pro = std::make_shared<np_channel_register_reply<T>>();
			pro->m_actorid = lnspserver;
			pro->m_nodereadalls = m_nodereadalls;
			pro->m_nodewritealls = m_nodewritealls;
			for (const auto& [_nodeid, _care] : m_care)
			{
				if (lactorid == _nodeid)
				{
					continue;
				}
				pro->m_care[_nodeid] = _care.get_core();
			}
			pro->m_care = m_nodewritealls;
			pro->m_node_fieldnumbers = m_operator_field.fieldnumbers();
			actor::send_actor(lactorid, pro);
		}
		// 同步数据
		{
			std::function<std::shared_ptr<np_channel_data<T>>()> lmalloc = []()->std::shared_ptr<np_channel_data<T>>
				{
					auto pro = std::make_shared<np_channel_data<T>>();
					pro->m_actorid = lnspserver;
					pro->m_firstsynchronize = true;
					return pro;
				};
			auto pro = lmalloc();

			int32_t lindex = 0;
			if (recv->m_readall)
			{
				m_dbmodule->foreach([&pro,&lindex](const data_modified<T>& adata)
					{
						const T& ldata = *adata.getconst();
						pro->m_data[ldata.mid()] = ldata;
						++lindex;
						if (lindex % esend_maxcount == 0)
						{
							actor::send_actor(lactorid, nguid::make(), pro);
							pro = lmalloc();
						}
					});
			}
			else 
			{
				//std::set<i64_actorid> m_readids;
				for (i64_actorid readid : recv->m_readids)
				{
					data_modified<T>* lpmodifieddata = m_dbmodule->find(readid);
					if (lpmodifieddata == nullptr)
					{
						continue;
					}
					const T* lpdata = lpmodifieddata->getconst();
					if (lpdata == nullptr)
					{
						continue;
					}
					pro->m_data[lpdata->mid()] = *lpdata;
					++lindex;
					if (lindex % esend_maxcount == 0)
					{
						actor::send_actor(lactorid, nguid::make(), pro);
						pro = lmalloc();
					}
				}
			}
			if (lindex % esend_maxcount == 0)
			{
				pro = lmalloc();
			}
			pro->m_recvfinish = true;
			actor::send_actor(lactorid, nguid::make(), pro);
		}

		// 通知其他结点，有新结点加入
		{
			auto pro = std::make_shared<np_channel_dataid_sync<T>>();
			pro->m_actorid = lactorid;
			pro->m_read = true;
			pro->m_all = recv->m_readall;
			if (!recv->m_readall)
			{
				pro->m_readpart = recv->m_readids;
			}
			pro->m_fieldnumbers = recv->m_fieldnumbers;

			std::set<i64_nodeid> lnodes;
			lnodes.insert(m_nodereadalls);
			lnodes.insert(m_nodewritealls);
			lnodes.insert(m_nodepart);
			actor::send_actor(lnodes, nguid::make(), pro);
		}
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_exit<T>>& adata)
	{
		const np_channel_exit<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		m_nodepart.erase(lactorid);
		m_nodewritealls.erase(lactorid);
		m_nodereadalls.erase(lactorid);
		m_care.erase(lactorid);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived*, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();
		i64_actorid lactorid = recv->m_actorid;

		if (!m_care.contains(lactorid))
		{
			tools::no_core_dump();
			return;
		}
		care_data lcare = m_care[lactorid];
		i16_area larea = nguid::area(lactorid);
		for (const auto& [ _dataid, _data] : recv->m_data)
		{
			if (!lcare.is_write(_dataid))
			{
				tools::no_core_dump();
				return;
			}
			data_modified<T>& ldata = m_dbmodule->get(_dataid);
			m_operator_field.field_copy(larea, _data, ldata.get());
		}
		for (i64_actorid dataid : recv->m_deldata)
		{
			if (!lcare.is_write(dataid))
			{
				tools::no_core_dump();
				return;
			}
			m_dbmodule->erase(dataid);
		}
	}
}//namespace ngl
