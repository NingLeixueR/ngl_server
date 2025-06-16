#include "actor_role.h"
#include "rolekv.h"

namespace ngl
{
	bool rolekv::value(const char* akey, int8_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint8_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, int32_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint32_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, int64_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, uint64_t& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, float& adata)
	{
		return number_value(akey, adata);
	}

	bool rolekv::value(const char* akey, double& adata)
	{
		return number_value(akey, adata);
	}
}//namespace ngl