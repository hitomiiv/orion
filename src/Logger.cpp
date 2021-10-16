// Logger.cpp
#include "Logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace ori
{

Logger Logger::instance = Logger();

auto Logger::get() -> spdlog::logger&
{
	return *instance.logger;
}

Logger::Logger()
{
	// console output should be info
	logger = spdlog::stderr_color_st("orion");
	logger->set_pattern("[%H:%M:%S] [Orion] %^[%l] %v%$");
	logger->enable_backtrace(32);

	auto console_file = std::make_shared<spdlog::sinks::basic_file_sink_st>("logs/engine.log", true);
	logger->sinks().push_back(console_file);
}

} // namespace ori
