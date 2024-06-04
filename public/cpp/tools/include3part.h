#pragma once

#include <strstream>
#include <iostream>
#include <string>
#include <ctime>

#include <asio.hpp>
#include <mysql.h>

#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32