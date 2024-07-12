#include "simulator.h"

#include <string>
#include <fstream>

#include "robotlogger.h"

void Simulator::readHouseFile(const std::string& house_file_path)
{
    initializeLogFile(house_file_path);

    std::ifstream house_file;
    house_file.open(house_file_path);

    if (!house_file.is_open())
    {
        throw std::runtime_error("Couldn't open input house file!");
    }

    max_simulator_steps = SimulatorDeserializer::deserializeMaxSteps(house_file);
    battery_controller = SimulatorDeserializer::deserializeBattery(house_file);
    location_manager = SimulatorDeserializer::deserializeHouse(house_file);
}

void Simulator::move(Step next_step)
{
    RobotLogger& logger = RobotLogger::getInstance();

    if (Step::FINISH == next_step)
    {
        /* Shouldn't happen */
        return;
    }

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (Step::STAY == next_step)
    {
        if (location_manager.isInDockingStation())
        {
            battery_controller.charge();
        }

        else
        {
            battery_controller.discharge();
            location_manager.cleanCurrentPosition();
        }
    }

    else
    {
        battery_controller.discharge();
    }
 
    location_manager.move(next_step);

    Position next_position = location_manager.getCurrentPosition();
    logger.logRobotStep(next_step, next_position);
}

void Simulator::setAlgorithm(const AbstractAlgorithm& algorithm)
{
    algorithm = std::make_unique<AbstractAlgorithm>(algorithm);

    algorithm.setMaxSteps(max_simulator_steps);
    algorithm.setWallsSensor(location_manager);
    algorithm.setDirtSensor(location_manager);
    alogrithm.setBatteryMeter(battery_controller);
}

void Simulator::run()
{
    RobotLogger& logger = RobotLogger::getInstance();
    unsigned int total_steps_performed = 0;
    bool is_algorithm_finished = false;

    while (!shouldStopCleaning(total_steps_performed))
    {
        Step next_step = navigation_system.nextStep();

        if (Step::FINISH == next_step)
        {
            // In case Robot mapped all accessible positions and have nothing left to clean
            is_algorithm_finished = true;
            break;
        }

        move(next_step);
        total_steps_performed++;
    }

    if (isMissionComplete() || is_algorithm_finished)
    {
        logger.logRobotFinish();
    }

    unsigned int total_dirt_count = location_manager.getTotalDirtCount();
    bool is_battery_exhausted = battery_controller.getBatteryState() < 1;
    bool is_mission_complete = (0 == total_dirt_count) && location_manager.isInDockingStation();

    logger.logCleaningStatistics(total_steps_performed,
                                 total_dirt_count,
                                 is_battery_exhausted,
                                 is_mission_complete);
}
