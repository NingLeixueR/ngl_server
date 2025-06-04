#pragma once

#include "ndb_modular.h"

namespace ngl
{
	//# ����/����[���ݸ���]
	//# nsp_client����nsp_server�ַ����������޸����ݺ���nsp_serverͬ��
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
		std::map<i64_actorid, T> m_data;

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
		}

		void channel_check(TDerived*, const message<np_channel_check>& adata)
		{
			auto lprecv = adata.get_data();
			if (m_register[lprecv->m_area])
			{
				twheel::wheel().removetimer(lprecv->m_timer);
				return;
			}
			register_echannel(lprecv->m_area);
		}

	public:
		static nsp_client<TDerived, TACTOR, T>& getInstance()
		{
			static nsp_client<TDerived, TACTOR, T> ltemp;
			return ltemp;
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
			// ��������
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_data<T>>(
				std::bind_front(&nsp_client<TDerived, TACTOR, T>::channel_data, this)
			);

			// ע��ظ�
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_register_reply<T>>(
				std::bind_front(&nsp_client<TDerived, TACTOR, T>::channel_register_reply, this)
			);

			// ���
			actor::register_actor_s<EPROTOCOL_TYPE_CUSTOM, TDerived, np_channel_check>(
				std::bind_front(&nsp_client<TDerived, TACTOR, T>::channel_check, this)
			);

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
							auto pro = std::make_shared<np_channel_check>(
								np_channel_check{
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
		}

		// # ������ݷ����仯
		// # �ڻص������� ����ֱ�ӵ��ü���actor���ڲ�������Ϊ�ص���������actor.nsp_client.handle�б����õ�
		void set_changedata_fun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_changedatafun = afun;
		}

		// # �����˳�����
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

	// nsp_client�ķ�װ ����ֻ������д
	// ���ڷǵ�����actor������actor_role,��actor_role�ĸ���ʵ����ʹ��nsp_constclient

	template <typename TDerived, typename TACTOR, typename T>
	class nsp_constclient
	{
		nsp_constclient() = delete;
		nsp_constclient(const nsp_constclient&) = delete;
		nsp_constclient& operator=(const nsp_constclient&) = delete;
		
		const nsp_client<TDerived, TACTOR, T>& m_instance;
	public:
		explicit nsp_constclient(const nsp_client<TDerived, TACTOR, T>& anc) :
			m_instance(anc)
		{}

		const T* getconst(i64_actorid aactorid)
		{
			return m_instance.getconst(aactorid);
		}
	};
}//namespace ngl