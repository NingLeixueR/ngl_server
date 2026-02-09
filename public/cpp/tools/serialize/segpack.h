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
#include "segpack.h"
#include "nrate.h"
#include "type.h"
#include "pack.h"

#include <functional>
#include <map>

namespace ngl
{
	class segpack_heartbeat
	{
	public:
		static bool is_heartbeat(i32_protocolnum aprotocolnum);
	};

	class segpack
	{
		std::map<i32_socket, std::shared_ptr<pack>> m_data; //残包暂时保管
		bpool			m_pool;
		nrate			m_rate;
		segpack*		m_segpack = nullptr;

	public:
		enum class edopush
		{
			e_continue,		// 继续循环
			e_break,		// 跳出循环
			e_error,		// 出错
		};

		segpack() = default;

		bool push(i32_socket aid, const char* ap, int alen, bool aislanip);

		void close(i32_socket aid);

	private:
		void erase(i32_socket aid);

		std::shared_ptr<pack> remnant_package(i32_socket aid);

		edopush telnet_cmd(std::shared_ptr<pack>& apack, const char*& ap, int& alen, bool aislanip);

		bool check_recv(std::shared_ptr<pack>& apack, i32_socket aid, int alen, bool aislanip);

		bool is_heartbeat(std::shared_ptr<pack>& apack);

		bool fill_pack(std::shared_ptr<pack>& apack, const char*& ap, int& alen, int& len);

		edopush do_push(i32_socket aid, const char*& ap, int& alen, bool aislanip);
	};
}// namespace ngl