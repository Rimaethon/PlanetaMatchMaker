#pragma once

#include <string>
#include <ostream>

namespace pgl {
	enum class server_error_code {
		ok,
		disconnected_expectedly,
		disconnected_by_client,
		acception_failed,
		message_send_timeout,
		message_reception_timeout,
		message_reception_error,
		invalid_message_type,
		message_type_mismatch,
		api_version_mismatch,
		message_send_error,
		permission_error,
		room_group_index_out_of_range,
		room_does_not_exist,
		room_player_is_full,
		room_permission_error,
		invalid_session,
	};

	std::string get_server_error_message(server_error_code error_code);
	std::ostream& operator <<(std::ostream& os, const server_error_code& error_code);
}
