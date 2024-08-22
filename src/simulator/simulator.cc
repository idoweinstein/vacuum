#include "simulator.h"

#include <string>
#include <fstream>

#include "deserializer.h"
#include "robot_logger.h"
#include "status.h"

void Simulator::updateMissionStatus(Step next_step)
{
    if (Step::Finish == next_step)
    {
        if (house->isAtDockingStation())
        {
            mission_status = Status::Finished;
        }

        else
        {
            mission_status = Status::Dead;
        }
    }

    else if (!house->isAtDockingStation() && battery->isBatteryExhausted())
    {
        mission_status = Status::Dead;
    }
}

void Simulator::move(Step next_step)
{
    RobotLogger::getInstance().logRobotStep(next_step);

    if (Step::Finish == next_step)
    {
        updateMissionStatus(next_step);
        return;
    }

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (Step::Stay == next_step)
    {
        if (house->isAtDockingStation())
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
    total_steps_taken++;

    updateMissionStatus(next_step);
}

void Simulator::readHouseFile(const std::string& house_file_path, bool is_logging)
{
    if (SimulatorState::Ready == state)
    {
        throw std::logic_error("Called Simulator::readHouseFile() after calling Simulator::setAlgorithm()");
    }

    if (is_logging)
    {
        RobotLogger::getInstance().initializeLogFile(house_file_path);
    }

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

    algorithm = &chosen_algorithm;

    algorithm->setMaxSteps(max_simulator_steps);
    algorithm->setWallsSensor(static_cast<const WallsSensor&>(*house));
    algorithm->setDirtSensor(static_cast<const DirtSensor&>(*house));
    algorithm->setBatteryMeter(static_cast<const BatteryMeter&>(*battery));

    state = SimulatorState::Ready;
}

std::size_t Simulator::calculateScore(Step last_step, std::size_t dirt_count, std::size_t steps_taken, bool is_at_docking_station) const
{
    std::size_t steps = max_simulator_steps;
    if (mission_status != Status::Dead && (is_at_docking_station || mission_status == Status::Finished)) {
        steps = steps_taken;
    }

    std::size_t penalty = 0;
    if (mission_status == Status::Dead) {
        penalty = kDeadPenalty;
    } else if (last_step == Step::Finish && !is_at_docking_station) {
        penalty = kLyingPenalty;
    } else if (!is_at_docking_station) {
        penalty = kNotInDockPenalty;
    }

    return steps + dirt_count * kDirtFactor + penalty;
}

std::size_t Simulator::run()
{
    if (SimulatorState::Ready != state)
    {
        throw std::logic_error("Called Simulator::run() before calling Simulator::readHouseFile() or Simulator::setAlgorithm()");
    }

    Step next_step;
    while (total_steps_taken <= max_simulator_steps)
    {
        next_step = algorithm->nextStep();
        if (total_steps_taken == max_simulator_steps && Step::Finish != next_step)
        {
            break;
        }

        move(next_step);
        if (Status::Finished == mission_status || Status::Dead == mission_status)
        {
            break;
        }
    }

    std::size_t dirt_count = house->getTotalDirtCount();
    bool is_at_docking_station = house->isAtDockingStation();
    std::size_t score = calculateScore(next_step, dirt_count, total_steps_taken, is_at_docking_station);

    RobotLogger::getInstance().logCleaningStatistics(total_steps_taken, house->getTotalDirtCount(), mission_status, is_at_docking_station, score);

    return score;
}
