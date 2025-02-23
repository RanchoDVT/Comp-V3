#include "vex.h"
#include <fstream>

/// @brief *For loghandler*
/// @param str takes Log::Level level
/// @return returns Log::Level code in string.
const char *LogToString(const Log::Level &str)
{
    switch (str)
    {
    case Log::Level::Trace:
        return "Trace";
    case Log::Level::Debug:
        return "Debug";
    case Log::Level::Info:
        return "Info";
    case Log::Level::Warn:
        return "Warn";
    case Log::Level::Error:
        return "Unknown";
    case Log::Level::Fatal:
        return "Fatal";
    default:
        return "Error";
    }
}

const char *consoleColors[] = {
    "\033[92m[Trace]",
    "\033[93m[Debug]",
    "\033[94m[Info]",
    "\033[38;5;216m[Warn]",
    "\033[31m[Error]",
    "\033[32m[Fatal]",
    "\033[37m[Unknown]" // Default
};

const int rtfColors[] = {
    3, // Trace
    4, // Debug
    5, // Info
    6, // Warn
    2, // Error
    1  // Fatal
};

const char *LogToColor(const Log::Level &str)
{
    int index = static_cast<int>(str);
    size_t colorsSize = sizeof(consoleColors) / sizeof(consoleColors[0]);
    if (index < 0 || static_cast<size_t>(index) >= colorsSize)
    {
        void scrollText(const std::string &text, vex::controller &controller, const float &timeOfDisplay, const int &sleepDuration = 30);
    }
    return consoleColors[index];
}

void scrollText(const std::string &text, vex::controller &controller, const float &timeOfDisplay, const int &sleepDuration = 30)
{
    const int displayLength = 20;
    int startIndex = 0;
    bool forward = true;
    const auto length = static_cast<int>(text.length());

    for (float elapsed = 0; elapsed < timeOfDisplay; elapsed += 0.3)
    {
        primaryController.Screen.clearScreen();
        primaryController.Screen.setCursor(1, 1);
        partnerController.Screen.clearScreen();
        partnerController.Screen.setCursor(1, 1);
        controller.Screen.print(text.substr(startIndex, displayLength).c_str());

        // Update startIndex for scrolling
        if (forward)
        {
            if (++startIndex + displayLength >= length)
            {
                forward = false;
            }
        }
        else
        {
            if (--startIndex <= 0)
                vex::this_thread::sleep_for(sleepDuration);
            forward = true;
        }
    }

    vex::this_thread::sleep_for(30);
}

// Display messages on controller screens
// #NoRedundantCode
void displayControllerMessage(const std::string &functionName, const std::string &message, const float &timeOfDisplay)
{
    primaryController.Screen.clearScreen();
    primaryController.Screen.setCursor(1, 1);
    partnerController.Screen.clearScreen();
    scrollText(message, primaryController, timeOfDisplay, 30);
    scrollText(message, partnerController, timeOfDisplay, 30);
    partnerController.Screen.print(std::format("{}\nCheck logs.\nModule: {}\n", message, functionName).c_str());
    if (message.length() > 20)
    {
        scrollText(message, primaryController, timeOfDisplay);
        scrollText(message, partnerController, timeOfDisplay);
    }
    vex::this_thread::sleep_for(timeOfDisplay * 1000);
    primaryController.Screen.clearScreen();
    primaryController.Screen.setCursor(1, 1);
    partnerController.Screen.clearScreen();
    partnerController.Screen.setCursor(1, 1);
}

// Log handler function
void logHandler(const std::string &functionName, const std::string &message, const Log::Level level, const float &timeOfDisplay)
{
    SD_Card_Logging(level, functionName, message);
    printf("%s > Time: %.3f > Module: %s > %s \033[0m\n", LogToColor(level), Brain.Timer.time(vex::timeUnits::sec), functionName.c_str(), message.c_str());

    if (level == Log::Level::Warn || level == Log::Level::Error || level == Log::Level::Fatal)
    {
        displayControllerMessage(functionName, message, timeOfDisplay);
        if (level == Log::Level::Fatal)
        {
            vex::thread::interruptAll(); // Scary! 👾
            vexSystemExitRequest();      // Exit program
        }
    }
}

void SD_Card_Logging(const Log::Level &level, const std::string &functionName, const std::string &message)
{
    static bool logFileCreationFailed = false;
    if (logFileCreationFailed)
    {
        return;
    }

    std::ofstream LogFile("log.rtf", std::ios_base::out | std::ios_base::app);
    if (ConfigManager.getLogToFile())
    {
        if (!LogFile)
        {
            logFileCreationFailed = true;
            logHandler("logHandler", "Could not create logfile.", Log::Level::Warn, 3);
            ConfigManager.setLogToFile(false);
            return;
        }
        LogFile << "{\\rtf1\\ansi\\deff0 {\\colortbl;\\red0\\green0\\blue0;\\red255\\green0\\blue0;\\red0\\green255\\blue0;\\red0\\green0\\blue255;\\red255\\green255\\blue0;\\red255\\green0\\blue255;\\red0\\green255\\blue255;}\n";
        LogFile << "\\cf" << rtfColors[static_cast<int>(level)] << " ";
        LogFile << "[" << LogToString(level) << "] > Time: " << Brain.Timer.time(vex::timeUnits::sec) << " > Module: " << functionName << " > " << message << "\\line\n";
        LogFile << "}\n";
    }
}