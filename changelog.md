# Version history 📕

## Version 2.0pr3.3 | 5/7/24 (Newest)

Build size: 367KB

### Added

- Added a new function `resetOrInitializeConfig` to reset or initialize the configuration file based on user input.
- Added a new function `stringtobool` to convert a string representation of boolean values to a bool.
- Added a new function `stringtofloat` to convert a string representation of a float to a float value.
- Added a new function `configParser` to parse the configuration file and initialize variables.
- Added error handling for file operations to ensure robustness.
- Added missing include directives for necessary headers.

### Modified

- Modified the existing function `setValForConfig` to handle unknown keys in the configuration file by either resetting the config or ignoring them.
- Modified the existing code to ensure consistent naming conventions throughout the file.
- Modified the `resetOrInitializeConfig` function to accept boolean parameters by reference for improved performance.
- Modified the `stringtobool` function to use the `std::string::find` method for more accurate boolean conversion.
- Modified the `stringtofloat` function to handle non-numeric input by resetting the config.
- Modified the `configParser` function to handle the absence of the readme file and reset the config accordingly.
- Modified variable initialization for improved readability and consistency.

### Removed

- Removed redundant code and replaced it with more efficient alternatives.
- Removed unnecessary sleep commands to improve code execution speed.
- Removed unused variables and function parameters.

### Fixed

- Fixed a bug in the `stringtobool` function where the logical OR operator was incorrectly used.
- Fixed a bug in the `configParser` function where the existence of the readme file was not properly checked.
- Fixed minor syntax errors and typos throughout the code.

### Optimized

- Optimized the code for better performance and readability.
- Optimized file handling operations to reduce redundancy and improve efficiency.
- Optimized boolean value parsing for improved accuracy and reliability.

### Documented

- Documented each function with brief descriptions and parameter details.
- Documented the change log to provide a summary of the modifications made to the code.
- Added comments to clarify the purpose and functionality of each code section.

## Version 2.0pr2 4/14/24

Build size: 325KB

### New: 🎉

- Updated from C++11 to C++ 20.
- New class: `Log::Level`
- New armVol for controlling the arm voltage setting.
- New scrolling support for `get_User_Option`.
- New function `motorTempMonitor` that shows motor temps and battery voltage on the controller screen.
- Utilization of the bummerA.

### Fixed / Updated: 🛠️

- Updated `get_user_Option` for more protections.
- Fixed bug in `get_User_Option` that when the user fails the log for what buttons are a repeat.
- Fixed bug that does not show % on the controller screen. (It requires %%)
- Performance fixes by using references.

## Version 1.0.0 3/9/24

Build size: 280KB

### New: 🎉

- Added a function `clear_Screen` to clear the controller and optionally the brain`s screen.
- Added a function `error_handler` to handle and display error messages with optional output to the controller`s screen and colored output to the console.
- Implemented autonomous and user control tasks with threading to run continuously during the respective phases of a VEX Competition.
- Implemented main function to set up competition callbacks for autonomous and driver control periods, start GUI startup, and handle continuous control of the drivetrain while preventing the main loop from exiting.

### Fixed / Updated: 🛠

- Faster execution time with `ctrl1_Bttn_Presd` using break to mimic a switch and case.
- Improved `ctrl1_Bttn_Presd` print statement, now using a local variable as the way to print it on Debug_Mode.
- Updated `ctrl1_Bttn_Presd` function to include error handling and prevent function execution if `controller1Command` flag is set.
- Updated function `get_User_Option` to display options and get user input with error handling and prevention of function execution if `controller1Command` flag is set.
- Updated `controller_L1_Pressed`, `controller_L2_Pressed`, `controller_R1_Pressed`, and `controller_R2_Pressed` functions to include error handling, to replace the `rc_auto_loop_function_Controller1`, and prevent function execution if `controller1Command` flag is not set.
- Updated the `setup` function to create controller callback events, new safety checks onm startup.
- Improved polling rate from 50Hz to 8000Hz on `ctrl1_Bttn_Presd` and the controller loop. (160X faster than the competition.)
- Improved Global variables and function naming.
- Improved comments.
- Lots of performance improvements.
- Lots of spelling fixes.

## Version 0.3.0a6 / 1/27/24

Build size: 260KB  

### New: ✨

- The `rc_auto_loop_function_Controller1` is now run as a separate thread using vex::thread. This allows the main program to continue executing other tasks concurrently.
- Thread creation and execution are printed to the console for debugging purposes.
- Added print statements to indicate the start and completion of threading and callback operations.

### Fixed: 🛠️

- Lots of spelling fixes.

## Version 0.3.0a5 / 1/26/24

Build size: 260KB  

### New: ✨

- Added a debug mode for easier debugging. (Prints useful debug data to the console)
- Created a new task (`rc_auto_loop_function_Controller1`) to monitor inputs from Controller1 in a loop, replacing multiple event handlers.
- Added a new user control task (`usercontrol`) to manage user control operations.
- Removed 3 global variables (`major`, `minor`, `patch`) in favor of a new one (`Version`).

### Fixed / Improved: 🛠️

- Updated print statements for better readability.
- Improved organization and comments for better code structure.
- Enhanced the control of motors based on joystick inputs for better responsiveness.
- Corrected the print color for the main function.
- Made new loops in GetUserOption for displaying options and general improvements to it.
- Removed unnecessary delays and added appropriate waits for better performance.
- Removed redundant statements in the setup function.

### Removed: ❌

- Removed unnecessary print statements for button presses.

## 0.3.0a4 / 1/23/24

Build size: 259KB  

### New: ✨

- New images with data!

### Fixed / Improved: 🛠️

- General code fixes.

### Removed: ❌

- `vex.h`, `robot-config.h`, `robot-config.cpp`

## 0.3.2a1 / 1/22/24

### New: ✨

- New files! (`robot-config.h`, `robot-config.cpp`)
- Made all code modular.
- Autonomous control functions ready for writing.
- Improved `getUserOption` functions.
- Version printing.

### Fixed / Improved: 🛠️

- Main from exiting with an infinite loop.
- Reduced file size from 563 KB to 258KB. (a 218% size reduction!) This required me to remove some standard library headers.
- (Removed: `stdlib.h`, `string.h`, `math.h`. Added: `string`, `iostream`)
- Better and *WAY* more comments.
- Fix for inconsistency with 2 spaces vs a tab. This has been converted to 2 spaces.
- Fix `ctrl1_Bttn_Presd` for if a user presses anything other than A it would return nothing.

### Removed: ❌

- Vex Sound play helper.
- Input reading from controller1 as a task.
