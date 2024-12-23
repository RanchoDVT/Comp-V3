#include <map>
#include <string>

class configManager
{
public:
    configManager(const std::string &configFileName, const std::string &maintenanceFileName);
    void resetOrInitializeConfig(std::string_view message);
    bool stringToBool(std::string_view str);
    long stringToLong(std::string_view str);
    void setValuesFromConfig();
    void parseConfig();
    void parseComplexConfig(std::ifstream &configFile, const std::string &section);

    enum class ConfigType
    {
        Brain,
        Controller
    };

    ConfigType configType;

    enum class DriveMode
    {
        LeftArcade,
        RightArcade,
        SplitArcade,
        Tank
    };

    DriveMode getDriveMode() const;
    void setDriveMode(const DriveMode &mode);

    std::string teamNumber;
    std::string loadingGifPath;
    std::string autoGifPath;
    std::string driverGifPath;
    std::string customMessage;

    std::size_t getMaxOptionSize() const { return maxOptionSize; }
    bool getLogToFile() const { return logToFile; }
    std::size_t getPollingRate() const { return POLLINGRATE; }
    bool getPrintLogo() const { return PRINTLOGO; }
    std::size_t getCtrlr1PollingRate() const { return CTRLR1POLLINGRATE; }
    Log::Level getLogLevel() const { return logLevel; }

    void setMaxOptionSize(const std::size_t &value);
    void setLogToFile(const bool &value);
    void setPollingRate(const std::size_t &value);
    void setPrintLogo(const bool &value);
    void setCtrlr1PollingRate(const std::size_t &value);
    void setLogLevel(const Log::Level &value);

    void setTeamNumber(const std::string &value);
    void setLoadingGifPath(const std::string &value);
    void setAutoGifPath(const std::string &value);
    void setDriverGifPath(const std::string &value);
    void setCustomMessage(const std::string &value);

    std::string getGearRatio(const std::string &motorName) const;
    bool getMotorReversed(const std::string &motorName) const;
    vex::gearSetting getGearSetting(const std::string &ratio) const;
    int getMotorPort(const std::string &motorName) const;
    vex::triport::port *getTriPort(const std::string &portName) const;

    std::string getTeamNumber() const;
    int getOdometer() const { return odometer; }
    int getLastService() const { return lastService; }
    int getServiceInterval() const { return serviceInterval; }
    void updateOdometer(const int &averagePosition);
    void checkServiceInterval();

    ConfigType stringToConfigType(const std::string &str);
    Log::Level stringToLogLevel(const std::string &str);

private:
    std::map<std::string, int> motorPorts;
    std::map<std::string, std::string> motorGearRatios;
    std::map<std::string, bool> motorReversed;
    std::map<std::string, vex::triport::port *> triPorts;
    std::map<std::string, int> inertialPorts;

    std::string configFileName;
    std::string maintenanceFileName;
    std::size_t maxOptionSize;
    bool logToFile;
    std::size_t POLLINGRATE;
    bool PRINTLOGO;
    std::size_t CTRLR1POLLINGRATE;
    Log::Level logLevel;

    int odometer;
    int lastService;
    int serviceInterval;
    DriveMode driveMode;

    void readMaintenanceData();
    void writeMaintenanceData();
};