#ifndef LOGGER_H
#define LOGGER_H

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/log/attributes/constant.hpp>
#include <string>

class Logger {
public:
    Logger(const std::string& className) {
        boost::log::add_console_log(
            std::clog,  // Using std::clog for better performance than std::cout
            boost::log::keywords::format = (
                boost::log::expressions::stream
                    << boost::log::trivial::severity << " :: "
                    << className << "::"
                    << boost::log::expressions::attr<boost::log::attributes::named_scope::value_type>("Scope") << " :: "
                    << boost::log::expressions::smessage
            )
        );
        boost::log::core::get()->add_global_attribute("Scope", boost::log::attributes::named_scope());
        boost::log::core::get()->add_global_attribute("ClassName", boost::log::attributes::constant<std::string>(className));
    }
};

#endif // LOGGER_H
