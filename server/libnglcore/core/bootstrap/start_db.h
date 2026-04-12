// File overview: Declares the bootstrap entry point for database-node startup.
#pragma once

#include "start_node.h"

startup_error start_db(int aargc, char** aargv, int* atcp_port);
