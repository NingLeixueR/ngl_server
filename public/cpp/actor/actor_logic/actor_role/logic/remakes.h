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

#include <thread>
#include <string>
#include <stack>

namespace ngl
{
	// # remakes
	//  在代码段中注入"remakes"字符串
	//  例如：
	//  local_remakes(role, "test_1");
	//  std::string lremakes = local_get_remakes(role); //lremakes == "test_1"
	// {
	//		lremakes = local_get_remakes(role); //lremakes == "test_1"
	//		local_remakes(lremakes, "test_2");
	//		role->addite();
	//		lremakes = local_get_remakes(role); //lremakes == "test_2"
	// }
	//  lremakes = local_get_remakes(role); //lremakes == "test_1"
	class dremakes;
	class remakes
	{
		remakes(const remakes&) = delete;
		remakes& operator=(const remakes&) = delete;

		friend class dremakes;
	private:
		std::stack<std::string> m_remakes;
		void add_remakes(const char* aremakes);
		void erase_remakes();
	public:
		remakes() = default;
		const char* get_remakes();
	};

	class actor_role;

	class dremakes
	{
		dremakes(const dremakes&) = delete;
		dremakes& operator=(const dremakes&) = delete;

		actor_role* m_role = nullptr;
	public:
		explicit dremakes(actor_role* arole, const char* aremakes);
		explicit dremakes(actor_role* arole, const std::string& aremakes);
		virtual ~dremakes();

		static const char* get_remake(actor_role*);
	};
}// namespace ngl

#define local_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define local_get_remakes(ROLE)			ngl::dremakes::get_remake(ROLE)