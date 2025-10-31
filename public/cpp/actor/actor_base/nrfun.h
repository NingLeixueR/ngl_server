/*
* Copyright (c) [2020-2025] NingLeixueR
* 
* ��Ŀ���ƣ�ngl_server
* ��Ŀ��ַ��https://github.com/NingLeixueR/ngl_server
* 
* ���ļ��� ngl_server ��Ŀ��һ���֣���ѭ MIT ��ԴЭ�鷢����
* �����԰���Э��涨����ʹ�á��޸ĺͷַ�����Ŀ��������ҵ��;��
* ���豣��ԭʼ��Ȩ�����������
* 
* �������μ���Ŀ��Ŀ¼�µ� LICENSE �ļ���
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
		std::map<i32_protocolnum, nlogicfun>	m_fun;				// key:Э��� value:������
		tnotfindfun								m_notfindfun;		// �����m_funû�в��ҵ�������,��ʹ���䴦��
	public:
		nrfunbase() = default;

		//# ����Э�鴦��û��ƥ��ĵ���
		nrfunbase& set_notfindfun(const tnotfindfun& afun);

		//# δƥ���Э�����
		void notfindfun(const actor_base* aactor, i32_threadid athreadid, handle_pram& apram)const;

		//# Э�鴦��
		bool handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram);
	};

	template <typename T>
	struct message
	{
	private:
		message() = delete;

		std::shared_ptr<T>		m_shared_data		= nullptr;	// ����
		T*						m_original_data		= nullptr;	// ����
		i32_threadid			m_thread			= 0;		// �߳�id
		std::shared_ptr<pack>	m_pack				= nullptr;	// �����Ϣ�������磬���ֵ��Ϊ�ռ�Ϊ�������ݰ�
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

		//# ��������std::function<void(TTTDerived*, T&)>���ص�ָ��actor��
		template <typename TTTDerived, typename T>
		nrfun& rfun(const std::function<void(TTTDerived*, message<T>&)>& afun, bool aisload = false);

		//# bool aisload = false 
		//# �Ƿ�����db���ݼ������֮ǰ�������Ϣ
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# actor����Ϣ����
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload = false);

		//# actor����Ϣ����,��ע�������
		template <typename TTTDerived, typename T>
		nrfun& rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload = false);

		//# gatewayע��c2g����ת��Э�鴦��Э��
		template <typename T>
		nrfun& rfun_c2g(const Tfun<TDerived, np_actor_forward<T, forward_c2g<forward>>> afun);

		//# gatewayע��g2c����ת��Э�鴦��Э��
		template <typename T>
		nrfun& rfun_g2c(const Tfun<TDerived, np_actor_forward<T, forward_g2c<forward>>> afun);		
	};
}//namespace ngl