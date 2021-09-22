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
    _MessageLogger(_MessageLogger&& td);
    /** Move assignment operator. */
    _MessageLogger& operator=(_MessageLogger&& td);

    /** Destructor. */
    ~_MessageLogger();

    /** Logs messages. */
    void log(std::string msg, MessageLabel tp);

    /** Logs information messages. */
    inline void info(std::string msg) { log(msg, InfoMessage); }

    /** Logs warning messages. */
    inline void warn(std::string msg) { log(msg, WarningMessage); }

    /** Logs error messages. */
    inline void error(std::string msg) { log(msg, ErrorMessage); }

    /** Logs the raw string to the (potentially various) output(s).
     * Externally accessible but also used internally. */
    void log_raw(std::string msg);

    /** Configures the use of std::cout. */
    void config_cout(bool use);

    /** Configures the use of a text file.
     * Pass `use=false` and `file=""` (i.e. an empty string) to remove all log
     * files from use. */
    void config_textFile(bool use, std::string file);

private:
    /** Clock type to use for timestamping messages. */
    typedef std::chrono::system_clock Clock_t;

    /** Message data to be added to the message queue. */
    typedef std::tuple<std::chrono::time_point<Clock_t>, MessageLabel, std::string> message_t;

    /** Converts a time point to an ISO timestamp. */
    std::string tpToISO(std::chrono::time_point<Clock_t> tp);

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

    /** Default log file name. */
    const std::string defLogFileName = "log.txt";

    /** Mutex for the log file names. */
    std::mutex lfnMutex;

    /** File stream used for opening log files. */
    std::fstream logFileStr;

    /** Config information. */
    struct {
        /** Whether to include milliseconds in message timestamps. */
        std::atomic_bool ts_show_ms;
        /** Whether to print messages to std::cout. */
        std::atomic_bool to_cout;
        /** Whether to print messages to the log text file. */
        std::atomic_bool to_file;
    } config;
};

extern _MessageLogger logger;

} // namespace logging

#endif
