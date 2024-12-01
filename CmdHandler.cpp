#include "CmdHandler.h"
#include <chrono>


void CmdHandler::Handle(const BlockCmd& block) noexcept
{
	if (!block.IsValid())
	{
		return;
	}

    auto get_current_timestamp = []() noexcept
    {
        return std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
    };

    VectorStr commands;
    int nested_blocks = 0;
    std::string timestamp;

    for (const auto& line : block.Commands)
    {
        if (line == "{")
        {
            ++nested_blocks;
            if (nested_blocks == 1)
            {
                m_LogerAsync.Log({ timestamp, commands });
                commands.clear();
            }
        }
        else if (line == "}")
        {
            --nested_blocks;
            if (nested_blocks == 0)
            {
                m_LogerAsync.Log({ timestamp, commands });
                commands.clear();
            }
        }
        else if (line == "EOF")
        {
            break;
        }
        else
        {
            if (nested_blocks == 0)
            {
                if (commands.empty())
                {
                    timestamp = get_current_timestamp();
                }

                commands.emplace_back(line);
                if (commands.size() == block.bulk)
                {
                    m_LogerAsync.Log({ timestamp, commands });
                    commands.clear();
                }
            }
            else
            {
                commands.emplace_back(line);
            }
        }
    }

    if (!commands.empty() && nested_blocks == 0)
    {
        m_LogerAsync.Log({ timestamp, commands });
        commands.clear();
    }
}
