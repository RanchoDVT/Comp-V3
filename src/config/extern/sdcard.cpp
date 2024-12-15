#include "vex.h"

#include <fstream>
#include <sstream>

// Method to reset or initialize the config file
void configManager::resetOrInitializeConfig(const std::string &message)
{
    maxOptionSize = 4;
    std::string resetcfg = getUserOption(message, {"Yes", "No"});
    if (resetcfg == "Yes")
    {
        primaryController.Screen.print("Resetting config file...");

        std::ofstream configFile(configFileName);
        if (!configFile)
        {
            logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn, 3);
            return;
        }
        // Write default configuration to the file
        configFile << ";Config File:\n";
        configFile << "POLLINGRATE=1\n";
        configFile << "PRINTLOGO=true\n";
        configFile << "LOGTOFILE=true\n";
        configFile << "MAXOPTIONSSIZE=4\n";
        configFile << "CTRLR1POLLINGRATE=25\n";
        configFile << "LOGLEVEL=Info\n";
        configFile << "VERSION=" << Version << "\n";
        configFile.close();
    }
    logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);
}

// Method to convert a string to boolean
bool configManager::stringToBool(const std::string &str)
{
    return (str == "true" || str == "1");
}

// Method to convert a string to long
long configManager::stringToLong(const std::string &str)
{
    try
    {
        return std::stol(str);
    }
    catch (const std::invalid_argument &e)
    {
        logHandler("stringToLong", "Invalid argument: " + str, Log::Level::Error);
        return 0;
    }
    catch (const std::out_of_range &e)
    {
        logHandler("stringToLong", "Out of range: " + str, Log::Level::Error);
        return 0;
    }
}

// Method to set values from the config file
void configManager::setValuesFromConfig()
{
    std::ifstream configFile(configFileName);
    if (!configFile)
    {
        logHandler("setValForConfig", "Could not open config file. Reason Unknown.", Log::Level::Warn, 4);
        return;
    }

    std::string configLine;
    while (std::getline(configFile, configLine))
    {
        if (configLine.empty() || configLine[0] == ';' || configLine[0] == '#')
        {
            continue; // Skip empty lines and comments
        }

        std::istringstream iss(configLine);
        std::string key, value;
        if (std::getline(iss, key, '=') && std::getline(iss, value))
        {
            if (key == "ConfigType")
            {
                configType = stringToConfigType(value);
            }
            else if (key == "TeamNumber")
            {
                setTeamNumber(value);
            }
            else if (key == "LoadingGifPath")
            {
                setLoadingGifPath(value);
            }
            else if (key == "AutoGifPath")
            {
                setAutoGifPath(value);
            }
            else if (key == "DriverGifPath")
            {
                setDriverGifPath(value);
            }
            else if (key == "CustomMessage")
            {
                setCustomMessage(value);
            }
            else if (key == "PRINTLOGO")
            {
                setPrintLogo(stringToBool(value));
            }
            else if (key == "LOGTOFILE")
            {
                setLogToFile(stringToBool(value));
            }
            else if (key == "MAXOPTIONSSIZE")
            {
                setMaxOptionSize(stringToLong(value));
            }
            else if (key == "POLLINGRATE")
            {
                setPollingRate(stringToLong(value));
            }
            else if (key == "CTRLR1POLLINGRATE")
            {
                setCtrlr1PollingRate(stringToLong(value));
            }
            else if (key == "LOGLEVEL")
            {
                setLogLevel(stringToLogLevel(value));
            }
            else if (key == "VERSION")
            {
                if (value != Version)
                {
                    resetOrInitializeConfig("Version mismatch with Config file (" + value + ") and code version (" + Version + "). Do you want to reset the config file?");
                }
            }
            else
            {
                resetOrInitializeConfig("Unknown key in config file: " + key + ". Do you want to reset the config?");
            }
        }
        else if (key == "MOTOR_CONFIG" || key == "INERTIAL" || key == "TRIPORT_CONFIG")
        {
            parseComplexConfig(configFile, key);
        }
        else
        {
            resetOrInitializeConfig("Invalid line in config file: " + configLine + ". Do you want to reset the config?");
        }
    }
    configFile.close();
}

// Method to parse complex config sections
void configManager::parseComplexConfig(std::ifstream &configFile, const std::string &section)
{
    std::string configLine;
    while (std::getline(configFile, configLine) && configLine != "}")
    {
        if (configLine.empty() || configLine[0] == ';' || configLine[0] == '#')
        {
            continue; // Skip empty lines and comments
        }

        std::string name = configLine;
        std::getline(configFile, configLine); // Skip the opening brace

        std::string port, gearRatio, reversedStr;
        while (std::getline(configFile, configLine) && configLine != "}")
        {
            std::istringstream iss(configLine);
            std::string configKey, configValue;
            if (std::getline(iss, configKey, '=') && std::getline(iss, configValue))
            {
                if (configKey == "PORT")
                {
                    port = configValue;
                }
                else if (configKey == "GEAR_RATIO")
                {
                    gearRatio = configValue;
                }
                else if (configKey == "REVERSED")
                {
                    reversedStr = configValue;
                }
            }
        }

        if (section == "MOTOR_CONFIG")
        {
            motorPorts[name] = std::stoi(port);
            motorGearRatios[name] = gearRatio;
            motorReversed[name] = stringToBool(reversedStr);
        }
        else if (section == "INERTIAL")
        {
            inertialPorts[name] = std::stoi(port);
        }
        else if (section == "TRIPORT_CONFIG")
        {
            triPorts[name] = getTriPort(port);
        }
    }
}

// Method to parse the config file
void configManager::parseConfig()
{
    std::string message = "Version: " + Version + " | Build date: " + BuildDate;
    logHandler("main", message, Log::Level::Info);
    primaryController.Screen.print("Starting up...");

    if (Brain.SDcard.isInserted())
    {
        if (Brain.SDcard.exists(configFileName.c_str()))
        {
            setValuesFromConfig();
        }
        else
        {
            resetOrInitializeConfig("Missing config file. Create it?");
            setValuesFromConfig();
        }
    }
    else
    {
        // Default values
        POLLINGRATE = 1;
        PRINTLOGO = false;
        maxOptionSize = 4;
        CTRLR1POLLINGRATE = 25;
        logHandler("configParser", "No SD card installed. Using default values.", Log::Level::Info);
    }
    calibrateGyro();
    gifplayer();
    Drivetrain.setStopping(vex::brakeType::coast);
}