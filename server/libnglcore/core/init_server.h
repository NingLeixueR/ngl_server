// File overview: Declares interfaces for core.

#pragma once

#include "start_node.h"

startup_error init_server([[maybe_unused]] int aid, const std::set<pbnet::ENUM_KCP>& akcp, int* atcp_port);
