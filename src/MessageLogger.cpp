#include "MessageLogger.hpp"

#include <iostream>

using namespace logging;

_MessageLogger::_MessageLogger():
msgTypeStr{"[INFO]", "[WARNING]", "[ERROR]"},
config{
    false, // show date
    true, // show ms
    true, // to std::cout
    false, // to file
    std::string{} // log file name
} {}

void _MessageLogger::log(const std::string& msg, MessageLabel ml) const {
    this->log_internal(message_t{clk.now(), ml, msg});
}

void _MessageLogger::log_raw(const std::string& msg) const {
    if (config.to_cout)
        std::cout << msg;

    if (config.to_file) {
        std::fstream fs(config.log_file, std::ios::app);
        fs << msg;
    }
}

void _MessageLogger::config_cout(bool use) {
    config.to_cout = use;
}

void _MessageLogger::config_textFile(bool use) {
    config.to_file = use;

    if (config.to_file && config.log_file.empty())
        config.log_file = logging::defaultLogFileName;
}

void _MessageLogger::config_textFile(const std::string& name) {
    config.to_file = true;
    config.log_file = name;
}

void _MessageLogger::config_timestamp(bool d, bool ms) {
    config.ts_show_date = d;
    config.ts_show_ms = ms;
}

void _MessageLogger::log_internal(message_t msg) const {
    try
    {
        std::string out {tpToISO(std::get<std::chrono::time_point<Clock_t>>(msg))};
        out.push_back(' ');
        out.append(msgTypeStr[std::get<MessageLabel>(msg)]);
        out.push_back(' ');
        out.append(std::get<std::string>(msg));
        if (out.back() != '\n')
            out.push_back('\n');

        log_raw(out);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Failed to log message with error: " << e.what() << '\n';
    }


}

std::string _MessageLogger::tpToISO(std::chrono::time_point<Clock_t> tp) const {
    const auto tt { clk.to_time_t(tp) };

    std::string out {""};
    std::string frmt {""};
    if (config.ts_show_date) {
        out.append("YYYY-MM-DD ");
        frmt.append("%Y-%m-%d ");
    }
    out.append("HH:MM:SS");
    frmt.append("%H:%M:%S");

    strftime(const_cast<char*>(out.data()), out.size(), frmt.c_str(), localtime(&tt));

    if (config.ts_show_ms) {
        auto ms { std::chrono::duration_cast<std::chrono::milliseconds>(tp.time_since_epoch()) };
        ms -= std::chrono::duration_cast<std::chrono::seconds>(tp.time_since_epoch());
        out.push_back('.');
        if (ms.count() < 100)
            out.push_back('0');
        if (ms.count() < 10)
            out.push_back('0');
        out.append(std::to_string(ms.count()));
    }

    return out;
}


// Define externally-linked logger object
_MessageLogger logging::logger;

// Define externally linked default log file name
const std::string logging::defaultLogFileName {"log.txt"};
