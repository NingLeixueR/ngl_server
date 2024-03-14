#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>
#include <iostream>
#include <string>

#include "uuid.h"

namespace ngl
{
	std::string& uuid::make(std::string& strUUID)
	{
		boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
		strUUID = boost::uuids::to_string(a_uuid);
		return strUUID;
	}
}// namespace ngl