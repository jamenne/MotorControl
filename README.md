# Control TRINAMIC motors
 Author: Janine Müller

## Description
Motors by TRINAMIC are connected to the PC via RS232.  
This software allows to send TRINAMIC Opcodes via RS232 to the motor, to control it (see [TRINAMIC Documentaion][1]).  
It has the basic operations implemented as C++ functions.  
The `CalcStepsX` and `CalcStepsY` functions are based on the gear ratio of the xy table.

The repository contains the source code as well as CMakeLists and a GUI version to operate such system.

## Requirements to run terminal program

Availability of the following libraries (author: Janine Müller):
* [RS232Communication][4]


## How to compile it
Compile source code:

```bash
mkdir build
cd build
cmake ..
make
```

## How to use it
e.g. in `CMakeLists.txt` of `XYTable:  

```bash
add_library(MotorControl ../MotorControl/Motor.cpp)
```

[1]: http://www.mctechnology.nl/pdf/TMCL_reference_2015.pdf
[4]: https://git.e5.physik.tu-dortmund.de/jmueller/RS232communication