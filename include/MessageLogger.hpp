#ifndef MESSAGE_LOGGER_H
#define MESSAGE_LOGGER_H

#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
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
    /** Disable copy assignment operator. */
    _MessageLogger& operator=(const _MessageLogger&) = delete;

    /** Move constructor. */
    _MessageLogger(_MessageLogger&& ml);
    /** Move assignment operator. */
    _MessageLogger& operator=(_MessageLogger&& ml);

    /** Destructor. */
    ~_MessageLogger();

    /** Logs messages. */
    void log(const std::string& msg, MessageLabel tp);

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

    /** Sets the logging period in milliseconds.
     * To prevent CPU choking, the messages given to the various methods are
     * only sent to output according to a kind of clock. This method sets the
     * period for the messages to be sent to output. */
    void set_period(unsigned int period);

private:
    /** Clock type to use for timestamping messages. */
    typedef std::chrono::system_clock Clock_t;

    /** Message data to be added to the message queue. */
    typedef std::tuple<std::chrono::time_point<Clock_t>, MessageLabel, std::string> message_t;

    /** Converts a time point to an ISO timestamp. */
    std::string tpToISO(std::chrono::time_point<Clock_t> tp) const;

    /** Method to use for thread execution. */
    void run();

    /** Thread for logging to keep overhead out of time-critical branches. */
    std::thread msgThread;

    /** Container for messages to log. */
    std::queue<std::tuple<std::chrono::time_point<Clock_t>, MessageLabel, std::string>> msgQueue;

    /** Mutex for the message queue to prevent data races. */
    std::mutex msgMutex;

    /** 'Running' flag for controlling thread execution. */
    std::atomic_bool running;

    /** Clock used for timestamping. */
    Clock_t clk;

    /** List of log files to print messages to. */
    std::forward_list<std::string> logFileNames;

    /** Mutex for the log file names. */
    mutable std::mutex lfnMutex;

    /** Config information. */
    struct {
        /** Whether to include milliseconds in message timestamps. */
        std::atomic_bool ts_show_ms;
        /** Whether to print messages to std::cout. */
        std::atomic_bool to_cout;
        /** Whether to print messages to the log text file. */
        std::atomic_bool to_file;
    } config;

    /** Time between messages in buffer being sent to output in milliseconds. */
    std::atomic_uint logPeriod;
};

extern _MessageLogger logger;

} // namespace logging

#endif
