#include <stdlib.h>

#include <string>  ///< Required for using string objects
#include <fstream> ///< Requried for using std::getline, and making and reading files.
#include <vector>
#include <cmath>	 ///< Required for using std::abs with an int
#include <algorithm> ///< Requried for using ::isdigit and std::any_of
#include <sstream>
#include <inttypes.h>

#include <stdlib.h>	 ///< Required for standard library definitions
#include <stdbool.h> ///< Required for standard boolean definitions
#include <math.h>	 ///< Required for mathematical functions
#include <string.h>	 ///< Required for string manipulation functions
#include <stdio.h>	 ///< Required for standard input/output definitions

#include "v5_cpp.h"

/**
 * @author @Voidless7125
 * @date 4/10/24
 * @brief Class for `Log`.
 * @public enum class `Level`.
 */
class Log
{
public:
    enum class Level
    {
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };
};

/// @brief Version Number of code.
extern std::string Version;

/// @brief Build date of code.
extern std::string BuildDate;

/// @brief Handles logging, to sd card if supported, to user if warn and higher, and outputs it in the console.
/// @param functionName Name of function that you want to log.
/// @param message The message! (If you can't understand this, you don't deserve a computer)
/// @param level The log level (from the class Log::Level)
/// @param timeOfDisplay How long the message stays on the screen for.
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level, const float &timeOfDisplay = 2);


/// @brief GUI for settting options
/// @param settingName The name for what you want to change (duh)
/// @param options The options you want (duh), in a vector like this: {"Opt1", "Opt2"}
/// @return a string output of what got selected.
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options);

/// @brief Helper cmd for clearing screens on one line of code.
/// @param brainClear Wither to clear the screen on the V5 Brain
/// @param primaryControllerClear Wither to clear the screen on the 1st controller
/// @param partnerControllerClear Wither to clear the screen on the 2nd controller
void clearScreen(const bool &brainClear, const bool &primaryControllerClear, const bool &partnerControllerClear);

/// @brief Function to calibrate Gyro
void calibrateGyro();

/// @brief Caller for autonomous code
void autonomous();

/// @brief
void userControl();

/// @brief
void motorMonitor();

/// @brief Gets input from the controller, NO GUI.
/// @return Time of button held (int), and what button got pressed (string)
std::pair<std::string, int> ctrl1BttnPressed();

/// @brief Called to start the gif player.
void gifplayer();

#include "config/robot-config.h"

#include "config/extern/configManager.h"

extern configManager ConfigManager;

#include "display/gifdec.h"