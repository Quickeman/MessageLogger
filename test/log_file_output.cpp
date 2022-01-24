#include "MessageLogger.hpp"

#include "unit_test_framework.h"

#include <array>
#include <thread>

using namespace logging;
using namespace std;

int main() {
    constexpr int numFiles = 3;
    array<string, numFiles> logFileNames;
    int i;
    string iStr;

    logger.config_textFile(true, defaultLogFileName);
    fstream lf(defaultLogFileName, ios::out | ios::trunc);
    test::check(lf.good(), "File clearing stream not good.");
    lf.close();

    for (i = 0; i < numFiles; i++) {
        iStr = to_string(i + 1);
        logFileNames[i] = "testlog" + iStr + ".txt";

        // Clear the log file to avoid confusion
        fstream fs(logFileNames[i], ios::out | ios::trunc);
        test::check(fs.good(), "File clearing stream not good.");
        fs.close();

        // Configure logger
        logger.config_cout(false);
        logger.config_textFile(true, logFileNames[i]);

        // Log some messages
        logger.info("This is some log file information.");
        logger.warn("This is a log file warning.");
        logger.error("This is a log file error message.");

        // Wait so the messages have time to get printed
        this_thread::sleep_for(chrono::milliseconds(100));
        
        // Make sure to remove this log file once it's no longer needed
        if (i == (numFiles - 2))
            logger.config_textFile(false, "");
        else
            logger.config_textFile(false, logFileNames[i]);
    }

    // Test the contents of the two log files
    for (int i = 0; i < numFiles; i++) {
        iStr = to_string(i + 1);
        string fileContents;
        fileContents.resize(200 * numFiles);

        fstream fs(logFileNames[i], ios::in);
        test::check(fs.good(), "Retrieving file contents, fstream not good.");
        fs.read(const_cast<char*>(fileContents.data()), 200 * numFiles);
        test::check(!fileContents.empty(), "Log file " + iStr + " empty.");
        fs.close();

        int nlCount = 0;
        for (auto& c : fileContents)
            if (c == '\n')
                nlCount++;
        test::check(nlCount, 3, 0, 
            "Log file " + iStr + " does not contain the right number of new line characters.");
    }
}