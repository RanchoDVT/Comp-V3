#include <map>

class configManager
{
public:
    configManager(const std::string &configFileName, const std::string &maintenanceFileName);
    void resetOrInitializeConfig(const std::string &message);
    bool stringToBool(const std::string &str);
    long stringToLong(const std::string &str);
    void setValuesFromConfig();
    void parseConfig();

    enum class ConfigType
    {
        Brain,
        Controller
    };

    ConfigType configType;

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

    void setMaxOptionSize(const size_t &value);
    void setLogToFile(const bool &value);
    void setPollingRate(const std::size_t &value);
    void setPrintLogo(const bool &value);
    void setCtrlr1PollingRate(const std::size_t &value);

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

private:
    std::map<std::string, int> motorPorts;
    std::map<std::string, std::string> motorGearRatios;
    std::map<std::string, bool> motorReversed;
    std::map<std::string, vex::triport::port *> triPorts;

    std::string configFileName;
    std::string maintenanceFileName;
    std::size_t maxOptionSize;
    bool logToFile;
    std::size_t POLLINGRATE;
    bool PRINTLOGO;
    std::size_t CTRLR1POLLINGRATE;

    int odometer;
    int lastService;
    int serviceInterval;

    void readMaintenanceData();
    void writeMaintenanceData();
};