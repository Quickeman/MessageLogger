#include "MessageLogger.hpp"

#include "unit_test_framework.h"

using namespace std;
using namespace logging;

int main() {
    // Configure logger
    logger.config_cout(true);
    logger.config_textFile(false, "testlog.txt");

    test::check(cout.good(), "Before logging, cout not good.");

    // Log some messages
    logger.info("This is some console information.");
    logger.warn("This is a console warning.");
    logger.error("This is a console error message.");

    test::check(cout.good(), "After logging, pre-output, cout not good.");

    // Wait so the messages have time to get printed
    this_thread::sleep_for(chrono::milliseconds(20));

    test::check(cout.good(), "After output, cout not good.");
}