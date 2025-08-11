#include "serialize_lua.h"
#include "ndefine.h"

namespace ngl
{
	// # ≤‚ ‘

	struct k0
	{
		int m_v1;
		double m_v2;
		std::string m_v3;

		dprotocoljson(k0, m_v1, m_v2, m_v3)
	};

	struct k1
	{
		int m_v1;
		double m_v2;
		std::string m_v3;
		std::vector<int32_t> m_v4;
		std::map<int32_t, std::string> m_v5;
		std::map<std::string, std::string> m_v6;
		std::map<std::string, k0> m_v7;

		dprotocoljson(k1, m_v1, m_v2, m_v3, m_v4, m_v5, m_v6, m_v7)
	};

	struct k2
	{
		int m_v1;
		k1 m_v2;

		dprotocoljson(k2, m_v1, m_v2)
	};
}//namespace ngl