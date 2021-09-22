#include "MessageLogger.hpp"

#include "unit_test_framework.h"

using namespace std;
using namespace logging;

int main() {
    // Configure logger
    logger.config_cout(true);
    logger.config_textFile(false, "testlog.txt");

    // Log some messages
    logger.info("This is some console information.");
    logger.warn("This is a console warning.");
    logger.error("This is a console error message.");

    // Wait so the messages have time to get printed
    this_thread::sleep_for(chrono::milliseconds(20));
}