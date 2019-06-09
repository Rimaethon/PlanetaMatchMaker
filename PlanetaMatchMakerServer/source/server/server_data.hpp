#pragma once

#include "data/thread_safe_data_container.h"
#include "room/room_data_container.hpp"
#include "client/client_data.hpp"

namespace pgl {
	struct server_data final {
		room_data_container room_data_container{};
		thread_safe_data_container<client_address, client_data> client_data_container{};
	};
}
