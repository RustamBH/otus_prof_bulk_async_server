#pragma once

#include "LogerAsync.h"


// A command handler class that processes command blocks and logs them asynchronously.
class CmdHandler
{
public:
	// Handles a command block by processing it and logging it asynchronously
	void Handle(const BlockCmd& block) noexcept;
private:
	LogerAsync m_LogerAsync{}; // The asynchronous logger
};
