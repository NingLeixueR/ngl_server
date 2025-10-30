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
#include <memory>

namespace ngl
{
	template <typename T>
	class impl
	{
		impl(const impl&) = delete;
		impl& operator=(const impl&) = delete;

		std::unique_ptr<T> m_impl;
	public:
		inline impl(){}

		~impl() {}

		template <typename ...ARG>
		inline void make_unique(const ARG&... args)
		{
			m_impl = std::make_unique<T>(args...);
		}

		inline std::unique_ptr<T>& operator()()
		{
			return m_impl;
		}
	};
}// namespace ngl