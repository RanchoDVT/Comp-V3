#include <stdlib.h>
#include <map>

#include "v5_cpp.h"

/**
 * @class Log
 * @brief Handles logging with different severity levels.
 * @details Provides an enumeration for log levels.
 */
class Log
{
public:
    /**
     * @enum Level
     * @brief Defines the severity levels for logging.
     */
    enum class Level
    {
        Trace, ///< Detailed information.
        Debug, ///< Debug-level messages.
        Info,  ///< Informational messages that highlight the progress of the application.
        Warn,  ///< Potentially harmful situations.
        Error, ///< Error events that still allow the application to continue running.
        Fatal  ///< Very severe error events that will lead the application to abort.
    };
};

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

/**
 * @brief Clears the screens of various devices.
 * @param brainClear Whether to clear the screen on the V5 Brain.
 * @param primaryControllerClear Whether to clear the screen on the primary controller.
 * @param partnerControllerClear Whether to clear the screen on the partner controller.
 */
void clearScreen(const bool &brainClear, const bool &primaryControllerClear, const bool &partnerControllerClear);

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

#include "config/robot-config.h"
#include "config/extern/configManager.h"

/// @brief Manages configuration settings.
extern configManager ConfigManager;

#include "display/gifdec.h"

struct ControllerButtonInfo
{
    const vex::controller::button *button;
    std::string name;
};

extern std::array<ControllerButtonInfo, 12> AllControllerButtons;

std::array<ControllerButtonInfo, 12> createControllerButtonArray(const vex::controller &controller);
std::array<ControllerButtonInfo, 12> getControllerButtonArray(const vex::controller &controller);