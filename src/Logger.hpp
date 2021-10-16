// Logger.hpp
#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <memory>

#include <spdlog/spdlog.h>

namespace ori
{

class Logger
{
public:
	static auto get() -> spdlog::logger&;

private:
	Logger();
	static Logger instance;

	std::shared_ptr<spdlog::logger> logger;
};

} // namespace ori

#endif // LOGGER_HPP_
