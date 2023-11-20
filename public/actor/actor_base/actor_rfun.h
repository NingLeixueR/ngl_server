#pragma once

#include <functional>
#include <map>
#include "handle_pram.h"
#include "actor_base.h"
#include "type.h"

namespace ngl
{
	using tlogicfun = std::function<void(actor_base*, i32_threadid, handle_pram&)>;
	using tnotfindfun = std::function<void(i32_threadid, handle_pram&)>;

	struct alogicfun
	{
		bool m_isdbload = false;
		tlogicfun m_fun = nullptr;
	};

	class arfunbase
	{
	protected:
		std::map<i32_protocolnum, alogicfun> m_fun;
		tnotfindfun m_notfindfun;
	public:
		arfunbase& set_notfindfun(const tnotfindfun& afun);

		void notfindfun(actor_base* aactor, i32_threadid athreadid, handle_pram& apram);

		bool handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram);
	};


	template <typename TDerived, typename TPRAM>
	using Tfun = bool (TDerived::*)(i32_threadid, const std::shared_ptr<pack>&, TPRAM&);

	template <typename TDerived, typename TPRAM>
	using TMountfun = bool (*)(TDerived*, i32_threadid, const std::shared_ptr<pack>&, TPRAM&);

	

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	class arfun : public arfunbase
	{
		arfun() {}
	public:
		static arfun<TDerived, TYPE>& instance()
		{
			static arfun<TDerived, TYPE> ltemp;
			return ltemp;
		}

		template <typename T>
		arfun& rfun(const std::function<void(TDerived*, T&)>& afun);

		//// #### bool aisload = false 是否允许db数据加载完成之前处理此消息
		template <typename T>
		arfun& rfun(Tfun<TDerived, T> afun, bool aisload = false);

		template <typename T>
		arfun& rfun_nonet(Tfun<TDerived, T> afun, bool aisload = false);

		template <bool BOOL, typename T>
		arfun& rfun_forward(Tfun<TDerived, actor_forward<T, TYPE, BOOL, ngl::forward>> afun, ENUM_ACTOR atype, bool aisload = false);

		template <typename T>
		arfun& rfun_recvforward(Tfun<TDerived, T> afun, bool aisload = false);
	};


}