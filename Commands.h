#pragma once

#include <string>
#include <vector>

// alias for a vector of strings
using VectorStr = std::vector<std::string>;

// An interface for a command
struct InterfaceCmd
{
public:	
	// The list of commands
	VectorStr Commands{};
	InterfaceCmd() = default;
	
	explicit InterfaceCmd(const VectorStr& cmds) : Commands{cmds}{}	
	virtual ~InterfaceCmd() noexcept = default;
	
	// Checks if the command is valid
	virtual bool IsValid() const noexcept
	{
		return !Commands.empty();
	}
};


// A command block that contains a list of commands and a bulk value
struct BlockCmd : public InterfaceCmd
{
public:
	std::size_t bulk = 0;

	BlockCmd() : InterfaceCmd(), bulk{0}{}
	explicit BlockCmd(std::size_t _bulk) : InterfaceCmd(), bulk{_bulk} {}	
	virtual ~BlockCmd() noexcept override = default;
	
	// Checks if the command block is valid
	bool IsValid() const noexcept override
	{
		return InterfaceCmd::IsValid() && bulk > 0;
	}
};


// A command log that contains a timestamp and a list of commands
struct LogCmd : public InterfaceCmd
{
public:
	std::string Timestamp;
	LogCmd() = default;
	LogCmd(const std::string& timestamp, const VectorStr& cmds) : InterfaceCmd(cmds), Timestamp {timestamp} {}
	virtual ~LogCmd() noexcept override = default;

	// Check if the command log is valid	
	bool IsValid() const noexcept override
	{
		return InterfaceCmd::IsValid() && !Timestamp.empty();
	}

	// Clear the command log
	virtual void Reset() noexcept
	{
		Timestamp.clear();
		Commands.clear();
	}
};
