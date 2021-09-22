#include "MessageLogger.hpp"

#include "unit_test_framework.h"

using namespace logging;
using namespace std;

int main() {
    const string logFileName = "testlog1.txt";
    
    // Clear the log file to avoid confusion
    fstream lf;
    lf.open(logFileName, ios::out | ios::trunc);
    lf.close();

    // Configure logger
    logger.config_cout(false);
    logger.config_textFile(true, logFileName);

    // Log some messages
    logger.info("This is some log file information.");
    logger.warn("This is a log file warning.");
    logger.error("This is a log file error message.");

    // Wait so the messages have time to get printed
    this_thread::sleep_for(chrono::milliseconds(20));
}