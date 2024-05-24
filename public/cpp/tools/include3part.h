#pragma once

#include <asio.hpp>
#include <strstream>
#include <iostream>
#include <string>
#include <ctime>

#include <mysql.h>

#ifdef WIN32
#define snprintf _snprintf
#endif //WIN32