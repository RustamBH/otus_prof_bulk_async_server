#pragma once

#include "CmdHandler.h"
#include <map>
#include <mutex>


// class that handles the receiving of commands
class SingletonGetCmd
{
public:
	std::size_t Connect(std::size_t bulk) noexcept;
	void Receive(std::size_t handle, const char* data, std::size_t size) noexcept;
	void Disconnect(std::size_t handle) noexcept;
	//SingletonGetCmd& GetInstance() noexcept;

private:	
	std::map<std::size_t, BlockCmd> m_cmd_blocks; // map of handles to command blocks
	CmdHandler m_cmd_handler{}; // command handler	
	std::size_t m_handle = 0; // current handle
	std::mutex m_mutex; // mutex for synchronization
};
