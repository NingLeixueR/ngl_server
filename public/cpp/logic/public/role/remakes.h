#pragma once

#include <thread>
#include <string>
#include <stack>

namespace ngl
{
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

#define d_remakes(ROLE, REMAKES)	ngl::dremakes ldremakes(ROLE, REMAKES)
#define g_remakes(ROLE)				ngl::dremakes::get_remake(ROLE)