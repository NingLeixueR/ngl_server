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

		actor*															m_actor = nullptr;
		std::map<i16_area, i64_actorid>									m_nspserver;
		std::map<i16_area, bool>										m_register;
		std::function<void(int64_t, const T&, bool)>					m_changedatafun;
		std::map<i64_actorid, T>										m_data;

		static std::mutex												m_mutex;
		static std::map<int64_t, nsp_client<TDerived, TACTOR, T>*>		m_instance;

		bool															m_onlyread = true;		// 只读
		std::set<i64_actorid>											m_dataid;				// 关注哪些数据,结点可读可写

		// [[ m_onlyread == false 才有意义
			std::set<i64_actorid> m_onlyreads;									// 只读全部数据
			std::set<i64_actorid> m_writealls;									// 读/写全部数据
			// m_publishlist1.first:<结点id>
			// m_publishlist1.second:<读写的数据id列表>
			std::map<i64_actorid, std::set<i64_actorid>>			m_publishlist1;
			// m_publishlist2.first:<数据id>
			// m_publishlist2.second:读写的结点列表
			std::map<i64_actorid, std::set<i64_actorid>>			m_publishlist2;							//<数据>被<哪些结点>关注
		// ]]

		nsp_client() = default;
	private:

		void log(const char* amessage)
		{
			m_actor->log_error()->print(
				"{} {}:{}", amessage, tools::type_name<TDerived>(), tools::type_name<T>()
			);
		}

		void channel_data(TDerived*, const message<np_channel_data<T>>& adata)
		{
			const np_channel_data<T>* recv = adata.get_data();
			std::map<int64_t, T>& lmap = *recv->m_data.m_data;
			bool lfirstsynchronize = recv->m_firstsynchronize;
			std::ranges::for_each(lmap, [this,lfirstsynchronize](const auto& apair)
				{
					// # 其他结点不能发送非本结点关注的数据的改变
					if (!m_dataid.empty())
					{
						if (!m_dataid.contains(apair.first))
						{
							tools::no_core_dump();
							return;
						}
					}
					m_data[apair.first] = apair.second;
					if (m_changedatafun != nullptr)
					{
						m_changedatafun(apair.first, apair.second, lfirstsynchronize);
					}
				});
		}

		// # 打印信息
		void print_info()
		{
			std::vector<nguid> lreads;
			std::ranges::for_each(m_onlyreads, [&lreads](i64_actorid anodeid)
				{
					lreads.push_back(anodeid);
				});
			std::vector<nguid> lwrites;
			std::ranges::for_each(m_writealls, [&lwrites](i64_actorid anodeid)
				{
					lwrites.push_back(anodeid);
				});
			std::map<nguid, std::set<int64_t>> lparts;
			for (const auto& item : m_publishlist1)
			{
				i64_actorid lnodeid = item.first;
				for (i64_actorid dataid : item.second)
				{
					lparts[lnodeid].insert(dataid);
				}
			}
			std::string llogstr = std::format("name:{} read:{} write:{} part:{}"
				, typeid(nsp_client<TDerived, TACTOR, T>).name(), lreads, lwrites, lparts
			);
			std::cout << llogstr << std::endl;
		}

		void channel_register_reply(TDerived*, const message<np_channel_register_reply<T>>& adata)
		{
			log("nsp_client np_channel_register_reply");
			const np_channel_register_reply<T>* recv = adata.get_data();

			if (m_actor->id_guid() != recv->m_actorid)
			{
				tools::no_core_dump();
				return;
			}

			m_register[nguid::area(recv->m_actorid)] = true;

			// 如果此结点是只读结点那么根本没必要知道其他结点关注什么数据
			if (!m_onlyread)
			{
				m_onlyreads = recv->m_onlyreads;
				m_onlyreads.erase(m_actor->id_guid());

				m_writealls = recv->m_writealls;
				m_writealls.erase(m_actor->id_guid());

				if (m_dataid.empty())
				{
					m_publishlist1 = recv->m_publishlist;
					m_publishlist1.erase(m_actor->id_guid());
					for (const auto& item : m_publishlist1)
					{
						i64_actorid lnodeid = item.first;
						for (i64_actorid dataid : item.second)
						{
							m_publishlist2[dataid].insert(lnodeid);
						}
					}
				}
				else
				{
					for (const auto& item : recv->m_publishlist)
					{
						i64_actorid lnodeid = item.first;
						if (lnodeid == item.first)
						{
							continue;
						}
						for (i64_actorid dataid : item.second)
						{
							if (m_dataid.contains(dataid))
							{
								m_publishlist1[lnodeid].insert(dataid);
								m_publishlist2[dataid].insert(lnodeid);
							}							
						}
					}
				}
				//print_info();
			}
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
			if (m_onlyread)
			{// 只读全部数据 不关注其他结点
				return;
			}
			const np_channel_dataid_sync<T>* lpdata = adata.get_data();
			if (lpdata == nullptr)
			{
				return;
			}
			if (lpdata->m_onlyread == m_actor->id_guid())
			{
				return;
			}
			if (lpdata->m_add)
			{
				if (lpdata->m_onlyread)
				{
					m_onlyreads.insert(lpdata->m_actorid);
				}
				else
				{
					if (lpdata->m_dataid.empty())
					{
						m_writealls.insert(lpdata->m_actorid);
					}
					else
					{
						if (m_dataid.empty())
						{
							m_publishlist1[lpdata->m_actorid].insert(lpdata->m_dataid.begin(), lpdata->m_dataid.end());
							for (i64_actorid dataid : lpdata->m_dataid)
							{
								m_publishlist2[dataid].insert(lpdata->m_actorid);
							}
						}
						else
						{
							for (i64_actorid dataid : lpdata->m_dataid)
							{
								if (m_dataid.contains(dataid))
								{
									m_publishlist1[lpdata->m_actorid].insert(dataid);
								}
							}
							for (i64_actorid dataid : m_publishlist1[lpdata->m_actorid])
							{
								m_publishlist2[dataid].insert(lpdata->m_actorid);
							}
						}
					}
				}
			}
			else
			{
				m_onlyreads.erase(lpdata->m_actorid);
				m_writealls.erase(lpdata->m_actorid);
				for (i64_actorid dataid : m_publishlist1[lpdata->m_actorid])
				{
					m_publishlist2[dataid].erase(lpdata->m_actorid);
				}
				m_publishlist1.erase(lpdata->m_actorid);
			}
			//print_info();
		}

	public:
		static nsp_client<TDerived, TACTOR, T>& instance(int64_t adataid = nguid::make(), bool acreate = false)
		{
			monopoly_lock(m_mutex);
			auto itor = m_instance.find(adataid);
			if (itor == m_instance.end())
			{
				tools::no_core_dump(acreate);
				tools::no_core_dump(!m_instance.contains(adataid));
				nsp_client<TDerived, TACTOR, T>* lpclient = new nsp_client<TDerived, TACTOR, T>();
				m_instance[adataid] = lpclient;
				return *lpclient;
			}
			return *itor->second;
		}

		static void freensp(int64_t adataid = nguid::make())
		{
			monopoly_lock(m_mutex);
			auto itor = m_instance.find(adataid);
			if (itor == m_instance.end())
			{
				return;
			}
			delete itor->second;
			m_instance.erase(adataid);
		}

		// 只读
		void init_onlyread(TDerived* aactor)
		{
			init(aactor, true, {});
		}

		void init_writeall(TDerived* aactor)
		{
			init(aactor, false, {});
		}

		void init_parts(TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			init(aactor, false, adataid);
		}

	private:
		void init(TDerived* aactor, const std::set<i64_actorid>& adataid)
		{
			init(aactor, false, adataid);
		}

		void init(TDerived* aactor, bool aonlyread, const std::set<i64_actorid>& adataid)
		{
			m_onlyread = aonlyread;
			if (!m_onlyread)
			{
				m_dataid = adataid;
			}
			const std::set<i16_area>* lsetarea = ttab_servers::instance().get_arealist(nconfig::m_nodeid);
			if (lsetarea == nullptr || lsetarea->empty())
			{
				tools::no_core_dump();
				return;
			}

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
				std::function<void(TDerived*, const message<np_channel_data<T>>&)> lchannel_data = [](TDerived* aacotor, const message<np_channel_data<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::instance(aacotor->id_guid()).channel_data(aacotor, adata);
					};
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, true, np_channel_data<T>>(lchannel_data);
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_data<T>>(lchannel_data);

				// 注册回复
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_register_reply<T>>(
					[](TDerived* aacotor, const message<np_channel_register_reply<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::instance(aacotor->id_guid()).channel_register_reply(aacotor, adata);
					}
				);

				// 检查
				actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_check<T>>(
					[](TDerived* aacotor, const message<np_channel_check<T>>& adata)
					{
						if (aacotor == nullptr)
						{
							return;
						}
						nsp_client<TDerived, TACTOR, T>::instance(aacotor->id_guid()).channel_check(aacotor, adata);
					}
				);

				// 同步channel_dataid
				if (!aonlyread)
				{//只读结点不需要知道数据被哪些结点订阅，因为他不能修改数据
					std::function<void(TDerived*, const message<np_channel_dataid_sync<T>>&)> lchannel_dataid_sync = [](TDerived* aacotor, const message<np_channel_dataid_sync<T>>& adata)
						{
							if (aacotor == nullptr)
							{
								return;
							}
							nsp_client<TDerived, TACTOR, T>::instance(aacotor->id_guid()).channel_dataid_sync(aacotor, adata);
						};
					actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, true, np_channel_dataid_sync<T>>(lchannel_dataid_sync);
					actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, false, np_channel_dataid_sync<T>>(lchannel_dataid_sync);
				}				
			}

			i64_actorid lactorid = m_actor->id_guid();
			std::ranges::for_each(m_register, [lactorid](const auto& apair)
				{
					i16_area larea = apair.first;
					wheel_parm lparm
					{
						.m_ms = 1000,
						.m_intervalms = [](int64_t) {return 10000; } ,
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

	public:
		const T* getconst(i64_actorid aactorid)const
		{
			const T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		T* get(i64_actorid aactorid)
		{
			if (m_onlyread)
			{
				tools::no_core_dump();
				return nullptr;
			}
			T* lp = tools::findmap(m_data, aactorid);
			return lp;
		}

		const std::map<i64_actorid, T>& data()const
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
			if (m_onlyread)
			{
				tools::no_core_dump();
				return;
			}
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

		T* add(i64_actorid adataid)
		{
			if (m_onlyread)
			{
				tools::no_core_dump();
				return nullptr;
			}
			if (!m_dataid.empty())
			{
				if (!m_dataid.contains(adataid))
				{
					tools::no_core_dump();
					return nullptr;
				}
			}
			return &m_data[adataid];
		}

		bool change(i64_actorid adataid)
		{
			if (m_onlyread)
			{
				tools::no_core_dump();
				return false;
			}
			if (!m_dataid.empty())
			{// 不是读写全部数据
				if (!m_dataid.contains(adataid))
				{
					tools::no_core_dump();
					return false;
				}
			}

			T* lpdata = tools::findmap(m_data, adataid);
			if (lpdata == nullptr)
			{
				return false;
			}

			auto pro = std::make_shared<np_channel_data<T>>();
			pro->m_data.make();
			(*pro->m_data.m_data)[adataid] = *lpdata;

			// # 发送给nsp server
			actor::static_send_actor(m_nspserver[nguid::area(adataid)], nguid::make(), pro);
			// # 发送给其他nsp client结点
			std::set<i64_actorid> lnodeids;
			// # 只读全部的结点
			lnodeids.insert(m_onlyreads.begin(), m_onlyreads.end());
			// # 读/写全部的结点
			lnodeids.insert(m_writealls.begin(), m_writealls.end());
			// # <数据>被<哪些结点>关注
			auto itor = m_publishlist2.find(adataid);
			if (itor != m_publishlist2.end())
			{
				lnodeids.insert(itor->second.begin(), itor->second.end());
			}
			actor::static_masssend_actor(lnodeids, m_actor->id_guid(), pro);
			return true;
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

			const std::set<i16_area>* lsetarea = ttab_servers::instance().get_arealist(nconfig::m_nodeid);
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
			pro->m_onlyread = m_onlyread;
			pro->m_dataid = m_dataid;
			log_error()->print("nsp_client register: {} -> {}", nguid(pro->m_actorid), nguid(m_nspserver[aarea]));
			actor::static_send_actor(m_nspserver[aarea], nguid::make(), pro);
		}
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::mutex nsp_client<TDerived, TACTOR, T>::m_mutex;

	template <typename TDerived, typename TACTOR, typename T>
	std::map<int64_t, nsp_client<TDerived, TACTOR, T>*>	nsp_client<TDerived, TACTOR, T>::m_instance;
}//namespace ngl
