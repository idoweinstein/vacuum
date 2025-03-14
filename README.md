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
  ./bin/myrobot [-house_path=<path>] [-algo_path=<path>] [-num_threads=<num>] [-summary_only]
  ```
  - `house_path` is the directory path to read house files from.
  - `algo_path` is the directory path to read algorithm files from.
  - `num_threads` limits the number of "worker" threads (threads which simulate an algorithm - house pair).
  - `summary_only` indicates whether or not to generate summary (and errors) only.

* For example:
  ```
  ./bin/myrobot -house_path=../examples -algo_path=./bin
  ```
 * Other make targets:
 ```
 # Make simulator and algorithms and run them with houses in `examples` directory:
 make run

 # Make just the first algorithm
 make algorithm1 # Will be found in ./bin directory
 
 # Make just the second algorithm
 make algorithm2

 # Make both algorithms
 make algorithms

 # Make the simulator
 make myrobot
 ```

## Solution Approach
The house is a 4-connected grid, where each node contains either a wall, an empty space (with a predefined amount of dirt) or a docking station. <br>
In the solution there are several components, each represented by a designated class. <br>
The `Simulator` class is the main class of the project. It creates a `House` and a `Battery` which together represent the true state of the simulation at any given time. <br>
The `House` and `Battery` together implement the `DirtSensor`, `WallsSensor`, and `BatteryMeter` interfaces, which allow getting the state of the simulation without mutating it. <br>
The `Algorithm` implements the navgiation algorithm, and uses information from all sensors to make its decisions. <br>
Our chosen algorithm relies heavely on finding the shortest route from the current destionation to a designated target - unvisited or dirty positions. <br>

## Inputs and Outputs
### Input File
The input format is as follows:
```
<name>
MaxSteps = <value>
MaxBattery = <value>
Rows = <value>
Cols = <value>
<map>
```

The `name` is a string containing internal name of input file, while all `value`s are numbers.

The map is a jagged array of characters, following the legend below:
```
D	docking station
W	wall (non-navigable position)
[space]	empty (navigable) position with dirt level of 0
0	empty position with dirt level of 0, same as [space]
⋮
9	empty position with dirt level of 9
```

### Output File
The output file format is:
```
NumSteps = <NUMBER>
DirtLeft = <NUMBER>
Status = <WORKING/FINISHED/DEAD>
InDock = <TRUE/FALSE>
Score = <NUMBER>
Steps:
<values>
```

The `Steps` attribute is a list of steps where each steps is either *N*orth, *E*ast, *S*outh, *W*est, *s*tay, or *F*inished, denoted as N, E, S, W, s, or F respectively.
