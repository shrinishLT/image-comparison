#ifndef LOGGER_H
#define LOGGER_H

#include <string>

/**
 * Logger Class
 * 
 * This Logger class handles two types of logging:
 * 
 * 1. JSON Responses to stdout:
 *    - These logs are intended to be captured by the code when the binary is executed.
 *    - The output is always in a standard JSON format with two fields: 
 *      - "status": either "success" or "error"
 *      - "result": a dynamic JSON object representing the result of the executed function.
 * 
 * 2. Internal Logs to stderr:
 *    - These logs are for internal debugging purposes.
 *    - The format includes severity, class name, function name, and message.
 *    - Severity levels: INFO and ERROR.
 *    - These logs do not interfere with the JSON output.
 * 
 * Example Usage:
 * 
 *     Logger logger("MyClass");
 *     
 *     // Simulating a function result
 *     nlohmann::json result;
 *     result["key"] = "value";
 *     
 *     // Log a JSON response
 *     logger.logJsonResponse("success", result.dump());
 *     
 *     // Log internal messages
 *     logger.logInternalInfo(__func__, "This is an info message");
 *     logger.logInternalError(__func__, "This is an error message");
 *     
 */
class Logger {
public:
    // Enum for internal log severity levels
    enum class Severity {
        INFO,
        ERROR
    };

    // Constructor accepting the class name
    Logger(const std::string& className);

    // Method to log JSON responses to stdout
    void logJsonResponse(const std::string& status, const std::string& result) const;

    // Method to log internal messages to stderr with severity
    void logInternal(Severity severity, const std::string& func, const std::string& message) const;

    // Convenience method to log internal info messages
    void logInternalInfo(const std::string& func, const std::string& message) const;

    // Convenience method to log internal error messages
    void logInternalError(const std::string& func, const std::string& message) const;

    // Method to handle errors by logging JSON response and exiting
    void handleError(const std::string& func, const std::string& message) const;

private:
    std::string className_;

    // Helper method to convert Severity enum to string
    std::string toString(Severity severity) const;
};

#endif // LOGGER_H
