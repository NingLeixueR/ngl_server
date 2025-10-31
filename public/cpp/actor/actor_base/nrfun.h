/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* 项目名称：ngl_server
* 项目地址：https://github.com/NingLeixueR/ngl_server
* 
* 本文件是 ngl_server 项目的一部分，遵循 MIT 开源协议发布。
* 您可以按照协议规定自由使用、修改和分发本项目，包括商业用途，
* 但需保留原始版权和许可声明。
* 
* 许可详情参见项目根目录下的 LICENSE 文件：
* https://github.com/NingLeixueR/ngl_server/blob/main/LICENSE
*/
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

		std::shared_ptr<T>		m_shared_data		= nullptr;	// 数据
		T*						m_original_data		= nullptr;	// 数据
		i32_threadid			m_thread			= 0;		// 线程id
		std::shared_ptr<pack>	m_pack				= nullptr;	// 如果消息来自网络，这个值不为空即为网络数据包
	public:
		inline message(i32_threadid athread, const std::shared_ptr<pack>& apack, const std::shared_ptr<T>& adata) :
			m_thread(athread),
			m_pack(apack),
			m_shared_data(adata)
		{
		}

		inline message(i32_threadid athread, std::shared_ptr<pack> apack, T* adata) :
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
			return m_pack.get();
		}

		inline const std::shared_ptr<pack>& get_shared_pack()const
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

	template <typename TDerived>
	class nrfun : public nrfunbase
	{
		nrfun(const nrfun&) = delete;
		nrfun& operator=(const nrfun&) = delete;

		nrfun() = default;
	public:
		static nrfun<TDerived>& instance()
		{
			static nrfun<TDerived> ltemp;
			return ltemp;
		}

		//# 允许任意std::function<void(TTTDerived*, T&)>挂载到指定actor上
		template <typename TTTDerived, typename T>
		nrfun& rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, bool aisload = false);

		//# bool aisload = false 
		//# 是否允许db数据加载完成之前处理此消息
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# actor间消息处理
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload = false);

		//# actor间消息处理,不注册网络层
		template <typename TTTDerived, typename T>
		nrfun& rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# gateway注册c2g接收转发协议处理协议
		template <typename T>
		nrfun& rfun_c2g(const Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>> afun);

		//# gateway注册g2c接收转发协议处理协议
		template <typename T>
		nrfun& rfun_g2c(const Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>> afun);		
	};
}//namespace ngl