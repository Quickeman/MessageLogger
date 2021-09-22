#include "MessageLogger.hpp"

#include "unit_test_framework.h"

#include <array>

using namespace logging;
using namespace std;

int main() {
    array<string, 2> logFileNames;
    fstream lf;
    int i;
    string iStr;
    for (i = 0; i < 2; i++) {
        iStr = to_string(i + 1);
        logFileNames[i] = "testlog" + iStr + ".txt";

        // Clear the log file to avoid confusion
        lf.open(logFileNames[i], ios::out | ios::trunc);
        test::check(lf.good(), "File clearing stream not good.");
        lf.close();

        // Configure logger
        logger.config_cout(false);
        logger.config_textFile(true, logFileNames[i]);

        // Log some messages
        logger.info("This is some log file information.");
        logger.warn("This is a log file warning.");
        logger.error("This is a log file error message.");

        // Wait so the messages have time to get printed
        this_thread::sleep_for(chrono::milliseconds(20));
        
        // Make sure to remove this log file once it's no longer needed
        logger.config_textFile(false, logFileNames[i]);
    }

    // Test the contents of the two log files
    for (int i = 0; i < 2; i++) {
        iStr = to_string(i + 1);
        string fileContents;
        fileContents.resize(1000);

        lf.open(logFileNames[i], ios::in);
        test::check(lf.good(), "Retrieving file contents, fstream not good.");
        lf.read(const_cast<char*>(fileContents.data()), 1000);
        test::check(!fileContents.empty(), "Log file " + iStr + " empty.");
        lf.close();
        
        int nlCount = 0;
        for (auto& c : fileContents)
            if (c == '\n')
                nlCount++;
        test::check(nlCount, 3, 0, 
            "Log file " + iStr + " does not contain the right number of new line characters.");
    }
}