<h3 align="center">Vacuum</h3>
<h5 align="center">Tel Aviv University - Advanced Topics in Programming (03683058)</h5>
<p align="center">
  A simulator of an automatic vacuum cleaner
</p>

# Vacuum
## Quick Start
* Make sure your system has GCC (and G++) 13, CLANG 14 or MSVC 16.10/VS 2019. Newer versions are also OK. <br>
  If necessary, set your default compiler to be the newly installed compiler. <br>
  For example:
  ```
  export CC=/bin/gcc-13
  export CXX=/bin/g++-13
  ```
  * These versions fully support `std::format`.
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
  * Example input and output files can be found in `tests` and `tests/inputs`.
