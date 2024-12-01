#pragma once

#include <cstddef>
#include "SingletonGetCmd.h"


// Returns a reference to the singleton instance of SingletonGetCmd
SingletonGetCmd& GetInstance() noexcept;

namespace async {
    using handle_t = std::size_t;

	handle_t connect(std::size_t bulk);
	void receive(handle_t handle, const char *data, std::size_t size);
	void disconnect(handle_t handle);
}
