#include "vex.h"

#include <fstream>
#include <sstream>

configManager ConfigManager("config.cfg", "maintenance.txt");

// Constructor
configManager::configManager(const std::string &configFileName, const std::string &maintenanceFileName)
    : configFileName(configFileName), maintenanceFileName(maintenanceFileName), maxOptionSize(4), logToFile(true),
      POLLINGRATE(5), PRINTLOGO(true), CTRLR1POLLINGRATE(25), odometer(0), lastService(0), serviceInterval(1000)
{
    readMaintenanceData();

    // Initialize triPorts with pointers to Brain.ThreeWirePort
    triPorts["A"] = &Brain.ThreeWirePort.A;
    triPorts["B"] = &Brain.ThreeWirePort.B;
    triPorts["C"] = &Brain.ThreeWirePort.C;
    triPorts["D"] = &Brain.ThreeWirePort.D;
    triPorts["E"] = &Brain.ThreeWirePort.E;
    triPorts["F"] = &Brain.ThreeWirePort.F;
    triPorts["G"] = &Brain.ThreeWirePort.G;
    triPorts["H"] = &Brain.ThreeWirePort.H;
}

vex::triport::port *configManager::getTriPort(const std::string &portName) const
{
    auto it = triPorts.find(portName);
    return (it != triPorts.end()) ? it->second : &Brain.ThreeWirePort.A; // Default to A if not found
}

// Setters with validation
void configManager::setMaxOptionSize(const size_t &value)
{
    std::ostringstream oss;
    oss << value;
    maxOptionSize = stringToLong(oss.str());
}

void configManager::setLogToFile(const bool &value)
{
    logToFile = (value);
}

void configManager::setPollingRate(const size_t &value)
{
    std::ostringstream oss;
    oss << value;
    POLLINGRATE = stringToLong(oss.str());
}

void configManager::setPrintLogo(const bool &value)
{
    PRINTLOGO = value;
}

void configManager::setCtrlr1PollingRate(const std::size_t &value)
{
    std::ostringstream oss;
    oss << value;
    logToFile = stringToLong(oss.str());
}

int configManager::getMotorPort(const std::string &motorName) const
{
    auto it = motorPorts.find(motorName);
    return (it != motorPorts.end()) ? it->second : -1; // Default invalid port
}

std::string configManager::getGearRatio(const std::string &motorName) const
{
    auto it = motorGearRatios.find(motorName);
    return (it != motorGearRatios.end()) ? it->second : "18_1"; // Default ratio
}

bool configManager::getMotorReversed(const std::string &motorName) const
{
    auto it = motorReversed.find(motorName);
    return (it != motorReversed.end()) ? it->second : false; // Default reversed state
}

vex::gearSetting configManager::getGearSetting(const std::string &ratio) const
{
    if (ratio == "6_1")
    {
        return vex::gearSetting::ratio6_1;
    }
    else if (ratio == "18_1")
    {
        return vex::gearSetting::ratio18_1;
    }
    else if (ratio == "36_1")
    {
        return vex::gearSetting::ratio36_1;
    }
    else
    {
        return vex::gearSetting::ratio18_1; // Default
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
                    odometer = stringToLong(value);
                }
                else if (key == "LAST_SERVICE")
                {
                    lastService = stringToLong(value);
                }
                else if (key == "SERVICE_INTERVAL")
                {
                    serviceInterval = stringToLong(value);
                }
            }
        }
        maintenanceFile.close();
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

void configManager::updateOdometer(const int &averagePosition)
{
    odometer += averagePosition;
    writeMaintenanceData();
}

void configManager::checkServiceInterval()
{
    if (odometer - lastService >= serviceInterval)
    {
        std::ostringstream message;
        message << "Service needed! Distance: " << odometer;
        logHandler("Service", message.str(), Log::Level::Warn, 5);
    }
}

configManager::ConfigType configManager::stringToConfigType(const std::string &str)
{
    if (str == "Brain")
    {
        return ConfigType::Brain;
    }
    else if (str == "Controller")
    {
        return ConfigType::Controller;
    }
    else
    {
        throw std::invalid_argument("Invalid config type");
    }
}

std::string configManager::getTeamNumber() const
{
    return teamNumber;
}