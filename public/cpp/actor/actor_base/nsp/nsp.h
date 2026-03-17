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
	// Tracks which NSP servers this actor must register with and whether the
	// initial per-area synchronization has completed.
	class nsp_regload
	{
		// Area -> NSP server actor id for that area.
		std::map<i16_area, i64_actorid>		m_nspserver;
		// Area -> registration acknowledged.
		std::map<i16_area, bool>			m_register;
		// Area -> first full synchronization completed.
		std::map<i16_area, bool>			m_loadfinish;
	public:
		// Build the area -> NSP server list based on the actor type encoded in aactorid.
		void init(i64_actorid aactorid);

		// Mark one area as registered.
		void set_register(i16_area aarea);

		// Return whether one area has acknowledged registration.
		bool is_register(i16_area aarea)const;

		// Return whether every target area has acknowledged registration.
		bool is_register()const;

		// Mark one area as fully synchronized.
		void set_loadfinish(i16_area aarea);

		// Return whether one area has completed the first full sync.
		bool is_loadfinish(i16_area aarea)const;

		// Return whether every area has completed the first full sync.
		bool is_loadfinish()const;

		// Resolve the NSP server actor id for one area.
		i64_actorid nspserid(i16_area aarea)const;

		// Iterate over every target NSP server.
		void foreach_nspser(const std::function<void(i16_area, i64_actorid)>& afun)const;
	};

	// Stores per-actor-type field permissions so NSP can copy only the allowed
	// protobuf fields between reader/writer/server nodes.
	class operator_field
	{
		// Server and client merge field permissions differently.
		bool m_nspserver = false;
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>> m_node_fieldnumbers;
	public:
		// Reset field rules and switch between server-side and client-side merge policy.
		void init(bool anspserver);

		// Merge one actor type's field permissions into the table.
		void set_field(i16_actortype atype, const std::map<i32_fieldnumber, epb_field>& anode_fieldnumbers);

		// Merge a full actor-type -> field-permission table.
		void set_field(const std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& anode_fieldnumbers);

	private:
		// Server-side merge keeps strict field-role consistency.
		void nspser_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
		// Client-side merge allows write permission to dominate read permission.
		void nspcli_add_field(std::map<i32_fieldnumber, epb_field>& afieldmap, i32_fieldnumber afieldnumber, epb_field afieldtype);
	public:
		// Add one field permission for one actor type.
		void add_field(i16_actortype atype, i32_fieldnumber afieldnumber, epb_field afieldtype);
	
		template <typename T>
		void add_field(i16_actortype atype, epb_field afieldtype, const std::set<i32_fieldnumber>& afield)
		{
			if (afieldtype == epb_field_read && afield.empty())
			{
				// Empty read sets mean "subscribe to every protobuf field".
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
				{
					// Write permissions must be explicit.
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
			{
				// Empty read sets mean "subscribe to every protobuf field".
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

		// Return the field-permission table for one actor type.
		std::map<i32_fieldnumber, epb_field>* get_field(i16_actortype atype);

		// Copy only the fields allowed by the source/target permission tables.
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

		// Copy using one actor type's field-permission table.
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

		// Expose the complete actor-type -> field-permission table.
		std::map<i16_actortype, std::map<i32_fieldnumber, epb_field>>& field_numbers();
	};

	// Describes which rows a subscriber cares about and whether it is read-only
	// or read/write for those rows.
	class care_data
	{
		nsp_care m_core;
	public:
		// Subscribe to all rows as read-only or read/write.
		void init(bool aread);

		// Subscribe read-only to a specific set of rows.
		void init(const std::set<i64_actorid>& aids);

		// Subscribe read-only/write to explicit row sets.
		void init(const std::set<i64_actorid>& areadids, const std::set<i64_actorid>& awriteids);

		// Subscribe to all rows for reading but only a subset for writing.
		void init(bool aread, const std::set<i64_actorid>& awriteids);

		// Copy from an existing wire-format care descriptor.
		void init(const nsp_care& acore);

		// Return whether the subscriber should receive this row at all.
		bool is_care(i64_actorid adataid)const;

		// Return whether the subscriber may read this row.
		bool is_read(i64_actorid adataid)const;

		// Return whether the subscriber may write this row.
		bool is_write(i64_actorid adataid)const;

		// Return whether reads cover all rows.
		bool is_readall()const;

		// Return whether writes cover all rows.
		bool is_writeall()const;

		// Return the explicit read set.
		std::set<i64_actorid>& readids();

		// Return the explicit write set.
		std::set<i64_actorid>& writeids();

		// Expose the underlying wire-format descriptor.
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
			// Register once per concrete NSP helper type and forward all messages
			// to the per-actor singleton instance.
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

	// Callback bundle used by NSP reader/writer helpers to notify higher-level code.
	template <typename T>
	class nsp_callback
	{
		template <typename TDATA>
		struct tcallback
		{
			// Data created/updated callback.
			std::function<void(int64_t, const TDATA&, bool)>				m_changedatafun = nullptr;
			// Data deleted callback.
			std::function<void(int64_t)>									m_deldatafun = nullptr;
			// Initial full synchronization completed callback.
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
