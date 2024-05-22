#pragma once

#include <boost/system/error_code.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/date_time.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <strstream>
#include <iostream>
#include <string>
#include <ctime>

#include <mysql.h>

#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32