#include "MessageLogger.hpp"

#include "unit_test_framework.h"

using namespace std;
using namespace logging;

int main() {
    logger.info("This is some information.");
    logger.warn("This is a warning.");
    logger.error("This is an error message.");
    this_thread::sleep_for(chrono::milliseconds(20));
}