#pragma once

#include "ndb_modular.h"
#include "threadtools.h"

namespace ngl
{
	//# 订阅/发布[数据副本]
	//# nsp_client接收nsp_server分发的数据与修改数据后向nsp_server同步
	template <typename TDerived, typename TACTOR, typename T>
	class nsp_client
	{
	private:
		nsp_client(const nsp_client&) = delete;
		nsp_client& operator=(const nsp_client&) = delete;

		actor*											m_actor = nullptr;
		std::map<i16_area, i64_actorid>					m_nspserver;
		std::map<i16_area, bool>						m_register;
		std::set<i64_actorid>							m_dataid;
		std::function<void(int64_t, const T&, bool)>	m_changedatafun;
		bool											m_activate = false;
		std::map<i64_actorid, T>						m_data;
		std::map<i64_actorid, std::set<i64_actorid>>	m_publishlist;
		static std::shared_mutex									m_mutex;
		static std::map<int64_t, nsp_client<TDerived, TACTOR, T>*>	m_map;
	private:
		nsp_client() = default;
	public:
		std::map<i64_actorid, T>& data()
		{
			return m_data;
		}
	private:
		template <typename TX>
		static std::string& type_name()
		{
			return tools::type_name<TX>();
		}

		void log(const char* amessage)
		{
			m_actor->log_error()->print("{} {}:{}", amessage, type_name<TDerived>(), type_name<T>());
		}

		void channel_data(TDerived*, const message<np_channel_data<T>>& adata)
		{
			const np_channel_data<T>& recv = *adata.get_data();
			std::map<int64_t, T>& lmap = *recv.m_data.m_data;
			bool lfirstsynchronize = recv.m_firstsynchronize;
			std::ranges::for_each(lmap, [this,lfirstsynchronize](const auto& apair)
				{
					if (!m_dataid.empty() && !m_dataid.contains(apair.first))
					{
						return;
					}
					m_data[apair.first] = apair.second;
					if (m_changedatafun != nullptr)
					{
						m_changedatafun(apair.first, apair.second, lfirstsynchronize);
					}
				});
		}

		void channel_register_reply(TDerived*, const message<np_channel_register_reply<T>>& adata)
		{
			log("nsp_client np_channel_register_reply");
			auto& recv = *adata.get_data();
			m_register[nguid::area(recv.m_actorid)] = true;
			m_publishlist = recv.m_publishlist;
			m_publishlist.erase(m_actor->id_guid());
		}

		void channel_check(TDerived*, const message<np_channel_check<T>>& adata)
		{
			auto lprecv = adata.get_data();
			if (m_register[lprecv->m_area])
			{
				twheel::wheel().removetimer(lprecv->m_timer);
				return;
			}
			register_echannel(lprecv->m_area);
		}

		void channel_dataid_sync(TDerived*, const message<np_channel_dataid_sync<T>>& adata)
		{
			const np_channel_dataid_sync<T>* lpdata = adata.get_data();
			if (lpdata == nullptr)
			{
				return;
			}
			if (lpdata->m_deleteactorid != 0)
			{
				m_publishlist.erase(lpdata->m_deleteactorid);
			}
			if (lpdata->m_addactorid != 0)
			{
				m_publishlist[lpdata->m_addactorid] = lpdata->m_adddataids;
			}
		}

	public:
		static nsp_client<TDerived, TACTOR, T>& getInstance(int64_t adataid = nguid::make(), bool acreate = false)
		{
			std::lock_guard<std::shared_mutex> llock(nsp_client<TDerived, TACTOR, T>::m_mutex);
			if (!m_map.contains(adataid))
			{
				assert(acreate);
				m_map[adataid] = new nsp_client<TDerived, TACTOR, T>();
			}
			return *m_map[adataid];
		}

		static void freensp(int64_t adataid = nguid::make())
		{
			std::lock_guard<std::shared_mutex> llock(nsp_client<TDerived, TACTOR, T>::m_mutex);
			auto itor = m_map.find(adataid);
			if (itor != m_map.end())
			{
				delete itor->second;
				m_map.erase(itor);
			}
		}

		void init(TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			const std::set<i16_area>* lsetarea = ttab_servers::get_arealist(nconfig::m_nodeid);
			Assert(lsetarea->empty() == false);
			auto ltype = (ENUM_ACTOR)nguid::type(TACTOR::actorid());
			for (i16_area area : *lsetarea)
			{
				m_nspserver[area] = nguid::make(ltype, area, nguid::none_actordataid());
				m_register[area] = false;
			}
			m_actor		= aactor;
			m_dataid	= adataid;
			
			static bool lisregister = false;
			if (!lisregister)
			{
				lisregister = true;

				// 更新数据
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>>(
					[](TDerived* aacotor, const message<np_channel_data<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::getInstance(aacotor->id_guid()).channel_data(aacotor, adata);
					}
				);

				// 注册回复
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>>(
					[](TDerived* aacotor, const message<np_channel_register_reply<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::getInstance(aacotor->id_guid()).channel_register_reply(aacotor, adata);
					}
				);

				// 检查
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_check<T>>(
					[](TDerived* aacotor, const message<np_channel_check<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::getInstance(aacotor->id_guid()).channel_check(aacotor, adata);
					}
				);

				// 同步channel_dataid
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_dataid_sync<T>>(
					[](TDerived* aacotor, const message<np_channel_dataid_sync<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::getInstance(aacotor->id_guid()).channel_dataid_sync(aacotor, adata);
					}
				);
			}

			i64_actorid lactorid = m_actor->id_guid();
			std::ranges::for_each(m_register, [lactorid](const auto& apair)
				{
					i16_area larea = apair.first;
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 1000; } ,
						.m_count = 0x7fffffff,
						.m_fun = [larea,lactorid](const wheel_node* anode)
						{
							auto pro = std::make_shared<np_channel_check<T>>(
								np_channel_check<T>{
									.m_timer = anode->m_timerid,
									.m_area = larea,
								});
							actor::static_send_actor(lactorid, nguid::make(), pro);
						}
					};
					twheel::wheel().addtimer(lparm);
				});			
		}

		const T* getconst(i64_actorid aactorid)const
		{
			const T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		T* get(i64_actorid aactorid)
		{
			T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		const std::map<i64_actorid, T>& get_map()
		{
			return m_data;
		}

		void foreach(const std::function<void(const T&)>& afun)const
		{
			for (const auto& [_key, _value] : m_data)
			{
				afun(_value);
			}
		}

		void foreach_change(const std::function<bool(T&)>& afun)
		{
			std::map<i16_area, std::shared_ptr<np_channel_data<T>>> lmap;
			for (auto itor = m_data.begin();itor != m_data.end();++itor)
			{
				i16_area larea = nguid::area(itor->first);
				if (!lmap.contains(larea))
				{
					lmap[larea] = std::make_shared<np_channel_data<T>>();
					lmap[larea]->m_data.make();
				}
				if (afun(itor->second))
				{
					(*lmap[larea]->m_data.m_data)[itor->first] = itor->second;
				}
			}
			for (auto itor = lmap.begin(); itor != lmap.end(); ++itor)
			{
				actor::static_send_actor(m_nspserver[itor->first], nguid::make(), itor->second);
			}
		}

		T* add(i64_actorid aactorid)
		{
			return &m_data[aactorid];
		}

		void change(i64_actorid aactorid)
		{
			T* lpdata = tools::findmap(m_data, aactorid);
			if (lpdata == nullptr)
			{
				return;
			}

			auto pro = std::make_shared<np_channel_data<T>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[aactorid] = *lpdata;
			actor::static_send_actor(m_nspserver[nguid::area(aactorid)], nguid::make(), pro);

			for (const auto& item : m_publishlist)
			{
				if (item.second.empty() || item.second.contains(aactorid))
				{
					actor::static_send_actor(item.first, nguid::make(), pro);
				}
			}			
		}

		// # 如果数据发生变化
		// # 在回调函数中 可以直接调用寄生actor的内部变量因为回调函数是在actor.nsp_client.handle中被调用的
		void set_changedata_fun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_changedatafun = afun;
		}

		// # 请求退出订阅
		void exit()
		{
			auto pro = std::make_shared<np_channel_exit<T>>();
			pro->m_actorid = m_actor->id_guid();

			const std::set<i16_area>* lsetarea = ttab_servers::get_arealist(nconfig::m_nodeid);
			if (lsetarea != nullptr)
			{
				for (i16_area area : *lsetarea)
				{
					actor::static_send_actor(m_nspserver[area], nguid::make(), pro);
					m_register[area] = false;
				}
			}
		}
	private:
		void register_echannel(i16_area aarea)
		{
			log("nsp_client register");
			auto pro = std::make_shared<np_channel_register<T>>();
			pro->m_actorid = m_actor->id_guid();

			log_error()->print("nsp_client register: {} -> {}", nguid(pro->m_actorid), nguid(m_nspserver[aarea]));
			actor::static_send_actor(m_nspserver[aarea], nguid::make(), pro);
		}
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::shared_mutex nsp_client<TDerived, TACTOR, T>::m_mutex;

	template <typename TDerived, typename TACTOR, typename T>
	std::map<int64_t, nsp_client<TDerived, TACTOR, T>*>	nsp_client<TDerived, TACTOR, T>::m_map;
}//namespace ngl