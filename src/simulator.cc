#include "simulator.h"

#include <string>
#include <fstream>

#include "deserializer.h"
#include "robot_logger.h"
#include "status.h"

Status Simulator::getMissionStatus(bool is_algorithm_finished, bool is_mission_complete, bool is_battery_exhausted)
{
    if (is_mission_complete || is_algorithm_finished)
    {
        return Status::Finished;
    }

    else if (is_battery_exhausted)
    {
        return Status::Dead;
    }

    else
    {
        return Status::Working;
    }
}

void Simulator::move(Step next_step)
{
    RobotLogger& logger = RobotLogger::getInstance();

    logger.logRobotStep(next_step);

    if (Step::Finish == next_step)
    {
        return;
    }

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (Step::Stay == next_step)
    {
        if (house->isInDockingStation())
        {
            battery->charge();
        }

        else
        {
            battery->discharge();
            house->cleanCurrentPosition();
        }
    }

    else
    {
        battery->discharge();
    }
 
    house->move(next_step);
}

void Simulator::readHouseFile(const std::string& house_file_path)
{
    if (SimulatorState::Ready == state)
    {
        throw std::logic_error("Called Simulator::readHouseFile() after calling Simulator::setAlgorithm()");
    }

    RobotLogger& logger = RobotLogger::getInstance();
    logger.initializeLogFile(house_file_path);

    std::ifstream house_file;
    house_file.open(house_file_path);

    if (!house_file.is_open())
    {
        throw std::runtime_error("Couldn't open input house file!");
    }

    Deserializer::ignoreInternalName(house_file);
    max_simulator_steps = Deserializer::deserializeMaxSteps(house_file);
    battery = Deserializer::deserializeBattery(house_file);
    house = Deserializer::deserializeHouse(house_file);

    state = SimulatorState::Deserialized;
}

void Simulator::setAlgorithm(AbstractAlgorithm& chosen_algorithm)
{
    if (SimulatorState::Initial == state)
    {
        throw std::logic_error("Called Simulator::setAlgorithm() before calling Simulator::readHouseFile()");
    }

    /*
     * Use a shared pointer.
     * Pass a custom no-op deleter to prevent calling the actual destructor since
     * we don't own this object.
     */
    algorithm = &chosen_algorithm;

    algorithm->setMaxSteps(max_simulator_steps);
    algorithm->setWallsSensor(static_cast<const WallsSensor&>(*house));
    algorithm->setDirtSensor(static_cast<const DirtSensor&>(*house));
    algorithm->setBatteryMeter(static_cast<const BatteryMeter&>(*battery));

    state = SimulatorState::Ready;
}

void Simulator::run()
{
    if (SimulatorState::Ready != state)
    {
        throw std::logic_error("Called Simulator::run() before calling Simulator::readHouseFile() or Simulator::setAlgorithm()");
    }

    RobotLogger& logger = RobotLogger::getInstance();
    unsigned int total_steps_performed = 0;
    bool is_algorithm_finished = false;

    while (total_steps_performed <= max_simulator_steps)
    {
        Step next_step = algorithm->nextStep();
        move(next_step);

        if (Step::Finish == next_step)
        {
            // In case Robot mapped all accessible positions and have nothing left to clean
            is_algorithm_finished = true;
            break;
        } else {
            total_steps_performed++;
        }
    }

    unsigned int total_dirt_count = house->getTotalDirtCount();

    bool is_battery_exhausted = battery->getBatteryState() < 1;
    bool is_mission_complete = (0 == total_dirt_count) && house->isInDockingStation();
    Status status = getMissionStatus(is_algorithm_finished, is_mission_complete, is_battery_exhausted);

    logger.logCleaningStatistics(total_steps_performed, total_dirt_count, status);
}
