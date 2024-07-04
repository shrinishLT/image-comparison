#ifndef LOGGER_H
#define LOGGER_H

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>
#include <string>

class Logger {
public:
    Logger(const std::string& className) : className_(className) {
        if (spdlog::get(className) != nullptr) {
            logger_ = spdlog::get(className);
        } else {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            logger_ = std::make_shared<spdlog::logger>(className, console_sink);
            spdlog::register_logger(logger_);
            logger_->set_level(spdlog::level::info);
            logger_->set_pattern("[%l] [%n::%s] %v");
        }
    }

    void logInfo(const std::string& func, const std::string& message) {
        logger_->info("[{}::{}] {}", className_, func, message);
    }

    void logError(const std::string& func, const std::string& message) {
        logger_->error("[{}::{}] {}", className_, func, message);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
    std::string className_;
};

#endif // LOGGER_H
