import argparse
import os
import re
import time


DOCKING_STATION = 'D'
CLEAR_BLOCK = ' '
ZERO_DIRT = '0'

CLEAR_SCREEN = '\033c'
COLOR_START = '\033['
REGULAR_STYLE = '0;37;40m'
CURRENT_STYLE = '0;30;43m'
CURRENT_CHARGING_STYLE = '1;37;42m'
COLOR_END = '\033[0m'


def clear_screen():
    print(CLEAR_SCREEN, end='')


def print_empty_line():
    print('')


def get_character_style(on_current_position, is_charging):
    if not on_current_position:
        return REGULAR_STYLE
    if is_charging:
        return CURRENT_CHARGING_STYLE
    return CURRENT_STYLE


def print_frame(map, current_position, current_battery, max_battery_steps, current_step, max_robot_steps, is_stay):
    clear_screen()
    print_empty_line()
    print()

    is_robot_charging = False
    for y, row in enumerate(map):
        print('\t', end='')
        for x, cell in enumerate(row):
            on_current_position = (current_position == (y, x))
            is_current_position = on_current_position and is_stay and cell == DOCKING_STATION
            is_robot_charging = is_robot_charging or is_current_position
            style = get_character_style(on_current_position, is_current_position)

            print(COLOR_START + style + cell + COLOR_END, end='')
        print_empty_line()

    print_empty_line() # Empty line
    if is_robot_charging:
        print(f'\t[ Charging ]')
    print(f'\tBattery: {current_battery}/{max_battery_steps}')
    print(f'\tStep: {current_step}/{max_robot_steps}')
    print_empty_line() # Empty line


def get_next_position(current_position, step):
    diffs = {
        'N': (-1, 0),
        'S': (1, 0),
        'W': (0, -1),
        'E': (0, 1),
        's': (0, 0),
        'F': (0, 0)
    }

    return (current_position[0] + diffs[step][0], current_position[1] + diffs[step][1])


def simulate(max_robot_steps, max_battery_steps, start_position, map, steps, fps):
    docking_station_position = start_position
    current_position = start_position
    current_step = 0
    current_battery = max_battery_steps

    # Make windows show ANSI colors
    # Source: https://stackoverflow.com/questions/12492810/python-how-can-i-make-the-ansi-escape-codes-to-work-also-in-windows#answer-64222858
    os.system('')
    print('Start')
    print_frame(map, current_position, current_battery, max_battery_steps, current_step, max_robot_steps, False)
    for step in steps:
        time.sleep(1 / fps)

        # Update
        next_position = get_next_position(current_position, step)
        is_stay = next_position == current_position
        current_position = next_position
        current_step += 1
        # Update batery
        if is_stay and current_position == docking_station_position:
            current_battery = min(max_battery_steps, current_battery + max_battery_steps / 20)
            # Avoid printing unnecessary decimal point if not needed
            if current_battery == int(current_battery):
                current_battery = int(current_battery)
        else:
            current_battery -= 1
        # Update dirt level
        if map[current_position[0]][current_position[1]].isnumeric() and is_stay:
            updated_dirt_level = int(map[current_position[0]][current_position[1]]) - 1
            updated_block_representation = CLEAR_BLOCK
            if updated_dirt_level > 0:
                updated_block_representation = str(updated_dirt_level)
            map[current_position[0]][current_position[1]] = updated_block_representation

        # Print
        print_frame(map, current_position, current_battery, max_battery_steps, current_step, max_robot_steps, is_stay)


def pase_output_file(output_file):
    with output_file as f:
        content = f.read()

    regex = (r"""NumSteps\s*=\s*(?P<num_steps>\d+)[\r]?\n""" +
             r"""DirtLeft\s*=\s*(?P<dirt_left>\d+)[\r]?\n""" +
             r"""Status\s*=\s*(?P<status>\w+)[\r]?\n""" +
             r"""Steps:[\r]?\n""" +
             r"""(?P<steps>(?:N|E|S|W|s|F)+)[\r]?\n?""")

    match = re.search(regex, content)

    if not match:
        raise ValueError('Invalid output file format')

    d = match.groupdict()

    return int(d['num_steps']), int(d['dirt_left']), d['status'], d['steps']


def parse_house(house):
    start_position = (0, 0)
    map = []
    is_station_found = False

    lines = house.replace('\r', '').split('\n')

    for line in lines:
        if DOCKING_STATION in line:
            if not is_station_found:
                is_station_found = True
                start_position = (len(map), line.index(DOCKING_STATION))
            else:
                raise ValueError('Multiple docking stations found')
        map.append(list(line))

    if not is_station_found:
        raise ValueError('No docking station found')

    return start_position, map


def parse_input_file(input_file):
    with input_file as f:
        content = f.read()

    regex = (
        r"""(?P<name>.*)[\r]?\n""" +
        r"""MaxSteps\s*=\s*(?P<max_robot_steps>\d+)[\r]?\n""" +
        r"""MaxBattery\s*=\s*(?P<max_battery_steps>\d+)[\r]?\n""" +
        r"""Rows\s*=\s*(?P<rows>\d+)[\r]?\n""" +
        r"""Cols\s*=\s*(?P<cols>\d+)[\r]?\n""" +
        r"""(?P<house>(?:.|\n|\r)+)$"""
    )

    match = re.search(regex, content, re.M)

    if not match:
        raise ValueError('Invalid input file format')

    d = match.groupdict()

    max_robot_steps = int(d['max_robot_steps'])
    max_battery_steps = int(d['max_battery_steps'])
    start_position, map = parse_house(d['house'])

    return max_robot_steps, max_battery_steps, start_position, map


def parse_args():
    parser = argparse.ArgumentParser(
                    prog='simulate',
                    description='Visualize the simulation of the robot')


    parser.add_argument('input_file', type=argparse.FileType('r'), help='The input file containing the map and start parameters')
    parser.add_argument('output_file', type=argparse.FileType('r'), help='The output file containing the robot log')
    parser.add_argument('--fps', type=float, default=1, help='The frame rate of the simulation in frames per second')

    return parser.parse_args()


def main():
    args = parse_args()
    max_robot_steps, max_battery_steps, start_position, map = parse_input_file(args.input_file)
    num_steps, dirt_left, status, steps = pase_output_file(args.output_file)
    simulate(max_robot_steps, max_battery_steps, start_position, map, steps, args.fps)
    print_empty_line()
    print(f'NumSteps: {num_steps}')
    print(f'DirtLeft: {dirt_left}')
    print(f'Status: {status}')
    input('[ Press enter to exit ]')


if __name__ == '__main__':
    main()
