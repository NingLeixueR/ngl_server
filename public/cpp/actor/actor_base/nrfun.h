#pragma once

#include "handle_pram.h"
#include "actor_base.h"
#include "type.h"

#include <functional>
#include <map>

namespace ngl
{
	using tlogicfun		= std::function<void(actor_base*, i32_threadid, handle_pram&)>;
	using tnotfindfun	= std::function<void(i32_threadid, handle_pram&)>;

	struct nlogicfun
	{
		bool m_isdbload = false;
		tlogicfun m_fun = nullptr;
	};

	class nrfunbase
	{
		nrfunbase(const nrfunbase&) = delete;
		nrfunbase& operator=(const nrfunbase&) = delete;
	protected:
		std::map<i32_protocolnum, nlogicfun>	m_fun;				// key:协议号 value:处理方法
		tnotfindfun								m_notfindfun;		// 如果在m_fun没有查找到处理方法,则使用其处理
	public:
		nrfunbase() = default;

		//# 设置协议处理没有匹配的调用
		nrfunbase& set_notfindfun(const tnotfindfun& afun);

		//# 未匹配的协议调用
		void notfindfun(const actor_base* aactor, i32_threadid athreadid, handle_pram& apram)const;

		//# 协议处理
		bool handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram);
	};

	template <typename T>
	struct message
	{
	private:
		message() = delete;

		std::shared_ptr<T>	m_shared_data		= nullptr;	// 数据
		T*					m_original_data		= nullptr;	// 数据
		i32_threadid		m_thread			= 0;		// 线程id
		const pack*			m_pack				= nullptr;	// 如果消息来自网络，这个值不为空即为网络数据包
	public:
		inline message(i32_threadid athread, const pack* apack, std::shared_ptr<T>& adata) :
			m_thread(athread),
			m_pack(apack),
			m_shared_data(adata)
		{
		}

		inline message(i32_threadid athread, const pack* apack, T* adata) :
			m_thread(athread),
			m_pack(apack),
			m_original_data(adata)
		{
		}

		inline const T* get_data()const
		{
			if (m_shared_data != nullptr)
			{
				return m_shared_data.get();
			}
			return m_original_data;
		}

		inline const std::shared_ptr<T>& get_shared_data()const
		{
			return m_shared_data;
		}

		inline const pack* get_pack()const
		{
			return m_pack;
		}

		inline i32_threadid thread()const
		{
			return m_thread;
		}
	};

	template <typename TDerived, typename TPRAM>
	using Tfun = bool (TDerived::*)(const message<TPRAM>&);

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	class nrfun : 
		public nrfunbase
	{
		nrfun(const nrfun&) = delete;
		nrfun& operator=(const nrfun&) = delete;

		nrfun() = default;
	public:
		static nrfun<TDerived, TYPE>& instance()
		{
			static nrfun<TDerived, TYPE> ltemp;
			return ltemp;
		}

		//# 允许任意std::function<void(TTTDerived*, T&)>挂载到指定actor上
		template <typename TTTDerived, typename T, bool MASS>
		nrfun& rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, bool aisload = false);

		//# bool aisload = false 
		//# 是否允许db数据加载完成之前处理此消息
		template <typename TTTDerived, typename T, bool MASS>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# actor间消息处理
		template <typename TTTDerived, typename T, bool MASS>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload = false);

		//# actor间消息处理,不注册网络层
		template <typename TTTDerived, typename T, bool MASS>
		nrfun& rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# 注册转发协议
		template <bool BOOL, typename T>
		nrfun& rfun_forward(
			const Tfun<TDerived, np_actor_forward<T, TYPE, BOOL, ngl::forward>> afun
			, ENUM_ACTOR atype
			, bool aisload = false
		);

		//# 注册接收转发协议处理协议
		template <typename T>
		nrfun& rfun_recvforward(const Tfun<TDerived, T> afun, bool aisload = false);
	};
}//namespace ngl