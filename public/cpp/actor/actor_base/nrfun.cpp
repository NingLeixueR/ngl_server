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
#include "time_consuming.h"
#include "sysconfig.h"
#include "nrfun.h"

namespace ngl
{
	nrfunbase& nrfunbase::set_notfindfun(const tnotfindfun& afun)
	{
		m_notfindfun = afun;
		return *this;
	}

	void nrfunbase::notfindfun(const actor_base*, i32_threadid athreadid, handle_pram& apram)const
	{
		if (m_notfindfun != nullptr)
		{
			m_notfindfun(athreadid, apram);
		}
	}

	bool nrfunbase::handle_switch(actor_base* aactor, i32_threadid athreadid, handle_pram& apram)
	{
		auto itor = m_fun.find(apram.m_enum);
		if (itor == m_fun.end())
		{
			if (aactor->type() != ACTOR_CLIENT && aactor->type() != ACTOR_SERVER)
			{
				tprotocol::info* lpinfo = tprotocol::get(apram.m_enum);
				if (lpinfo == nullptr)
				{
					log_error()->print(
						"{}::handle_switch  m_fun.find({}) == end", aactor->guid(), apram.m_enum
					);
				}
				else
				{
					log_error()->print(
						"{}::handle_switch  m_fun.find({}:{}) == end", aactor->guid(), apram.m_enum, lpinfo->m_name
					);					
				}				
			}
			return false;
		}
		bool lisloadfinish = aactor->isloadfinish();
		if (lisloadfinish == false && itor->second.m_isdbload == true)
		{
			log_error()->print(
				"{}::handle_switch isloadfinish() == {}", aactor->guid(), lisloadfinish
			);
			return false;
		}
		time_consuming lconsuming(std::format("{}-{}-{}", aactor->guid(), apram.m_enum, tprotocol::protocol_name(apram.m_enum)));
		lconsuming.consuming_start();
		itor->second.m_fun(aactor, athreadid, apram);
		if (aactor->type() != ACTOR_LOG)
		{
			lconsuming.consuming_finish();
		}
		return true;
	}
}//namespace ngl