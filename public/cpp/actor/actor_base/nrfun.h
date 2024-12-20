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
		std::map<i32_protocolnum, nlogicfun>	m_fun;
		tnotfindfun								m_notfindfun;
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
		std::shared_ptr<T>	m_shared_data;
		T*					m_original_data;
	public:
		i32_threadid		m_thread;			// �߳�id
		const pack*			m_pack;				// �����Ϣ�������磬���ֵ��Ϊ�ռ�Ϊ�������ݰ�

		message() = delete;

		message(i32_threadid athread, const pack* apack, std::shared_ptr<T>& adata) :
			m_thread(athread),
			m_pack(apack),
			m_shared_data(adata),
			m_original_data(nullptr)
		{
		}

		message(i32_threadid athread, const pack* apack, T* adata) :
			m_thread(athread),
			m_pack(apack),
			m_original_data(adata),
			m_shared_data(nullptr)
		{
		}

		const T* get_data()const
		{
			if (m_shared_data != nullptr)
			{
				return m_shared_data.get();
			}
			return m_original_data;
		}

		const std::shared_ptr<T>& get_shared_data()const
		{
			return m_shared_data;
		}
	};

	template <typename TDerived, typename TPRAM>
	using Tfun = bool (TDerived::*)(const message<TPRAM>&);

	template <typename TDerived, EPROTOCOL_TYPE TYPE>
	class nrfun : public nrfunbase
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

		//# ��������std::function<void(TTTDerived*, T&)>���ص�ָ��actor��
		template <typename TTTDerived, typename T>
		nrfun& rfun(const std::function<void(TTTDerived*, message<T>&)>& afun);

		//# bool aisload = false 
		//# �Ƿ�����db���ݼ������֮ǰ�������Ϣ
		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, bool aisload = false);

		template <typename TTTDerived, typename T>
		nrfun& rfun(const Tfun<TTTDerived, T> afun, ENUM_ACTOR atype, bool aisload = false);

		template <typename TTTDerived, typename T>
		nrfun& rfun_nonet(const Tfun<TTTDerived, T> afun, bool aisload = false);

		template <bool BOOL, typename T>
		nrfun& rfun_forward(const Tfun<TDerived, np_actor_forward<T, TYPE, BOOL, ngl::forward>> afun, ENUM_ACTOR atype, bool aisload = false);

		template <typename T>
		nrfun& rfun_recvforward(const Tfun<TDerived, T> afun, bool aisload = false);
	};
}//namespace ngl