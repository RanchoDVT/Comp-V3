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
    maxOptionSize = stringToLong(std::to_string(value));
}

void configManager::setLogToFile(const bool &value)
{
    logToFile = (value);
}

void configManager::setPollingRate(const size_t &value)
{
    POLLINGRATE = stringToLong(std::to_string(value));
}

void configManager::setPrintLogo(const bool &value)
{
    PRINTLOGO = value;
}

void configManager::setCtrlr1PollingRate(const std::size_t &value)
{
    logToFile = stringToLong(std::to_string(value));;
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

std::string generateKey() {
    return "KEY" + Brain.Battery.capacity().str(); // Simple example; replace with secure derivation
}

void encryptAndSave(const std::string& data, const std::string& fileName) {
    std::string key = generateKey();
    std::string encryptedData = mine::encrypt(data, key);
    std::ofstream file(fileName);
    if (file.is_open()) {
        file << encryptedData;
        file.close();
    }
}

std::string decryptFromFile(const std::string& fileName) {
    std::ifstream file(fileName);
    std::string encryptedData, decryptedData;
    if (file.is_open()) {
        std::getline(file, encryptedData);
        file.close();
    }
    std::string key = generateKey();
    return mine::decrypt(encryptedData, key);
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