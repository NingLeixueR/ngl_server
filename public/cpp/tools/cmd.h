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

#include <functional>
#include <list>
#include <map>

namespace ngl
{
	///////////////////////////////////////////////////////////
	// �����ṩkey/std::function<void(ARG...)>�ṹ
	// ͨ����Ա����push�ṩkey��std::function<void(ARG...)>
	// ͨ����Ա����functionִ�ж�Ӧ�ķ���
	// TAG���,������,��Ҫ���ڲ�����ͬ����
	// TKEY���ڲ��ҵ�key����
	// ...ARG ����ָ��std::function<void(ARG...)>�Ĳ���
	template <typename TAG, typename TKEY, typename ...ARG>
	class cmd
	{
		cmd() = delete;
		cmd(const cmd&) = delete;
		cmd& operator=(const cmd&) = delete;

		static std::map<TKEY, std::function<void(ARG...)>> m_fun;
	public:
		static bool empty()
		{
			return m_fun.empty();
		}

		static bool function(const TKEY& akey, ARG... arg)
		{
			auto itor = m_fun.find(akey);
			if (itor == m_fun.end())
			{
				return false;
			}
			itor->second(arg...);
			return true;
		}

		static std::function<void(ARG...)>& add(const TKEY& akey)
		{
			return m_fun[akey];
		}
	};

	template <typename TAG, typename TKEY, typename ...ARG>
	std::map<TKEY, std::function<void(ARG...)>> cmd<TAG, TKEY, ARG...>::m_fun;
}//namespace ngl