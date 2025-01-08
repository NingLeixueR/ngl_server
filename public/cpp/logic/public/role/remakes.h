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
	//		add_remakes(lremakes, "test_2");
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
		remakes() {}
		const char* get_remakes();
	};

	class actor_role;

	class dremakes
	{
		dremakes(const dremakes&) = delete;
		dremakes& operator=(const dremakes&) = delete;

		actor_role* m_role;
	public:
		dremakes(actor_role* arole, const char* aremakes);
		dremakes(actor_role* arole, const std::string& aremakes);
		virtual ~dremakes();

		static const char* get_remake(actor_role*);
	};
}// namespace ngl

#define local_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define local_get_remakes(ROLE)			ngl::dremakes::get_remake(ROLE)