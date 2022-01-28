#ifndef MESSAGE_LOGGER_H
#define MESSAGE_LOGGER_H

#include <mutex>
#include <atomic>
#include <string>
#include <tuple>
#include <chrono>
#include <fstream>
#include <forward_list>

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
    void log(const std::string& msg, MessageLabel ml);

    /** Logs information messages. */
    inline void info(const std::string& msg) {
        log(msg, InfoMessage);
    }

    /** Logs warning messages. */
    inline void warn(const std::string& msg) {
        log(msg, WarningMessage);
    }

    /** Logs error messages. */
    inline void error(const std::string& msg) {
        log(msg, ErrorMessage);
    }

    /** Logs the raw string to the (potentially various) output(s).
     * Externally accessible but also used internally. */
    void log_raw(const std::string& msg) const;

    /** Configures the use of std::cout. */
    void config_cout(bool use);

    /** Configures the use of a text file.
     * Pass `use=false` and `file=""` (i.e. an empty string) to remove all log
     * files from use. */
    void config_textFile(bool use, const std::string& file);

    /** Configures the timestamp appearance in logged messages.
     * @param show_date whether to show the date.
     * @param show_ms whther to include milliseconds in the timestamp. */
    void config_timestamp(bool show_date, bool show_ms);

private:
    /** Clock type to use for timestamping messages. */
    typedef std::chrono::system_clock Clock_t;

    /** Message data to be added to the message queue. */
    typedef std::tuple<std::chrono::time_point<Clock_t>, MessageLabel, std::string> message_t;

    void log_internal(message_t msg);

    /** Converts a time point to an ISO timestamp. */
    std::string tpToISO(std::chrono::time_point<Clock_t> tp) const;

    /** Clock used for timestamping. */
    Clock_t clk;

    /** Config information. */
    struct {
        /** Whether to include the date in message timestamps. */
        std::atomic_bool ts_show_date;
        /** Whether to include milliseconds in message timestamps. */
        std::atomic_bool ts_show_ms;
        /** Whether to print messages to std::cout. */
        std::atomic_bool to_cout;
        /** List of log files to print messages to. */
        std::forward_list<std::string> log_files;
    } config;

    /** Mutex for the log file names. */
    mutable std::mutex lfnMutex;
};

extern _MessageLogger logger;

} // namespace logging

#endif
