#include "MessageLogger.hpp"

#include <array>
#include <iostream>

using namespace logging;
using namespace std;
using namespace std::chrono;

_MessageLogger::_MessageLogger() {
    // Configuration defaults
    config.ts_show_ms = true;
    config_cout(true);
    config_textFile(false, defaultLogFileName);

    // Start message posting thread
    running = true;
    msgThread = thread([this](){ this->run(); });
}

_MessageLogger::_MessageLogger(_MessageLogger&& ml):
msgThread(move(ml.msgThread)) {
    running = false;
}

_MessageLogger& _MessageLogger::operator=(_MessageLogger&& ml) {
    running = false;
    if (msgThread.joinable())
        msgThread.join();
    msgThread = move(ml.msgThread);
    return *this;
}

_MessageLogger::~_MessageLogger() {
    running = false;
    if (msgThread.joinable())
        msgThread.join();
}

void _MessageLogger::log(string msg, MessageLabel tp) {
    lock_guard<mutex> lg(msgMutex);
    msgQueue.push(make_tuple(clk.now(), tp, msg));
}

void _MessageLogger::log_raw(string msg) {
    if (config.to_cout) {
        cout << msg;
    }
    if (config.to_file) {
        lock_guard<mutex> lg(lfnMutex);
        for (auto& fn : logFileNames) {
            logFileStr.open(fn, ios::app);
            logFileStr << msg;
            logFileStr.close();
        }
    }
}

void _MessageLogger::config_cout(bool use) {
    config.to_cout = use;
}

void _MessageLogger::config_textFile(bool use, string file) {
    lock_guard<mutex> lg(lfnMutex);
    logFileNames.remove(file);
    if (use)
        logFileNames.push_front(file);
    else if (file == "")
        logFileNames.clear();
    config.to_file = !logFileNames.empty();
}

string _MessageLogger::tpToISO(time_point<Clock_t> tp) {
    const auto tt = clk.to_time_t(tp);
    string out = "YYYY-MM-DD HH:MM:SS";
    strftime(const_cast<char*>(out.data()), out.size(), "%Y-%m-%d %H:%M:%S", localtime(&tt));
    if (config.ts_show_ms) {
        auto ms = duration_cast<milliseconds>(tp.time_since_epoch());
        ms -= duration_cast<seconds>(tp.time_since_epoch());
        out.push_back('.');
        if (ms.count() < 100)
            out.push_back('0');
        if (ms.count() < 10)
            out.push_back('0');
        if (ms.count() == 0)
            out.push_back('0');
        out.append(to_string(ms.count()));
    }
    return out;
}

void _MessageLogger::run() {
    const array<string, _NumMessageTypes> typeStr = {
        "[INFO]",
        "[WARNING]",
        "[ERROR]"
    };
    while (running) {
        if (!msgQueue.empty()) {
            // Format string as desired
            auto& msg = msgQueue.front();
            string out = tpToISO(get<time_point<Clock_t>>(msg));
            out.push_back(' ');
            out.append(typeStr[get<MessageLabel>(msg)]);
            out.push_back(' ');
            out.append(get<string>(msg));
            if (out.back() != '\n')
                out.push_back('\n');

            log_raw(out);

            lock_guard<mutex> lg(msgMutex);
            msgQueue.pop();
        }
    }
}


// Define externally-linked logger object
_MessageLogger logging::logger;

// Define externally linked default log file name
const string logging::defaultLogFileName = "log.txt";
