#include "vex.h"

#include <fstream>

// Method to reset or initialize the config file
void configManager::resetOrInitializeConfig(std::string_view message)
{
    maxOptionSize = 4; // Example, already validated in setMaxOptionSize
    std::string resetcfg = getUserOption(std::string(message), {"Yes", "No"});
    if (resetcfg == "Yes")
    {
        primaryController.Screen.print("Resetting config file...");
        std::ofstream configFile(configFileName);
        if (!configFile)
        {
            logHandler("resetOrInitializeConfig", "Could not create config.", Log::Level::Warn, 3);
            return;
        }
        // Write default configuration
        configFile << "# Config File:\n";
        configFile << "MOTOR_CONFIG {\n";
        configFile << "    FRONT_LEFT_MOTOR {\n";
        configFile << "        PORT=1\n";
        configFile << "        GEAR_RATIO=6_1\n";
        configFile << "        REVERSED=false\n";
        configFile << "    }\n";
        configFile << "    FRONT_RIGHT_MOTOR {\n";
        configFile << "        PORT=10\n";
        configFile << "        GEAR_RATIO=6_1\n";
        configFile << "        REVERSED=true\n";
        configFile << "    }\n";
        configFile << "    REAR_LEFT_MOTOR {\n";
        configFile << "        PORT=11\n";
        configFile << "        GEAR_RATIO=6_1\n";
        configFile << "        REVERSED=false\n";
        configFile << "    }\n";
        configFile << "    REAR_RIGHT_MOTOR {\n";
        configFile << "        PORT=20\n";
        configFile << "        GEAR_RATIO=6_1\n";
        configFile << "        REVERSED=true\n";
        configFile << "    }\n";
        configFile << "    INERTIAL {\n";
        configFile << "        PORT=3\n";
        configFile << "    }\n";
        configFile << "    Rear_Bumper {\n";
        configFile << "        PORT=A\n";
        configFile << "    }\n";
        configFile << "}\n";
        configFile << "PRINTLOGO=true\n";
        configFile << "LOGTOFILE=true\n";
        configFile << "MAXOPTIONSSIZE=4\n";
        configFile << "POLLINGRATE=5\n";
        configFile << "CTRLR1POLLINGRATE=25\n";
        configFile << "CONFIGTYPE=Brain\n";
        configFile << "TEAMNUMBER=12\n";
        configFile << "LOADINGGIFPATH=loading.gif\n";
        configFile << "AUTOGIFPATH=auto.gif\n";
        configFile << "DRIVEGIFPATH=drive.gif\n";
        configFile << "CUSTOMMESSAGE=test\n";
        configFile << "DRIVEMODE=Split\n";
        configFile << std::format("VERSION={}\n", Version);
        configFile.close();

        logHandler("resetConfig", "Successfully reset config file.", Log::Level::Debug);
    }
}

void configManager::writeMaintenanceData()
{
    std::ofstream maintenanceFile(maintenanceFileName);
    if (maintenanceFile.is_open())
    {
        maintenanceFile << "ODOMETER=" << odometer << "\n";
        maintenanceFile << "LAST_SERVICE=" << lastService << "\n";
        maintenanceFile << "SERVICE_INTERVAL=" << serviceInterval << "\n";
        maintenanceFile.close();
    }
}

void configManager::setDriveMode(const configManager::DriveMode &mode)
{
    driveMode = mode; // Update in-memory

    // Also write to config file
    std::ofstream configFile(configFileName, std::ios::app);
    if (configFile.is_open())
    {
        configFile << "DRIVEMODE=";
        switch (mode)
        {
        case DriveMode::LeftArcade:
            configFile << "LeftArcade";
            break;
        case DriveMode::RightArcade:
            configFile << "RightArcade";
            break;
        case DriveMode::SplitArcade:
            configFile << "SplitArcade";
            break;
        case DriveMode::Tank:
            configFile << "Tank";
            break;
        }
        configFile << "\n";
        configFile.close();
    }
}

void configManager::readMaintenanceData()
{
    std::ifstream maintenanceFile(maintenanceFileName);
    if (maintenanceFile.is_open())
    {
        std::string line;
        while (std::getline(maintenanceFile, line))
        {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value))
            {
                if (key == "ODOMETER")
                {
                    odometer = stringToNumber<int>(value);
                }
                else if (key == "LAST_SERVICE")
                {
                    lastService = stringToNumber<long>(value);
                }
                else if (key == "SERVICE_INTERVAL")
                {
                    serviceInterval = stringToNumber<long>(value);
                }
            }
        }
        maintenanceFile.close();
    }
}

// Method to convert a string to boolean
bool configManager::stringToBool(std::string_view str)
{
    if (str.empty())
    {
        logHandler("stringToBool", "Empty string cannot be converted to boolean", Log::Level::Error);
        return false;
    }

    if (str == "true" || str == "1")
    {
        return true;
    }
    else if (str == "false" || str == "0")
    {
        return false;
    }
    else
    {
        logHandler("stringToBool", std::format("Invalid boolean string: {}", str), Log::Level::Error);
        return false;
    }
}

// Method to convert a string to a number
template <typename T>
T configManager::stringToNumber(std::string_view str)
{
    if (str.empty())
    {
        logHandler("stringToNumber", "Empty string cannot be converted to number", Log::Level::Error);
        return T{};
    }

    try
    {
        if constexpr (std::is_same_v<T, int>)
        {
            return std::stoi(std::string(str));
        }
        else if constexpr (std::is_same_v<T, long>)
        {
            return std::stol(std::string(str));
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return std::stod(std::string(str));
        }
        else if constexpr (std::is_same_v<T, std::size_t>)
        {
            return static_cast<std::size_t>(std::stoul(std::string(str)));
        }
    }
    catch (const std::invalid_argument &e)
    {
        logHandler("stringToNumber", std::format("Invalid argument: {}", str), Log::Level::Error);
    }
    catch (const std::out_of_range &e)
    {
        logHandler("stringToNumber", std::format("Out of range: {}", str), Log::Level::Error);
    }

    return T{};
}

// Method to set values from the config file and parse complex config sections
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
                setMaxOptionSize(stringToNumber<std::size_t>(value));
            }
            else if (key == "POLLINGRATE")
            {
                setPollingRate(stringToNumber<std::size_t>(value));
            }
            else if (key == "CTRLR1POLLINGRATE")
            {
                setCtrlr1PollingRate(stringToNumber<std::size_t>(value));
            }
            else if (key == "LOGLEVEL")
            {
                setLogLevel(stringToLogLevel(value));
            }
            else if (key == "DRIVEMODE")
            {
                if (value == "Arcade")
                    setDriveMode(DriveMode::LeftArcade);
                else if (value == "SplitArcade")
                    setDriveMode(DriveMode::RightArcade);
                else if (value == "Tank")
                    setDriveMode(DriveMode::SplitArcade);
                else if (value == "Custom")
                    setDriveMode(DriveMode::Tank);
            }
            else if (key == "VERSION")
            {
                if (value != Version)
                {
                    resetOrInitializeConfig(std::format("Version mismatch with Config file ({}) and code version ({}). Do you want to reset the config file?", value, Version));
                }
            }
            else
            {
                resetOrInitializeConfig(std::format("Unknown key in config file: {}. Do you want to reset the config?", key));
            }
        }
        else if (key == "MOTOR_CONFIG" || key == "INERTIAL" || key == "TRIPORT_CONFIG")
        {
            std::string section = key;
            while (std::getline(configFile, configLine) && configLine != "}")
            {
                if (configLine.empty() || configLine.starts_with(';') || configLine.starts_with('#'))
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
                    motorPorts[name] = stringToNumber<int>(port);
                    motorGearRatios[name] = gearRatio;
                    motorReversed[name] = stringToBool(reversedStr);
                }
                else if (section == "INERTIAL")
                {
                    inertialPorts[name] = stringToNumber<int>(port);
                }
                else if (section == "TRIPORT_CONFIG")
                {
                    triPorts[name] = getTriPort(port);
                }
            }
        }
        else
        {
            resetOrInitializeConfig(std::format("Invalid line in config file: {}. Do you want to reset the config?", configLine));
        }
    }
    configFile.close();
}

// Method to parse the config file
void configManager::parseConfig()
{
    std::string message = std::format("Version: {} | Build date: {}", Version, BuildDate);
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
        PRINTLOGO = true;
        logToFile = true;
        maxOptionSize = 4;
        CTRLR1POLLINGRATE = 25;
        logLevel = Log::Level::Info;
        driveMode = DriveMode::SplitArcade;
        configType = ConfigType::Brain;
        odometer = 0;
        lastService = 0;
        serviceInterval = 1000;
        logHandler("configParser", "No SD card installed. Using default values.", Log::Level::Info);
    }
    calibrateGyro();
    gifplayer();
    Drivetrain.setStopping(vex::brakeType::coast);
}