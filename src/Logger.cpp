#include "Logger.h"
#include <cstdio>
#include <iostream>
#include <nlohmann/json.hpp>
#include <cstdlib>  // For exit()

// Constructor accepting the class name
Logger::Logger(const std::string& className) : className_(className) {}

// Method to log JSON responses to stdout
void Logger::logJsonResponse(const std::string& status, const std::string& result) const {
    nlohmann::json response;
    response["status"] = status;
    response["result"] = nlohmann::json::parse(result);
    std::cout << response.dump() << std::endl;
}

// Method to log internal messages to stderr with severity
void Logger::logInternal(Severity severity, const std::string& func, const std::string& message) const {
    fprintf(stderr, "[%s] [%s::%s] %s\n", toString(severity).c_str(), className_.c_str(), func.c_str(), message.c_str());
}

// Convenience method to log internal info messages
void Logger::logInternalInfo(const std::string& func, const std::string& message) const {
    logInternal(Severity::INFO, func, message);
}

// Convenience method to log internal error messages
void Logger::logInternalError(const std::string& func, const std::string& message) const {
    logInternal(Severity::ERROR, func, message);
}

// Helper method to convert Severity enum to string
std::string Logger::toString(Severity severity) const {
    switch(severity) {
        case Severity::INFO: return "INFO";
        case Severity::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

// Method to handle errors by logging JSON response and exiting
void Logger::handleError(const std::string& func, const std::string& message) const {
    nlohmann::json errorResult;
    errorResult["error"] = message;
    logJsonResponse("error", errorResult.dump());
    logInternalError(func, message);
    exit(EXIT_FAILURE);
}
