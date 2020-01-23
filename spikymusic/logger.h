#pragma once
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <stdarg.h>
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
#include <string>
/*
boost libs uses auto linking therefore there no need to specify specific libarry to link*/


class Logger
{
public:
	typedef enum ErrorLevel { LEVEL_INFO, LEVEL_WARNING, LEVEL_ERROR, LEVEL_FATAL } error_level;

	Logger();
	void init();
	void log(error_level level, const char* format, ...);
	~Logger();
	
private:
	boost::log::sources::severity_logger< boost::log::trivial::severity_level > lg;
};
extern Logger logger;


