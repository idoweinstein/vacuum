import argparse
import os
import re
import time


DOCKING_STATION = '@'
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


def get_next_position(line):
    groups = re.search(r'\(([0-9]+),([0-9]+)\)', line).groups()
    return (int(groups[0]), int(groups[1]))


def simulate(max_robot_steps, max_battery_steps, start_position, map, output_file, fps):
    with output_file as f:
        lines = f.readlines()

    docking_station_position = start_position
    current_position = start_position
    current_step = 0
    current_battery = max_battery_steps

    # Make windows show ANSI colors
    # Source: https://stackoverflow.com/questions/12492810/python-how-can-i-make-the-ansi-escape-codes-to-work-also-in-windows#answer-64222858
    os.system('')
    print('Start')
    print_frame(map, current_position, current_battery, max_battery_steps, current_step, max_robot_steps, False)
    for line in lines:
        if line.startswith('[STEP]'):
            time.sleep(1 / fps)

            # Update
            next_position = get_next_position(line)
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
        else:
            print(line)


def parse_input_file(input_file):
    max_robot_steps = 0
    max_battery_steps = 0
    start_position = (0, 0)
    map = []
    with input_file as f:
        lines = f.readlines()

    for line in lines:
        if line.startswith('max_robot_steps'):
            max_robot_steps = int(line.split(' ')[1])
        elif line.startswith('max_battery_steps'):
            max_battery_steps = int(line.split(' ')[1])
        elif line.startswith('house'):
            continue
        else:
            if DOCKING_STATION in line:
                start_position = (len(map), line.index(DOCKING_STATION))
            line = line.replace(ZERO_DIRT, CLEAR_BLOCK).strip('\r\n')
            map.append(list(line))
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
    simulate(max_robot_steps, max_battery_steps, start_position, map, args.output_file, args.fps)
    input('[ Press enter to exit ]')


if __name__ == '__main__':
    main()
