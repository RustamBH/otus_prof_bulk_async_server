#pragma once

#include "Commands.h"
#include <atomic>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>


// A shared command class that extends the LogCmd class and adds atomic flags for console and file completion
struct SharedCmd : public LogCmd
{
public:
	// Atomic flag indicating if console output is finished
	std::atomic_bool IsConsoleDone{ false };

	// Atomic flag indicating if file output is finished
	std::atomic_bool IsFileDone{ false };
	
	virtual ~SharedCmd() noexcept override = default;

	// Checks if the command is complete. True if both console and file flags are set, false otherwise	
	bool IsBothFinished() const noexcept
	{
		return IsConsoleDone && IsFileDone;
	}

	virtual void Reset() noexcept override
	{
		if (IsBothFinished())
		{
			LogCmd::Reset();
		}
	}

	// Assignment operator
	SharedCmd& operator=(const LogCmd& other) noexcept
	{
		if (this != &other)
		{
			Timestamp = other.Timestamp;
			Commands  = other.Commands;

			IsConsoleDone = false;
			IsFileDone = false;
		}
		return *this;
	}
};


// An asynchronous logger class that logs commands asynchronously to console and file
class LogerAsync final
{
public:
	LogerAsync() noexcept;
	~LogerAsync() noexcept;	
	void Log(const LogCmd& cmd) noexcept; // Logs a command asynchronously

private:
	void QueueWorkerThread(const std::stop_token& stop_token) noexcept;
	void ConsoleWorkerThread(const std::stop_token& stop_token) noexcept;
	void FileWorkerThread1(const std::stop_token& stop_token) noexcept;
	void FileWorkerThread2(const std::stop_token& stop_token) noexcept;
	void LogToFile(const std::string& timestamp, const VectorStr& cmds) noexcept;

private:
	std::vector<std::jthread> m_WorkerThreads; // Vector of worker threads
	std::stop_source m_StopToken; // Stop source for the worker threads
	std::atomic_int m_Counter = 0; // Atomic counter for the number of commands processed
	std::queue<LogCmd> m_Queue; // Queue of commands to process
	std::mutex m_MutexQueue; // Mutex for the queue
	std::condition_variable m_CVQueue; // Condition variable for the queue
	SharedCmd m_SharedCmd; // Shared command object
	std::mutex m_MutexWorker; // Mutex for the shared command object
	std::condition_variable m_CVWorker; // Condition variable for the shared command object
};
