#include "simulator.h"

#include <string>
#include <fstream>
#include <iostream>
#include <stop_token>

#include "enum_operators.h"
#include "deserializer.h"
#include "status.h"

Simulator::Simulator(const HouseFile& house_file)
    : max_simulator_steps(house_file.max_steps),
      house(house_file.house),
      battery(house_file.battery)
{}

void Simulator::updateMissionStatus(Step next_step)
{
    if (Step::Finish == next_step)
    {
        if (house.isAtDockingStation())
        {
            statistics.mission_status = Status::Finished;
        }

        else
        {
            statistics.mission_status = Status::Dead;
        }
    }

    else if (!house.isAtDockingStation() && battery.isBatteryExhausted())
    {
        statistics.mission_status = Status::Dead;
    }
}

void Simulator::move(Step next_step)
{
    statistics.step_history.emplace_back(next_step);

    if (Step::Finish == next_step)
    {
        updateMissionStatus(next_step);
        return;
    }

    /* If no battery left - discharge() throws an Empty Battery exception */
    if (Step::Stay == next_step)
    {
        if (house.isAtDockingStation())
        {
            battery.charge();
        }

        else
        {
            battery.discharge();
            house.cleanCurrentPosition();
        }
    }

    else
    {
        battery.discharge();
    }
 
    house.move(next_step);
    statistics.num_steps_taken++;

    updateMissionStatus(next_step);
}

void Simulator::setAlgorithm(AbstractAlgorithm& chosen_algorithm)
{
    algorithm = &chosen_algorithm;

    algorithm->setMaxSteps(max_simulator_steps);
    algorithm->setWallsSensor(static_cast<const WallsSensor&>(house));
    algorithm->setDirtSensor(static_cast<const DirtSensor&>(house));
    algorithm->setBatteryMeter(static_cast<const BatteryMeter&>(battery));

    state = SimulatorState::Ready;
}

void Simulator::calculateScore(Step last_step)
{
    std::size_t steps = statistics.num_steps_taken;

    std::size_t penalty = 0;
    if (isDeadScoring(last_step))
    {
        steps = max_simulator_steps;
        penalty = kDeadPenalty;
    }

    else if (isLyingScoring(last_step))
    {
        steps = max_simulator_steps;
        penalty = kLyingPenalty;
    }

    else if (!house.isAtDockingStation())
    {
        penalty = kNotInDockPenalty;
    }

    statistics.score = steps + house.getTotalDirtCount() * kDirtFactor + penalty;
}

std::size_t Simulator::run(std::stop_token stop_token)
{
    if (SimulatorState::Ready != state)
    {
        throw std::logic_error("Called Simulator::run() before calling Simulator::setAlgorithm()");
    }

    Step next_step = Step::Finish;
    if (statistics.num_steps_taken > max_simulator_steps)
    {
        throw std::runtime_error("Simulator exceeded the maximal allowed steps!");
    }

    while (statistics.num_steps_taken <= max_simulator_steps)
    {
        if (stop_token.stop_requested())
        {
            break;
        }

        next_step = algorithm->nextStep();
        if (statistics.num_steps_taken == max_simulator_steps && Step::Finish != next_step)
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
