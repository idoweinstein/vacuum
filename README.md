<div style="clear: both">
  <h3 style="display: inline-block" align="center">Vacuum - by Amit Gabay & Ido Weinstein</h3>
</div>
<h5 align="center">Tel Aviv University - Advanced Topics in Programming (03683058)</h5>
<p align="center">
  A simulator of an automatic vacuum cleaner
</p>

# Vacuum
## Quick Start
* Clone this repository.
* Build the project:
  ```
  mkdir build
  cd build
  cmake ..
  make
  ```
* Run it:
  ```
  ./myrobot <inputfile>
  ```
  * Example input and output files can be found in `examples`, `tests`, and `tests/inputs`.

## Solution Approach
The house is a 4-connected grid, where each node contains either a wall, an empty space (with a predefined amount of dirt) or a docking station. <br>
In the solution there are several components, each represented by a designated class. <br>
The `Robot` class is the main class of the project. It creates a `LocationManager` and a `BatteryManager` which together represent the true state of the simulation at any given time. <br>
The `LocationManager` and `BatteryManager` together implement the `DirtSensor`, `WallSensor`, and `BatterySensor` interfaces, which allow getting the state of the simulation without mutating it.
The `NavigationSystem` implements the navgiation algorithm, and uses information from all sensors to make its decisions.
More information can be found in the HLD document.

## Inputs and Outputs
### Input File
The input format is as follows:
```
max_robot_steps <value>
max_battery_steps <value>
house
<map>
```

The `max_robot_steps` and `max_battery_steps` values must be non-negative integers.
The map is a jagged array of characters, following the legend below:
```
@	docking station
X	wall (non-navigable position)
[space]	empty (navigable) position with dirt level of 0
0	empty position with dirt level of 0, same as [space]
⋮
9	empty position with dirt level of 9
```

### Output File
The output file format is:
```
[Step] Robot took step to <direction> - New Position <y, x>
 ⋮
[Step] Robot took step to <direction> - New Position <y, x>
### Program Terminated ###
Total Steps Taken: <value>
Total Dirt Left: <value>
Is Battery Exhausted: <value>
Mission Succeeded: <value>
```

where the following line might also appear:
```
[FINISH] Robot finished cleaning all accessible places!
```
