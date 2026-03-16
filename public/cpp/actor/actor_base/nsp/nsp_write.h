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

#include "actor/actor_base/nsp/nsp.h"

#include <ranges>

namespace ngl
{
	template <typename TDerived, typename TACTOR, typename T>
	class nsp_write
	{
		static std::atomic<bool>										m_isregister;

		TDerived*														m_actor = nullptr;
		
		// # Automaticallyregisterhandleprotocol handling
		nsp_callback<T>													m_call;

		// # Responsible formanageto[nsp server]register node stateandmanagedataloadstate
		nsp_regload														m_regload;

		// # Node "whichdata"
		care_data														m_care;

		// # Node whichdata
		std::map<i64_actorid, care_data>								m_othercare;

		// # "Whichtype node" "whichdatafield"
		operator_field													m_operator_field;

		// # Alldata node
		std::set<i64_nodeid>											m_nodereadalls;

		// # Alldata node
		std::set<i64_nodeid>											m_nodewritealls;		

		std::map<i64_actorid, T>										m_data;

		// # Node/ nodeexit() need tonotifywhichnode(1, allnsp_servernode.2, allnsp_writenode)
		std::set<i64_actorid>											m_exit;

		// Datalist
		std::set<i64_dataid>											m_changeids;

		// Delete datalist
		std::set<i64_dataid>											m_delids;
	public:
		// # Getsingleton
		static nsp_write<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// # [ Alldata] andinitialize
		static nsp_write<TDerived, TACTOR, T>& instance_writeall(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers  			/* Read-onlywhichfield */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* Read-onlywhichfield */
		);

		// # [ Partialdata]createandinitialize
		static nsp_write<TDerived, TACTOR, T>& instance_writepart(
			TDerived* aactor
			, const std::set<i32_fieldnumber>& areadfieldnumbers			/* Read-onlywhichfield */
			, const std::set<i32_fieldnumber>& awritefieldnumbers			/* / Whichfield */
			, const std::set<i64_actorid>& areadids							/* Read-onlywhichdata */
			, const std::set<i64_actorid>& awriteids						/* / Whichdata */
		);

		// # Replacedataguid [actor_type]
		static i64_actorid to_actorid(i64_actorid adataid);

		// # Initialize
		void init();

		// # Setdata change callback
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		// # Setdata delete callback
		void set_deldatafun(const std::function<void(int64_t)>& afun);

		// # Setdataallloadsuccessful callback
		void set_loadfinishfun(const std::function<void()>& afun);

		// # Adddata
		T* add(i64_dataid adataid);

		// # Getspecifieddata(andautomaticallyset state )
		T* get(i64_dataid adataid);

		// # Getspecifieddata
		const T* getconst(i64_dataid adataid);

		// //////////////////// This this (prevent data) ////////////////////////
		// This [get_mapconst] [get]
		//std::map<i64_actorid, T>& get_map()
		//{
		//	std::transform(m_data.begin(), m_data.end(), std::inserter(m_changeids, m_changeids.end()),
		//		[](const auto& pair) { return pair.first; });
		//	return m_data;
		//}
		///////

		const std::map<i64_actorid, T>& get_mapconst();

		// # This nspmodule
		void exit();

		// # Messageend
		void change();
		static void change(i64_actorid aactorid);

		// # Datasynchronizehandle
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		// # To[nsp server]registerthis module
		void handle(TDerived*, const message<np_channel_check<T>>& adata);

		// # To[nsp server]registerthis module response
		void handle(TDerived*, const message<np_channel_register_reply<T>>& adata);

		// # Synchronizenewly added [operator_field]
		void handle(TDerived*, const message<np_channel_dataid_sync<T>>& adata);

		// # Nodeexit
		void handle(TDerived*, const message<np_channel_exit<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_write<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl