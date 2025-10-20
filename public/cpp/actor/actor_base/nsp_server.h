#pragma once

#include "ndb_modular.h"
#include "threadtools.h"
#include "pb_field.h"
#include "tools.h"
#include "type.h"

#include "ndb_modular.h"

namespace ngl
{
	template <typename TDerived>
	class nsp_handle_print
	{
	public:
		template <typename TRECV>
		static void print(const char* aname, TDerived* aactor, const TRECV* arecv)
		{
			TRECV* lrecv = (TRECV*)arecv;

			std::string lcustomstr;
			tools::custom2json(*lrecv, lcustomstr);
			log_error_net()->print(
				"{}::handle<{}>( actor({}) : {} )"
				, aname
				, tools::type_name<TRECV>()
				, nguid(aactor->id_guid())
				, lcustomstr
			);
		}

		template <typename TACTOR, typename TX>
		static void msg_info(TX& adata)
		{
			adata.m_msg = std::format(
				"{}:{}:{}"
				, tools::type_name<TDerived>()
				, tools::type_name<TACTOR>()
				, tools::type_name<typename TX::T>()
			);
		}
	};

	template <
		pbdb::ENUM_DB ENUMDB,	// ��������ö��
		typename TDerived,		// ���޵�actor
		typename T				// ��������
	>
	class nsp_server
	{
		// �����޸���Щ�ֶα��
		static std::map<i16_actortype, std::set<i32_fieldnumber>> m_noderead_fieldnumbers;
		static std::map<i16_actortype, std::set<i32_fieldnumber>> m_nodewrite_fieldnumbers;

		// (���ֶ�/д)���ݱ���Щ������
		static std::map<i64_dataid, std::map<i64_nodeid, enp_channel>> m_part;

		// ��ȫ�����ݵĽ��
		static std::set<i64_nodeid> m_nodereadalls;

		// дȫ�����ݵĽ��
		static std::set<i64_nodeid> m_nodewritealls; // ��Щ���дȫ������

		static std::set<i64_nodeid> m_nodepart;

		// ����
		static ndb_modular<ENUMDB, T, TDerived>* m_dbmodule;
	public:
		using tnsp_server = nsp_server<ENUMDB, TDerived, T>;

		// # ����ע�ᴦ��
		static void init(ndb_modular<ENUMDB, T, TDerived>* adbmodule);

		// # �����½�㣬�㲥���������
		static void broadcast_addnode(
			i64_nodeid anodeid
			, enp_channel atype
			, const std::set<i64_dataid>& areadids
			, const std::set<i64_dataid>& awriteids
			, const std::set<i32_fieldnumber>& afieldnumbers
		);

		static void sync_data(i64_nodeid anodeid);

		static bool check_map(const std::set<int32_t>& aset1, const std::set<int32_t>& aset2);

		// # ����ͬ��
		static void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		static void handle(TDerived*, const message<np_channel_read_register<T>>& adata);

		static void handle(TDerived*, const message<np_channel_write_register<T>>& adata);

		static void handle(TDerived*, const message<np_channel_exit<T>>& adata);
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::map<i16_actortype, std::set<i32_fieldnumber>> nsp_server<ENUMDB, TDerived, T>::m_noderead_fieldnumbers;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::map<i16_actortype, std::set<i32_fieldnumber>> nsp_server<ENUMDB, TDerived, T>::m_nodewrite_fieldnumbers;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::map<i64_dataid, std::map<i64_nodeid, enp_channel>> nsp_server<ENUMDB, TDerived, T>::m_part;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodereadalls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodewritealls;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	std::set<i64_nodeid> nsp_server<ENUMDB, TDerived, T>::m_nodepart;

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	ndb_modular<ENUMDB, T, TDerived>* nsp_server<ENUMDB, TDerived, T>::m_dbmodule;
}//namespace ngl

namespace ngl
{
	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::init(ndb_modular<ENUMDB, T, TDerived>* adbmodule)
	{
		m_dbmodule = adbmodule;

		actor::register_actor_s<TDerived, np_channel_data<T>>(
			[](TDerived* aactor, const message<np_channel_data<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);

		// # ����ע�ᴦ��
		actor::register_actor_s<TDerived, np_channel_read_register<T>>(
			[](TDerived* aactor, const message<np_channel_read_register<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);
		actor::register_actor_s<TDerived, np_channel_write_register<T>>(
			[](TDerived* aactor, const message<np_channel_write_register<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);
		
		// # �������ݱ��޸�
		actor::register_actor_s<TDerived, np_channel_data<T>>(
			[](TDerived* aactor, const message<np_channel_data<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);

		// # �˳�����
		actor::register_actor_s<TDerived, np_channel_exit<T>>(
			[](TDerived* aactor, const message<np_channel_exit<T>>& adata)
			{
				nsp_server<ENUMDB, TDerived, T>::handle(aactor, adata);
			}, true);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived* aactor, const message<np_channel_data<T>>& adata)
	{
		const np_channel_data<T>* recv = adata.get_data();
		const std::map<int64_t, T>& lmap = recv->m_data;
		for (const auto& lpair : lmap)
		{
			data_modified<T>& ldata = m_dbmodule->get(lpair.first);
			data_modified_continue_get(lpddata, ldata);
			pb_field::copy(lpair.second, &(*lpddata), m_nodewrite_fieldnumbers[nguid::type(recv->m_actorid)]);
		}
		for (int64_t dataid : recv->m_deldata)
		{
			m_dbmodule->erase(dataid);
		}
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::broadcast_addnode(
		i64_nodeid anodeid
		, enp_channel atype
		, const std::set<i64_dataid>& areadids
		, const std::set<i64_dataid>& awriteids
		, const std::set<i32_fieldnumber>& afieldnumbers
	)
	{
		if (!areadids.empty() && !awriteids.empty())
		{
			tools::no_core_dump();
		}

		auto pro = std::make_shared<np_channel_dataid_sync<T>>();
		pro->m_actorid = anodeid;
		pro->m_add = true;
		pro->m_type = atype;

		if (!areadids.empty())
		{
			pro->m_part = areadids;
		}
		if (!awriteids.empty())
		{
			pro->m_part = awriteids;
		}
		pro->m_fieldnumbers = afieldnumbers;

		std::set<i64_nodeid> lnodes;
		// ֪ͨ����ȫ��д���
		lnodes.insert(m_nodewritealls.begin(), m_nodewritealls.end());
		if (atype == enp_channel_readall || atype == enp_channel_writeall)
		{
			for (const auto& item1 : m_part)
			{
				for (const auto& item2 : item1.second)
				{
					if (item2.second == enp_channel_writepart)
					{
						lnodes.insert(item2.first);
					}
				}
			}
		}
		else
		{//֪ͨ����д���
			static auto lfun = [](const std::set<i64_dataid>& aids, std::set<i64_nodeid>& anodes)
				{
					for (i64_dataid dataid : aids)
					{
						auto itor = m_part.find(dataid);
						if (itor == m_part.end())
						{
							for (std::pair<const i64_nodeid, enp_channel>& item : itor->second)
							{
								if (item.second == enp_channel_writepart)
								{
									anodes.insert(item.first);
								}
							}
						}
					}
				};
			lfun(areadids, lnodes);
			lfun(awriteids, lnodes);
		}

		if (!lnodes.empty())
		{
			actor::send_actor(lnodes, nguid::make(), pro);
		}
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::sync_data(i64_nodeid anodeid)
	{
		std::map<nguid, data_modified<T>>& lmap = m_dbmodule->data();
		auto pro = std::make_shared<np_channel_data<T>>();
		pro->m_firstsynchronize = true;
		pro->m_recvfinish = false;
		if (m_nodereadalls.contains(anodeid) || m_nodewritealls.contains(anodeid))
		{
			int32_t lconnt = 0;
			for (std::pair<const nguid, data_modified<T>>& item : lmap)
			{
				if (++lconnt >= 100)
				{
					actor::send_actor(anodeid, nguid::make(), pro);
					pro = std::make_shared<np_channel_data<T>>();
					pro->m_firstsynchronize = true;
					pro->m_recvfinish = false;
					lconnt = 0;
				}
				pro->m_data[item.first] = *item.second.getconst();
			}
			if (lconnt > 0)
			{
				pro->m_recvfinish = true;
				actor::send_actor(anodeid, nguid::make(), pro);
			}
		}
		else
		{
			// (���ֶ�/д)���ݱ���Щ������
			std::set<i64_dataid> ldatas;
			int32_t lconnt = 0;
			for (const auto& item1 : m_part)
			{
				if (item1.second.contains(anodeid))
				{
					ldatas.insert(item1.first);
				}
			}
			for (i64_dataid dataid : ldatas)
			{
				auto itor = lmap.find(dataid);
				if (itor != lmap.end())
				{
					data_modified<T>& ldatamodf = itor->second;
					if (++lconnt >= 100)
					{
						actor::send_actor(anodeid, nguid::make(), pro);
						pro = std::make_shared<np_channel_data<T>>();
						pro->m_firstsynchronize = true;
						pro->m_recvfinish = false;
						lconnt = 0;
					}
					pro->m_data[dataid] = *ldatamodf.getconst();
				}
			}
			if (lconnt > 0)
			{
				pro->m_recvfinish = true;
				actor::send_actor(anodeid, nguid::make(), pro);
			}
		}
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived* aactor, const message<np_channel_read_register<T>>& adata)
	{
		const np_channel_read_register<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_server", aactor, recv);

		if (m_nodereadalls.contains(recv->m_actorid))
		{
			return;
		}
		if (m_nodewritealls.contains(recv->m_actorid))
		{
			return;
		}
		if (m_nodepart.contains(recv->m_actorid))
		{
			return;
		}

		if (recv->m_type == enp_channel_readall)
		{
			m_nodereadalls.insert(recv->m_actorid);
		}
		else
		{
			m_nodepart.insert(recv->m_actorid);
			for (i64_dataid dataid : recv->m_readids)
			{
				m_part[dataid][recv->m_actorid] = enp_channel_readpart;
			}
		}

		{// �ظ�
			auto pro = std::make_shared<np_channel_read_register_reply<T>>();
			pro->m_actorid = m_dbmodule->get_actor()->id_guid();
			pro->m_node_fieldnumbers = m_nodewrite_fieldnumbers;
			actor::send_actor(recv->m_actorid, nguid::make(), pro);
		}
		
		m_noderead_fieldnumbers[nguid::type(recv->m_actorid)] = recv->m_fieldnumbers;
		broadcast_addnode(recv->m_actorid, recv->m_type, recv->m_readids, {}, recv->m_fieldnumbers);

		sync_data(recv->m_actorid);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	bool nsp_server<ENUMDB, TDerived, T>::check_map(const std::set<int32_t>& aset1, const std::set<int32_t>& aset2)
	{
		if (aset1.size() < aset2.size())
		{
			for (int32_t lid : aset1)
			{
				if (aset2.contains(lid))
				{
					return true;
				}
			}
			return false;
		}
		else
		{
			return check_map(aset2, aset1);
		}
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	class actortypes_fieldnumbers_check
	{
		std::set<i16_actortype>		m_actortypes;
		const std::set<int32_t>&	m_fieldnumbers; // ���޸���Щ�ֶα��
		std::map<i16_actortype, std::set<i32_fieldnumber>>& m_nodewrite_fieldnumbers;
	public:
		actortypes_fieldnumbers_check(
			const std::set<int32_t>& afieldnumbers
			, std::map<i16_actortype, std::set<i32_fieldnumber>>& anodewrite_fieldnumbers
		) :
			m_fieldnumbers(afieldnumbers)
			, m_nodewrite_fieldnumbers(anodewrite_fieldnumbers)
		{}

		void insert(i64_nodeid anodeid)
		{
			m_actortypes.insert(nguid::type(anodeid));
		}

		void check()
		{
			for (i16_actortype ltype : m_actortypes)
			{
				if (!m_nodewrite_fieldnumbers.contains(ltype))
				{
					tools::no_core_dump();
				}
			}
			m_actortypes.clear();
		}
	};

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived* aactor, const message<np_channel_write_register<T>>& adata)
	{
		const np_channel_write_register<T>* recv = adata.get_data();

		nsp_handle_print<TDerived>::print("nsp_server", aactor, recv);

		if (m_nodereadalls.contains(recv->m_actorid))
		{
			return;
		}
		if (m_nodewritealls.contains(recv->m_actorid))
		{
			return;
		}
		if (m_nodepart.contains(recv->m_actorid))
		{
			return;
		}

		// ���id ���ֶ����
		actortypes_fieldnumbers_check<ENUMDB, TDerived, T> lcheck(recv->m_fieldnumbers, m_nodewrite_fieldnumbers);
		for (i64_nodeid nodeid : m_nodewritealls)
		{
			lcheck.insert(nodeid);
		}
		lcheck.check();

		if (recv->m_type == enp_channel_writeall)
		{// ȫ����д
			// (���ֶ�/д)���ݱ���Щ������
			for (const auto& item1 : m_part)
			{
				for (const auto& item2 : item1.second)
				{
					if (item2.second == enp_channel_writepart)
					{
						lcheck.insert(item2.first);
					}
				}
			}
			lcheck.check();
			// дȫ�����ݵĽ��
			m_nodewritealls.insert(recv->m_actorid);
		}
		else
		{// ���ֿ�д
			m_nodepart.insert(recv->m_actorid);
			for (i64_actorid dataid : recv->m_writeids)
			{
				// (���ֶ�/д)���ݱ���Щ������
				auto itor = m_part.find(dataid);
				if (itor != m_part.end())
				{
					for (const auto& item : itor->second)
					{
						lcheck.insert(item.first);
					}
				}
				m_part[dataid][recv->m_actorid] = enp_channel_writepart;
			}
			lcheck.check();
		}

		m_nodewrite_fieldnumbers[nguid::type(recv->m_actorid)] = recv->m_fieldnumbers;

		{//�ظ�
			auto pro = std::make_shared<np_channel_write_register_reply<T>>();
			pro->m_actorid = m_dbmodule->get_actor()->id_guid();
			pro->m_nodereadalls = m_nodewritealls;
			pro->m_nodewritealls = m_nodereadalls;
			pro->m_node_fieldnumbers = m_nodewrite_fieldnumbers;
			if (recv->m_type == enp_channel_writeall)
			{
				// ���ֶ�/д
				pro->m_part = m_part;
			}
			else
			{
				for (i64_actorid dataid : recv->m_writeids)
				{
					pro->m_part[dataid] = m_part[dataid];
				}
			}
			actor::send_actor(recv->m_actorid, nguid::make(), pro);
		}

		broadcast_addnode(recv->m_actorid, recv->m_type, {}, recv->m_writeids, recv->m_fieldnumbers);

		sync_data(recv->m_actorid);
	}

	template <pbdb::ENUM_DB ENUMDB, typename TDerived, typename T>
	void nsp_server<ENUMDB, TDerived, T>::handle(TDerived* aactor, const message<np_channel_exit<T>>& adata)
	{
		const np_channel_exit<T>* recv = adata.get_data();
		nsp_handle_print<TDerived>::print("nsp_server", aactor, recv);

		{
			auto itor = m_nodereadalls.find(recv->m_actorid);
			if (itor != m_nodereadalls.end())
			{
				m_nodereadalls.erase(itor);
				return;
			}
		}
		{
			auto itor = m_nodewritealls.find(recv->m_actorid);
			if (itor != m_nodewritealls.end())
			{
				m_nodewritealls.erase(itor);
				return;
			}
		}
		{
			auto itor = m_nodepart.find(recv->m_actorid);
			if (itor != m_nodepart.end())
			{
				m_nodepart.erase(itor);
				for (auto itor = m_part.begin(); itor != m_part.end(); ++itor)
				{
					itor->second.erase(recv->m_actorid);
				}
				return;
			}
		}
	}

	template <typename T>
	class nsp_instance
	{
	private:
		static std::map<i64_actorid, std::shared_ptr<T>>	m_instance;
		static std::mutex									m_mutex;
	public:
		static T* nclient(i64_actorid aactorid)
		{
			monopoly_lock(m_mutex);
			auto itor = m_instance.find(aactorid);
			if (itor == m_instance.end())
			{
				tools::no_core_dump();
				return nullptr;
			}
			return itor->second.get();
		}

		static T* init(i64_actorid aactorid, std::shared_ptr<T>& athis)
		{
			monopoly_lock(m_mutex);
			if (m_instance.contains(aactorid))
			{
				tools::no_core_dump();
			}
			m_instance[aactorid] = athis;
			return athis.get();
		}

		static void exit(i64_actorid aactorid)
		{
			monopoly_lock(m_mutex);
			m_instance.erase(aactorid);
		}

		template <typename TDerived, typename TMESSAGE>
		static void register_handle()
		{
			actor::register_actor_s<TDerived, TMESSAGE>([](TDerived* aacotor, const message<TMESSAGE>& adata)
				{
					nclient(aacotor->id_guid())->handle(aacotor, adata);
				}, false);
		}
	};

	template <typename T>
	std::map<i64_actorid, std::shared_ptr<T>> nsp_instance<T>::m_instance;

	template <typename T>
	std::mutex nsp_instance<T>::m_mutex;

	template <typename T>
	class nsp_callback
	{
		template <typename TDATA>
		struct tcallback
		{
			std::function<void(int64_t, const TDATA&, bool)>				m_changedatafun = nullptr;	// [�ص�] �����ݷ����仯
			std::function<void(int64_t)>									m_deldatafun = nullptr;		// [�ص�] �����ݱ�ɾ��
			std::function<void()>											m_loadfinishfun = nullptr;	// [�ص�] ���ݼ������
		};
		std::map<int64_t, tcallback<T>> m_call;
	public:
		void set_changedatafun(int64_t aid, const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call[aid].m_changedatafun = afun;
		}

		void set_deldatafun(int64_t aid, const std::function<void(int64_t)>& afun)
		{
			m_call[aid].m_deldatafun = afun;
		}

		void set_loadfinishfun(int64_t aid, const std::function<void()>& afun)
		{
			m_call[aid].m_loadfinishfun = afun;
		}

		void changedatafun(int64_t aid, const T& adata, bool afrist)
		{
			for (auto& item : m_call)
			{
				item.second.m_changedatafun(aid, adata, afrist);
			}
		}

		void deldatafun(int64_t aid)
		{
			for (auto& item : m_call)
			{
				item.second.m_deldatafun(aid);
			}
		}

		void loadfinishfun()
		{
			for (auto& item : m_call)
			{
				item.second.m_loadfinishfun();
			}
		}
	};
}//namespace ngl