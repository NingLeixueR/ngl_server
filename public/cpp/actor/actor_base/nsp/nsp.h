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
// File overview: Declares interfaces for nsp.

#pragma once 

#include "actor/protocol/nprotocol_template.h"
#include "actor/tab/ttab_servers.h"
#include "actor/actor_base/nguid.h"
#include "actor/actor_base/actor.h"
#include "tools/pb_field.h"

namespace ngl
{
	// # Nsp client pack nsp_readandnsp_write
	// # Nsp client load
	class nsp_regload
	{
		// # Need toconnection nspserverlist
		std::map<i16_area, i64_actorid>		m_nspserver;
		// # Registerstate
		std::map<i16_area, bool>			m_register;
		// # Loadstate
		std::map<i16_area, bool>			m_loadfinish;
	public:
		void init(i64_actorid aactorid);

		// # Set[area]completeregister
		void set_register(i16_area aarea);

		// # [Area]whethercompleteregister
		bool is_register(i16_area aarea)const;

		// # All[area]whether completeregister
		bool is_register()const;

		// # Set[area]loaddatacomplete
		void set_loadfinish(i16_area aarea);

		// # [Area]whetherloaddatacomplete
		bool is_loadfinish(i16_area aarea)const;

		// # All[area]whether loaddatacomplete
		bool is_loadfinish()const;

		// # [Area]getnsp server
		i64_actorid nspserid(i16_area aarea)const;

		// # Allneed toconnection nsp server
		void foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const;
	};

	// # "Datafield"
	class operator_field
	{
		// # Nep serverset "because fieldset /, first, "
		bool m_nspserver = false;
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;
	public:
		// # Initialize
		void init(bool anspserver);

		// # Set"datafield"
		void set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers);

		// # Set
		void set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers);

		// # Add
	private:
		void nspser_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
		void nspcli_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
	public:
		void add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype);
	
		template <typename T>
		void add_field(i16_actortype atype, epb_field afieldtype, const std::set<i32_fieldnumber>& afield)
		{
			if (afieldtype == epb_field_read && afield.empty())
			{// [Awritefieldnumbers], allfield
				std::set<i32_fieldnumber> lreadfield;
				pb_field::field_numbers<T>(lreadfield);
				for (i32_fieldnumber field : lreadfield)
				{
					add_field(atype, field, afieldtype);
				}
			}
			else
			{
				if (afield.empty())
				{// Afieldtype == epb_field_write do not allow[afield]
					tools::no_core_dump();
				}
				for (i32_fieldnumber field : afield)
				{
					add_field(atype, field, afieldtype);
				}
			}
		}

		template <typename T>
		void add_field(i16_actortype atype, const std::set<i32_fieldnumber>& areadfield, const std::set<i32_fieldnumber>& awritefield)
		{
			std::set<i32_fieldnumber> lreadfield;
			if (areadfield.empty())
			{// [Awritefieldnumbers], allfield
				pb_field::field_numbers<T>(lreadfield);
			}
			for (i32_fieldnumber field : awritefield)
			{
				add_field(atype, field, epb_field_write);
			}
			for (i32_fieldnumber field : (lreadfield.empty() ? areadfield : lreadfield))
			{
				add_field(atype, field, epb_field_read);
			}
		}

		// # Typeget
		std::map<i32_fieldnumber, epb_field>* get_field(i16_actortype atype);

		// # Fieldtype datacopy
		template <typename T>
		bool field_copy(i16_actortype atypesource, i16_actortype atypetarget, const T& asource, T& atarget, bool amessage)
		{
			std::map<i32_fieldnumber, epb_field>* lpsource = get_field(atypesource);
			std::map<i32_fieldnumber, epb_field>* lptarget = get_field(atypetarget);
			if (lpsource == nullptr || lptarget == nullptr)
			{
				return false;
			}
			pb_field::copy(asource, &atarget, *lpsource, *lptarget, amessage);
			return true;
		}

		// # Fieldtype datacopy
		template <typename T>
		bool field_copy(i16_actortype atype, const T& asource, T& atarget, bool amessage)
		{
			std::map<i32_fieldnumber, epb_field>* lp = get_field(atype);
			if (lp == nullptr)
			{
				return false;
			}
			pb_field::copy(asource, &atarget, *lp, amessage);
			return true;
		}

		// # Getallarea datafield
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& field_numbers();
	};

	// # Whichdata
	class care_data
	{
		nsp_care m_core;
	public:
		// # "All,all "
		void init(bool aread);

		// # "Partial "
		void init(const std::set<i64_actorid>& aids);

		// # "Partial,partial "
		void init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids);

		// # "All,partial "
		void init(bool aread, const std::set<i64_actorid>& awriteids);

		// # Nsp_caredatainitialize
		void init(const nsp_care& acore);

		// # Whether
		bool is_care(i64_actorid adataid)const;

		// # Whether
		bool is_read(i64_actorid adataid)const;

		// # Whether ( )
		bool is_write(i64_actorid adataid)const;

		// # Whether alldata
		bool is_readall()const;

		// # Whether alldata
		bool is_writeall()const;

		// # List
		std::set<i64_actorid>& readids();

		// # List
		std::set<i64_actorid>& writeids();

		// # Get data
		const nsp_care& get_core()const;
	};

	template <typename T>
	class nsp_instance
	{
	private:
		static std::map<i64_actorid, std::shared_ptr<T>>	m_instance;
		static std::shared_mutex							m_mutex;
	public:
		static T* nclient(i64_actorid aactorid, bool adump)
		{
			lock_read(m_mutex);
			auto lpinstance = tools::findmap(m_instance, aactorid);
			if (lpinstance == nullptr)
			{
				if (adump)
				{
					tools::no_core_dump();
				}
				return nullptr;
			}
			return lpinstance->get();
		}

		static T* init(i64_actorid aactorid, std::shared_ptr<T>& athis)
		{
			lock_write(m_mutex);
			auto [it, inserted] = m_instance.emplace(aactorid, athis);
			if (!inserted)
			{
				tools::no_core_dump();
			}
			return it->second.get();
		}

		static void exit(i64_actorid aactorid)
		{
			lock_write(m_mutex);
			m_instance.erase(aactorid);
		}

		template <typename TDerived, typename ...TMESSAGES>
		static void register_handle()
		{
			(actor::register_actor_s<TDerived, TMESSAGES>(e_ready_db, [](TDerived* aacotor, const message<TMESSAGES>& adata)
				{
					T* lpclass = nclient(aacotor->id_guid(), true);
					if (lpclass != nullptr)
					{
						lpclass->handle(aacotor, adata);
					}
				} 
			), ...);
		}
	};

	// Nsp client registercallback
	template <typename T>
	class nsp_callback
	{
		template <typename TDATA>
		struct tcallback
		{
			// [Callback] data change
			std::function<void(int64_t, const TDATA&, bool)>				m_changedatafun = nullptr;
			// [Callback] data delete
			std::function<void(int64_t)>									m_deldatafun = nullptr;
			// [Callback] dataloadcomplete
			std::function<void()>											m_loadfinishfun = nullptr;
		};
		tcallback<T> m_call;
	public:
		// # Setdata or getdata callback
		inline void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun)
		{
			m_call.m_changedatafun = afun;
		}

		// # Setdata delete callback
		inline void set_deldatafun(const std::function<void(int64_t)>& afun)
		{
			m_call.m_deldatafun = afun;
		}

		// # Setdataloadcomplete callback
		inline void set_loadfinishfun(const std::function<void()>& afun)
		{
			m_call.m_loadfinishfun = afun;
		}

		// # Data or getdata callback
		inline void changedatafun(int64_t aid, const T& adata, bool afrist)
		{
			if (m_call.m_changedatafun != nullptr)
			{
				m_call.m_changedatafun(aid, adata, afrist);
			}
		}

		// # Data delete callback
		inline void deldatafun(int64_t aid)
		{
			if (m_call.m_deldatafun != nullptr)
			{
				m_call.m_deldatafun(aid);
			}
		}

		// # Dataloadcomplete callback
		inline void loadfinishfun()
		{
			if (m_call.m_loadfinishfun != nullptr)
			{
				m_call.m_loadfinishfun();
			}
		}
	};

	template <typename TDerived>
	class nsp_handle_print
	{
	public:
		template <typename TRECV>
		static void print(const char* aname, const TDerived* aactor, const TRECV* arecv)
		{
			std::string lcustomstr;
			tools::custom2json(*arecv, lcustomstr);
			log_error()->print("{}::handle<{}>( actor({}) : {} )", aname, tools::type_name<TRECV>(), nguid(aactor->id_guid()), lcustomstr);
		}

		template <typename TACTOR, typename TX>
		static void msg_info(TX& adata)
		{
			adata.m_msg = std::format("{}:{}:{}", tools::type_name<TDerived>(), tools::type_name<TACTOR>(), tools::type_name<typename TX::T>());
		}
	};
}//namespace ngl

namespace ngl
{
	template <typename T>
	std::map<i64_actorid, std::shared_ptr<T>> nsp_instance<T>::m_instance;

	template <typename T>
	std::shared_mutex nsp_instance<T>::m_mutex;
}//namespace ngl