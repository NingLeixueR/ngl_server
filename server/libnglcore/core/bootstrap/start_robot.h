// File overview: Declares the bootstrap entry point for robot-node startup.
#pragma once

#include "start_node.h"

startup_error start_robot(int aargc, char** aargv, int* atcp_port);
