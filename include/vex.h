#include <stdlib.h>
#include <map>

#include "v5_cpp.h"

#include "config/robot-config.h"
#include "config/extern/configManager.h"

#include "display/gifdec.h"

/// @brief Version number of the code.
extern std::string Version;

/// @brief Build date of the code.
extern std::string BuildDate;

/**
 * @brief Handles logging to various outputs.
 * @param functionName The name of the function where the log is generated.
 * @param message The log message.
 * @param level The severity level of the log (from Log::Level).
 * @param timeOfDisplay Duration for which the message is displayed on the screen.
 */
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level, const float &timeOfDisplay = 2);

/**
 * @brief Logs messages to the SD card if supported.
 * @param level The severity level of the log (from Log::Level).
 * @param functionName The name of the function where the log is generated.
 * @param message The log message.
 */
void SD_Card_Logging(const Log::Level &level, const std::string &functionName, const std::string &message);

/**
 * @brief Displays a GUI for setting options.
 * @param settingName The name of the setting to be changed.
 * @param options A vector of options to choose from.
 * @return The selected option as a string.
 */
std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options);

/// @brief Calibrates the gyro sensor.
void calibrateGyro();

/// @brief Executes autonomous code.
void autonomous();

/// @brief Executes user control code.
void userControl();

/// @brief Monitors motor status.
void motorMonitor();

/**
 * @brief Gets input from the controller without a GUI.
 * @param controller The controller to get input from.
 * @return A map containing the button pressed and the duration it was held.
 */
std::map<std::string, std::vector<int>> controllerButtonsPressed(const vex::controller &controller);

/// @brief Starts the GIF player.
void gifplayer();