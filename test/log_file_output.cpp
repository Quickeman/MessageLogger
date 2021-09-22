#include "MessageLogger.hpp"

#include "unit_test_framework.h"

using namespace logging;
using namespace std;

int main() {
    const string logFileName = "testlog1.txt";

    // Clear the log file to avoid confusion
    fstream lf;
    lf.open(logFileName, ios::out | ios::trunc);
    test::check(lf.good(), "File clearing stream not good.");
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

    string fileContents;
    fileContents.resize(200);
    lf.open(logFileName, ios::in);
    test::check(lf.good(), "Retrieving file contents, fstream not good.");
    lf.read(const_cast<char*>(fileContents.data()), 200);
    test::check(!fileContents.empty(), "Log file empty.");
    lf.close();
    int nlCount = 0;
    for (auto& c : fileContents)
        if (c == '\n')
            nlCount++;
    test::check(nlCount, 3, 0, "Log file does not contain the right number of new line characters.");
}