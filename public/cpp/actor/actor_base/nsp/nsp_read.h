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
	class nsp_read
	{
	public:
		using tnsp_read = nsp_read<TDerived, TACTOR, T>;
	private:
		static std::atomic<bool>										m_isregister;
				
		// # This nspmodule actor
		TDerived*														m_actor = nullptr;

		// # Automaticallyregisterhandleprotocol handling
		nsp_callback<T>													m_call;

		// # Responsible formanageto[nsp server]register node stateandmanagedataloadstate
		nsp_regload														m_regload;

		// # Node "whichdata"
		care_data														m_care;

		// # "Whichnode" "whichdatafield"
		operator_field													m_operator_field;

		// # Node/ nodeexit() need tonotifywhichnode(1, allnsp_servernode.2, allnsp_writenode)
		std::set<i64_actorid>											m_exit;

		// # Data
		std::map<i64_actorid, T>										m_data;
	public:
		// # Getsingleton
		static nsp_read<TDerived, TACTOR, T>& instance(i64_actorid aactorid);

		// # [ Alldata] getsingleton- createandinitialize
		static nsp_read<TDerived, TACTOR, T>& instance_readall(TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers);

		// # [ Partialdata] getsingleton- createandinitialize
		static nsp_read<TDerived, TACTOR, T>& instance_readpart(
			TDerived* aactor, const std::set<i32_fieldnumber>& afieldnumbers, const std::set<i64_actorid>& aids
		);

		// # Replacedataguid [actor_type]
		static i64_actorid to_actorid(i64_actorid adataid);

		// # Setdata change callback
		void set_changedatafun(const std::function<void(int64_t, const T&, bool)>& afun);

		// # Setdata delete callback
		void set_deldatafun(const std::function<void(int64_t)>& afun);

		// # Setdataallloadsuccessful callback
		void set_loadfinishfun(const std::function<void()>& afun);

		// # Initialize
		void init();

		// # This nspmodule
		void exit();

		// # Getspecifieddata
		const T* getconst(i64_dataid adataid);

		// # Get alldata
		const std::map<i64_actorid, T>& get_mapconst();

		// # Datasynchronizehandle
		void handle(TDerived* aactor, const message<np_channel_data<T>>& adata);

		// # To[nsp server]registerthis module
		void handle(TDerived* aactor, const message<np_channel_check<T>>& adata);

		// # To[nsp server]registerthis module response
		void handle(TDerived* aactor, const message<np_channel_register_reply<T>>& adata);

		// # Synchronizenewly added [operator_field]
		void handle(TDerived* aactor, const message<np_channel_dataid_sync<T>>& adata);
	};

	template <typename TDerived, typename TACTOR, typename T>
	std::atomic<bool> nsp_read<TDerived, TACTOR, T>::m_isregister = true;
}//namespace ngl