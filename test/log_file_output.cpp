#include "MessageLogger.hpp"

#include "unit_test_framework.h"

#include <array>
#include <thread>

using namespace logging;

int main() {
    constexpr int numFiles = 3;
    std::array<std::string, numFiles> logFileNames;
    int i;
    std::string iStr;

    logger.config_textFile(true);
    std::fstream lf(defaultLogFileName, std::ios::out | std::ios::trunc);
    test::check(lf.good(), "File clearing stream not good.");
    lf.close();

    for (i = 0; i < numFiles; i++) {
        iStr = std::to_string(i + 1);
        logFileNames[i] = "testlog" + iStr + ".txt";

        // Clear the log file to avoid confusion
        std::fstream fs(logFileNames[i], std::ios::out | std::ios::trunc);
        test::check(fs.good(), "File clearing stream not good.");
        fs.close();

        // Configure logger
        logger.config_cout(false);
        logger.config_textFile(logFileNames[i]);

        // Log some messages
        logger.info("This is some log file information.");
        logger.warn("This is a log file warning.");
        logger.error("This is a log file error message.");

        // Wait so the messages have time to get printed
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // Make sure to remove this log file once it's no longer needed
        logger.config_textFile(false);
    }

    // Test the contents of the two log files
    for (int i = 0; i < numFiles; i++) {
        iStr = std::to_string(i + 1);
        std::string fileContents;
        fileContents.resize(200 * numFiles);

        std::fstream fs(logFileNames[i], std::ios::in);
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