#include "logger.h"





void Logger::init()
{
	logging::add_file_log
	(
		keywords::file_name = "log_%N.log",                                        /*< file name pattern >*/
		keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
		keywords::format = "[%TimeStamp%]: %Message%",                                 /*< log record format >*/
		keywords::auto_flush = true, 												   //write the log enry immedietely
		keywords::open_mode = std::ios::out | std::ios::app
	);

	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::info
	);
	logging::add_common_attributes();
}


/*
we use an variable arguments list for this function
*/
void Logger::log(error_level level, const char* format, ...)
{
	using namespace logging::trivial;
	char buffer[256];
	//a place to store the list of arguments
	va_list args; 
	//initialize args to store all values after format
	va_start(args, format);
	vsprintf_s(buffer, format, args);
	switch (level)
	{
	case LEVEL_INFO:
		BOOST_LOG_SEV(lg, info) << "Info: " << buffer;
		break;
	case LEVEL_WARNING:
		BOOST_LOG_SEV(lg, warning) << "Warning: " << buffer;
		break;
	case LEVEL_ERROR:
		BOOST_LOG_SEV(lg, error) << "Error: " << buffer;
		break;
	case LEVEL_FATAL:
		BOOST_LOG_SEV(lg, fatal) << "Fatal: " << buffer;
		break;
	default:
		return;
	}

}

Logger::Logger()
{
}


Logger::~Logger()
{
}
Logger logger{};

