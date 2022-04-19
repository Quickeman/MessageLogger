#ifndef MESSAGE_LOGGER_H
#define MESSAGE_LOGGER_H

#include <string>
#include <tuple>
#include <chrono>
#include <fstream>
#include <array>

namespace logging {

extern const std::string defaultLogFileName;

/** Message logging class for tracking info and error messages. */
class _MessageLogger {
public:
    enum MessageLabel {
        InfoMessage,
        WarningMessage,
        ErrorMessage,
        _NumMessageTypes
    };

    /** Constructor.
     * Sets up the logging system. */
    _MessageLogger();

    /** Disable copy constructor. */
    _MessageLogger(const _MessageLogger&) = delete;
    /** Disable move constructor. */
    _MessageLogger(_MessageLogger&& ml) = delete;
    /** Disable copy assignment operator. */
    _MessageLogger& operator=(const _MessageLogger&) = delete;
    /** Disable move assignment operator. */
    _MessageLogger& operator=(_MessageLogger&& ml) = delete;

    /** Default destructor. */
    ~_MessageLogger() = default;

    /** Logs messages. */
    void log(const std::string& msg, MessageLabel ml) const;

    /** Logs information messages. */
    inline void info(const std::string& msg) const {
        log(msg, InfoMessage);
    }

    /** Logs warning messages. */
    inline void warn(const std::string& msg) const {
        log(msg, WarningMessage);
    }

    /** Logs error messages. */
    inline void error(const std::string& msg) const {
        log(msg, ErrorMessage);
    }

    /** Logs the raw string to the (potentially various) output(s).
     * Externally accessible but also used internally. */
    void log_raw(const std::string& msg) const;

    /** Configures the use of std::cout. */
    void config_cout(bool use);

    /** Configures the use of a text file.
     * The name of the file can be configured with @ref config_textfile(std::string) */
    void config_textFile(bool use);

    /** Renames the text file used for logging messages to.
     * @note Calling this method enables text-file logging regardless of previous
     * calls to @ref config_textfile(bool) */
    void config_textFile(const std::string& name);

    /** Configures the timestamp appearance in logged messages.
     * @param show_date whether to show the date.
     * @param show_ms whther to include milliseconds in the timestamp. */
    void config_timestamp(bool show_date, bool show_ms);

private:
    /** Clock type to use for timestamping messages. */
    typedef std::chrono::system_clock Clock_t;

    /** Message data to be added to the message queue. */
    typedef std::tuple<std::chrono::time_point<Clock_t>, MessageLabel, std::string> message_t;

    void log_internal(message_t msg) const;

    /** Converts a time point to an ISO timestamp. */
    std::string tpToISO(std::chrono::time_point<Clock_t> tp) const;

    /** Clock used for timestamping. */
    Clock_t clk;

    const std::array<std::string, _NumMessageTypes> msgTypeStr;

    /** Config information. */
    struct {
        /** Whether to include the date in message timestamps. */
        bool ts_show_date;
        /** Whether to include milliseconds in message timestamps. */
        bool ts_show_ms;
        /** Whether to print messages to std::cout. */
        bool to_cout;
        /** Whether to print messages to a log file. */
        bool to_file;
        /** Log file name to print messages to. */
        std::string log_file;
    } config;
};

extern _MessageLogger logger;

} // namespace logging

#endif
