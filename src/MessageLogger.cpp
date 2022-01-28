#include "MessageLogger.hpp"

#include <array>
#include <iostream>
#include <thread>

using namespace logging;
using namespace std;
using namespace std::chrono;

_MessageLogger::_MessageLogger() {
    // Configuration defaults
    config.ts_show_date = false;
    config.ts_show_ms = true;
    config_cout(true);
    // Don't log to any files by default
    config.log_files.clear();
}

void _MessageLogger::log(const string& msg, MessageLabel ml) {
    std::thread t([this, msg, ml](){
        this->log_internal(std::make_tuple(clk.now(), ml, msg));
    });
    t.detach();
}

void _MessageLogger::log_raw(const string& msg) const {
    if (config.to_cout) {
        std::cout << msg;
    }
    lock_guard<mutex> lg(lfnMutex);
    for (const auto& fn : config.log_files) {
        std::fstream fs(fn, ios::app);
        fs << msg;
    }
}

void _MessageLogger::config_cout(bool use) {
    config.to_cout.store(use);
}

void _MessageLogger::config_textFile(bool use, const string& file) {
    lock_guard<mutex> lg(lfnMutex);
    config.log_files.remove(file);
    if (use)
        config.log_files.push_front(file);
    else if (file == "")
        config.log_files.clear();
}

void _MessageLogger::config_timestamp(bool d, bool ms) {
    config.ts_show_date = d;
    config.ts_show_ms = ms;
}

void _MessageLogger::log_internal(message_t msg) {
    const std::array<std::string, _NumMessageTypes> typeStr {
        "[INFO]",
        "[WARNING]",
        "[ERROR]"
    };

    try
    {
        std::string out {tpToISO(std::get<time_point<Clock_t>>(msg))};
        out.push_back(' ');
        out.append(typeStr[std::get<MessageLabel>(msg)]);
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

string _MessageLogger::tpToISO(time_point<Clock_t> tp) const {
    const auto tt { clk.to_time_t(tp) };
    
    string out {""};
    string frmt {""};
    if (config.ts_show_date) {
        out.append("YYYY-MM-DD ");
        frmt.append("%Y-%m-%d ");
    }
    out.append("HH:MM:SS");
    frmt.append("%H:%M:%S");

    strftime(const_cast<char*>(out.data()), out.size(), frmt.c_str(), localtime(&tt));

    if (config.ts_show_ms) {
        auto ms { duration_cast<milliseconds>(tp.time_since_epoch()) };
        ms -= duration_cast<seconds>(tp.time_since_epoch());
        out.push_back('.');
        if (ms.count() < 100)
            out.push_back('0');
        if (ms.count() < 10)
            out.push_back('0');
        out.append(to_string(ms.count()));
    }

    return out;
}


// Define externally-linked logger object
_MessageLogger logging::logger;

// Define externally linked default log file name
const string logging::defaultLogFileName { "log.txt" };
