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

#include "threadtools.h"
#include "handle_pram.h"
#include "actor_base.h"

namespace ngl
{
	class nthread
	{
		nthread() = delete;
		nthread(const nthread&) = delete;
		nthread& operator=(const nthread&) = delete;

		i32_threadid		m_id = 0;				// 线程id
		ptractor			m_actor = nullptr;		// 此时占用线程的actor
		bool				m_isactivity = false;	// 线程是否活跃
		ngl::thread			m_thread;				// 线程
		std::shared_mutex	m_mutex;				// 互斥量
		ngl::sem			m_sem;					// 信号量

	public:
		explicit nthread(i32_threadid aid);

		// # 返回线程id
		i32_threadid id();

		// # 是否活跃
		bool isactivity();

		// # 向线程中添加actor
		void push(ptractor aactor);

	private:
		void run();
	};
}//namespace ngl