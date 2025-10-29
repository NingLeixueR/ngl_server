#pragma once

#include "ndbclient.h"
#include "csvtable.h"
#include "nlog.h"

namespace ngl
{
	class ndb_component
	{
		ndb_component() = delete;
		ndb_component(const ndb_component&) = delete;
		ndb_component& operator=(const ndb_component&) = delete;

	private:
		actor_base*				m_actor		= nullptr;							// ����actor
		i64_actorid				m_id		= 0;								// ����actor id
		ndbclient_base*			m_dbclient	= nullptr;							// ndbclient����
		pbdb::ENUM_DB			m_type		= pbdb::ENUM_DB::ENUM_DB_FAIL;		// ��������

	protected:
		explicit ndb_component(pbdb::ENUM_DB aenum);

		//# ����ndbclient����
		void set_dbclient(ndbclient_base* andbclient);

		//# ��ȡndbclient����
		inline ndbclient_base* get_dbclient();
	public:
		//# ��������actor
		void				set(actor_base* aactor);

		// # ��ȡactor
		actor_base*			get_actor();

		//# ��ȡ����actor id
		i64_actorid			get_actorid()const;

		//# ��ȡ��������
		pbdb::ENUM_DB		type()const;

		//# ��ȡ����actor
		actor_base*			actorbase();

		//# ������related_actorid,�����Զ���actor id
		void				set_actorid(i64_actorid aactorid);

		//# ����m_id����m_actor
		virtual void		related_actorid();

		//# ����m_actor����m_dbclient
		void				init();

		//# m_dbclient.create()
		void				create();

		//# ��ȡm_dbclient
		ndbclient_base*		dbclientbase();

		// # ������ȫ�����غ����
		virtual void		init_data();
	};

	template <pbdb::ENUM_DB ENUM, typename TDATA, typename TACTOR>
	class ndb_modular : 
		public ndb_component
	{
		ndb_modular(const ndb_modular&) = delete;
		ndb_modular& operator=(const ndb_modular&) = delete;

	protected:
		ndbclient<ENUM, TDATA, TACTOR> m_data;

		inline ndb_modular():
			ndb_component(ENUM)
		{
			set_dbclient(&m_data);
		}

		virtual ~ndb_modular() = default;
	public:
		inline ndbclient<ENUM, TDATA, TACTOR>* dbclient()
		{ 
			return &m_data;
		}

		//# ����data
		inline void foreach(const std::function<void(data_modified<TDATA>&)>& afun)
		{
			for (std::pair<const nguid, data_modified<TDATA>>& apair : data())
			{
				afun(apair.second);
			}
		}

		//# ����ָ������
		inline data_modified<TDATA>* find(const std::function<bool(data_modified<TDATA>&)>& afun)
		{
			for (std::pair<const nguid, data_modified<TDATA>>& apair : data())
			{
				if (afun(apair.second))
				{
					return &apair.second;
				}
			}
			return nullptr;
		}

		//# ��ȡ��������
		inline std::map<nguid, data_modified<TDATA>>& data()
		{
			return m_data.get_data();
		}

		//# ��ȡm_id��Ӧ������
		inline data_modified<TDATA>& get()
		{
			return get(get_actorid());
		}

		// # ����ָ������
		inline data_modified<TDATA>* find(nguid aid)
		{
			return tools::findmap(data(), aid);
		}

		// # ��find����(ֻ��û�о����)
		inline data_modified<TDATA>& get(nguid aid)
		{
			if (aid == nguid::make())
			{
				tools::no_core_dump();
			}
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			data_modified<TDATA>& ldata = data()[aid];
			TDATA* lpdata = ldata.get(false, false);
			if (lpdata == nullptr)
			{
				tools::no_core_dump();
			}
			lpdata->set_mid(aid);
			return ldata;
		}

		// # ɾ��ָ������
		inline void erase(nguid aid)
		{
			get_actor_dbclient()->del(aid, true);
		}

		// # ���ر�������
		inline data_modified<TDATA>* db()
		{ 
			return m_data.get_dbdata();
		}

		// # ��ȡ����actor
		inline TACTOR* nactor()
		{ 
			return (TACTOR*)get_actor();
		}

		// # ��������
		inline void create()
		{
			m_id = m_actor->id_guid();
			m_data.set_id(m_id);
			m_data.set_actor(m_actor);
			m_data.add(m_id, TDATA());
		}

		// # �������
		inline void clear()
		{
			get_actor_dbclient()->clear();
		}

		// # ������ȫ�����غ����
		void init_data() final;

		virtual void initdata() = 0;

		inline ndbclient<ENUM, TDATA, TACTOR>* get_actor_dbclient()
		{
			return (ndbclient<ENUM, TDATA, TACTOR>*)get_dbclient();
		}

		inline data_modified<TDATA>& add(i64_actorid aid, const TDATA& adbtab)
		{
			if (get_actorid() != nguid::make() && get_actorid() != aid)
			{
				tools::no_core_dump();
			}
			data_modified<TDATA>& ldata = get(aid);
			*ldata.get() = adbtab;
			return ldata;
		}

		inline void remove(int64_t aid)
		{
			get_actor_dbclient()->del(aid);
		}
	};
}//namespace ngl