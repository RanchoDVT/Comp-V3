#include "vex.h"

vex::brain Brain;

int frontLeftPort = ConfigManager.getMotorPort("FRONT_LEFT_MOTOR");
int rearLeftPort = ConfigManager.getMotorPort("REAR_LEFT_MOTOR");
int frontRightPort = ConfigManager.getMotorPort("FRONT_RIGHT_MOTOR");
int rearRightPort = ConfigManager.getMotorPort("REAR_RIGHT_MOTOR");

vex::gearSetting frontLeftGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("FRONT_LEFT_MOTOR"));
vex::gearSetting rearLeftGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("REAR_LEFT_MOTOR"));
vex::gearSetting frontRightGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("FRONT_RIGHT_MOTOR"));
vex::gearSetting rearRightGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("REAR_RIGHT_MOTOR"));

bool frontLeftReversed = ConfigManager.getMotorReversed("FRONT_LEFT_MOTOR");
bool rearLeftReversed = ConfigManager.getMotorReversed("REAR_LEFT_MOTOR");
bool frontRightReversed = ConfigManager.getMotorReversed("FRONT_RIGHT_MOTOR");
bool rearRightReversed = ConfigManager.getMotorReversed("REAR_RIGHT_MOTOR");

vex::triport::port *bumperTriPort = ConfigManager.getTriPort("REAR_BUMPER"); // Thanks vex for making this with pointers! (sarcasm)

vex::motor frontLeftMotor = vex::motor(frontLeftPort, frontLeftGearRatio, frontLeftReversed);
vex::motor rearLeftMotor = vex::motor(rearLeftPort, rearLeftGearRatio, rearLeftReversed);
vex::motor_group LeftDriveSmart = vex::motor_group(frontLeftMotor, rearLeftMotor);

vex::motor frontRightMotor = vex::motor(frontRightPort, frontRightGearRatio, frontRightReversed);
vex::motor rearRightMotor = vex::motor(rearRightPort, rearRightGearRatio, rearRightReversed);
vex::motor_group RightDriveSmart = vex::motor_group(frontRightMotor, rearRightMotor);

vex::inertial InertialGyro = vex::inertial(vex::PORT3);
vex::smartdrive Drivetrain = vex::smartdrive(LeftDriveSmart, RightDriveSmart, InertialGyro, 319.19, 320, 165, vex::distanceUnits::mm, 1);

vex::controller primaryController = vex::controller(vex::controllerType::primary);
vex::controller partnerController = vex::controller(vex::controllerType::partner);

vex::bumper RearBumper = vex::bumper(*bumperTriPort);

vex::competition Competition;

void applyTheme() {
    std::string teamNumber = ConfigManager.getConfigValue("TEAM_NUMBER");
    std::string colorName = ConfigManager.getConfigValue("COLOR");
    vex::color themeColor = vex::color::white;

    // Parse color
    if (colorName == "blue") themeColor = vex::color::blue;
    else if (colorName == "red") themeColor = vex::color::red;
    else if (colorName == "green") themeColor = vex::color::green;

    // Apply to Brain screen
    Brain.Screen.clearScreen(themeColor);
    Brain.Screen.setPenColor(vex::color::white);
    Brain.Screen.setFont(vex::fontType::mono20);
    Brain.Screen.printAt(200, 20, "Team: %s", teamNumber.c_str());
}

std::string processMessage(const std::string& message) {
    std::string processedMessage = message;
    processedMessage = replacePlaceholder(processedMessage, "{USER_NAME}", "Driver");
    processedMessage = replacePlaceholder(processedMessage, "{SERVICE_INTERVAL}", std::to_string(ConfigManager.getServiceInterval()));
    return processedMessage;
}

void displayMessages() {
    std::ifstream file("messages.txt");
    std::string line;
    while (std::getline(file, line)) {
        Brain.Screen.print(processMessage(line).c_str());
    }
}


/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexCodeInit(void)
{
    clearScreen(true, true, true);

    std::ostringstream message;

    primaryController.Screen.print("Starting up...");
    logHandler("startup", "Starting GUI startup...", Log::Level::Info);

    if (Competition.isEnabled())
    {
        logHandler("startup", "Robot is IN Competition mode!", Log::Level::Fatal);
    }

    ConfigManager.checkServiceInterval();

    vex::competition::bStopAllTasksBetweenModes = false;

    message << "Battery is at: " << Brain.Battery.capacity() << "%%";
    if (Brain.Battery.capacity() < 90)
    {
        logHandler("startup", message.str(), Log::Level::Warn, 3);
    }
    else
    {
        logHandler("startup", message.str(), Log::Level::Info, 3);
    }
    message.str(std::string());

    std::string autorun = getUserOption("Run Autonomous?", {"Yes", "No"});
    if (autorun == "Yes")
    {
        logHandler("startup", "Starting autonomous from setup.", Log::Level::Trace);
        primaryController.Screen.print("Running autonomous.");
        autonomous();
        logHandler("startup", "Finished autonomous.", Log::Level::Trace);
    }
    else if (autorun == "No")
    {
        primaryController.Screen.print("Skipped autonomous.");
        logHandler("startup", "Skipped autonomous.", Log::Level::Trace);
        vex::this_thread::sleep_for(1000);
    }
    clearScreen(false, true, true);
    return;
}