#include "simulator.h"

#include <string>
#include <fstream>
#include <iostream>

#include "enum_operators.h"
#include "deserializer.h"
#include "status.h"

void Simulator::updateMissionStatus(Step next_step)
{
    if (Step::Finish == next_step)
    {
        if (house->isAtDockingStation())
        {
            statistics.mission_status = Status::Finished;
        }

        else
        {
            statistics.mission_status = Status::Dead;
        }
    }

    else if (!house->isAtDockingStation() && battery->isBatteryExhausted())
    {
        statistics.mission_status = Status::Dead;
    }
}

void Simulator::move(Step next_step)
{
    statistics.steps_taken << next_step;

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
    statistics.total_steps_taken++;

    updateMissionStatus(next_step);
}

void Simulator::readHouseFile(const std::string& house_file_path)
{
    if (SimulatorState::Ready == state)
    {
        throw std::logic_error("Called Simulator::readHouseFile() after calling Simulator::setAlgorithm()");
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

void Simulator::calculateScore(Step last_step)
{
    std::size_t steps = max_simulator_steps;
    if (statistics.mission_status == Status::Finished && house->isAtDockingStation())
    {
        steps = statistics.total_steps_taken;
    }

    std::size_t penalty = 0;
    if (statistics.mission_status == Status::Dead)
    {
        penalty = kDeadPenalty;
    }

    else if (last_step == Step::Finish && !house->isAtDockingStation())
    {
        penalty = kLyingPenalty;
    }

    else if (!house->isAtDockingStation())
    {
        penalty = kNotInDockPenalty;
    }

    statistics.score = steps + house->getTotalDirtCount() * kDirtFactor + penalty;
}

std::size_t Simulator::run()
{
    if (SimulatorState::Ready != state)
    {
        throw std::logic_error("Called Simulator::run() before calling Simulator::readHouseFile() or Simulator::setAlgorithm()");
    }

    Step next_step;
    while (statistics.total_steps_taken <= max_simulator_steps)
    {
        next_step = algorithm->nextStep();
        if (statistics.total_steps_taken == max_simulator_steps && Step::Finish != next_step)
        {
            break;
        }

        move(next_step);
        if (Status::Finished == statistics.mission_status || Status::Dead == statistics.mission_status)
        {
            break;
        }
    }

    calculateScore(next_step);
    return statistics.score;
}
