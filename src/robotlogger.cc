#include "robotlogger.h"

const RobotLogger::kRobotStepFormat = "[STEP] Robot took step to {} - New Position ({},{})";

const RobotLogger::kCleaningStatisticsFormati = "### Program Terminated ###\n
                                                 Total Steps Taken: {}\n
                                                 Total Dirt Left: {}\n
                                                 Is Vacuum Cleaner Dead: {}\n
                                                 Mission Succeeded: {}";