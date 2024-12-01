#include "SingletonGetCmd.h"

std::size_t SingletonGetCmd::Connect(std::size_t bulk) noexcept
{
    std::unique_lock lck(m_mutex);
    const auto [handle, _]{ m_cmd_blocks.emplace(std::make_pair(++m_handle, bulk)) };

    return handle->first;
}

void SingletonGetCmd::Receive(std::size_t handle, const char* data, std::size_t size) noexcept
{
	if (handle > 0 && data && size > 0)
	{
        std::unique_lock lck(m_mutex);
        if (m_cmd_blocks.contains(handle))
        {
            m_cmd_blocks[handle].Commands.emplace_back(data, size);
            m_cmd_handler.Handle(m_cmd_blocks[handle]);
        }
	}
}

void SingletonGetCmd::Disconnect(std::size_t handle) noexcept
{
    if (handle > 0)
    {
        std::unique_lock lck(m_mutex);
        if (m_cmd_blocks.contains(handle))
        {
            m_cmd_blocks.erase(handle);
        }
    }
}
