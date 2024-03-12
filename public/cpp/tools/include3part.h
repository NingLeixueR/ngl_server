#pragma once

#include <boost/system/error_code.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/pool/pool.hpp>
#include <boost/date_time.hpp>
#include <boost/beast.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <strstream>
#include <iostream>
#include <string>
#include <ctime>

#include <mysql.h>

#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32