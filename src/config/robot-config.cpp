#include "vex.h"

vex::brain Brain;

auto frontLeftPort = ConfigManager.getMotorPort("FRONT_LEFT_MOTOR");
auto rearLeftPort = ConfigManager.getMotorPort("REAR_LEFT_MOTOR");
auto frontRightPort = ConfigManager.getMotorPort("FRONT_RIGHT_MOTOR");
auto rearRightPort = ConfigManager.getMotorPort("REAR_RIGHT_MOTOR");

auto frontLeftGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("FRONT_LEFT_MOTOR"));
auto rearLeftGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("REAR_LEFT_MOTOR"));
auto frontRightGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("FRONT_RIGHT_MOTOR"));
auto rearRightGearRatio = ConfigManager.getGearSetting(ConfigManager.getGearRatio("REAR_RIGHT_MOTOR"));

auto frontLeftReversed = ConfigManager.getMotorReversed("FRONT_LEFT_MOTOR");
auto rearLeftReversed = ConfigManager.getMotorReversed("REAR_LEFT_MOTOR");
auto frontRightReversed = ConfigManager.getMotorReversed("FRONT_RIGHT_MOTOR");
auto rearRightReversed = ConfigManager.getMotorReversed("REAR_RIGHT_MOTOR");

auto bumperTriPort = ConfigManager.getTriPort("REAR_BUMPER"); // Thanks vex for making this with pointers! (sarcasm)

vex::motor frontLeftMotor{frontLeftPort, frontLeftGearRatio, frontLeftReversed};
vex::motor rearLeftMotor{rearLeftPort, rearLeftGearRatio, rearLeftReversed};
vex::motor_group LeftDriveSmart{frontLeftMotor, rearLeftMotor};

vex::motor frontRightMotor{frontRightPort, frontRightGearRatio, frontRightReversed};
vex::motor rearRightMotor{rearRightPort, rearRightGearRatio, rearRightReversed};
vex::motor_group RightDriveSmart{frontRightMotor, rearRightMotor};

vex::inertial InertialGyro{vex::PORT3};
vex::smartdrive Drivetrain{LeftDriveSmart, RightDriveSmart, InertialGyro, 319.19, 320, 165, vex::distanceUnits::mm, 1};

vex::controller primaryController{vex::controllerType::primary};
vex::controller partnerController{vex::controllerType::partner};

vex::bumper RearBumper{*bumperTriPort};

vex::competition Competition;

/**
 * Check if the Y button is held at startup to enter diagnostic mode.
 */
bool isDiagnosticMode()
{
    // Wait for a short period to allow the user to press the button
    vex::this_thread::sleep_for(1000);
    return primaryController.ButtonY.pressing();
}

/**
 * Initialize diagnostic mode.
 */
void initializeDiagnosticMode()
{
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Entering Diagnostic Mode...");

    // Add any additional diagnostic initialization here

    // Run the display task for diagnostic mode
    displayTask();
}

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 *
 * This should be called at the start of your int main function.
 */
void vexCodeInit()
{
    clearScreen(true, true, true);

    primaryController.Screen.print("Starting up...");
    logHandler("startup", "Starting GUI startup...", Log::Level::Info);

    if (Competition.isEnabled())
    {
        logHandler("startup", "Robot is IN Competition mode!", Log::Level::Fatal);
    }

    ConfigManager.checkServiceInterval();

    vex::competition::bStopAllTasksBetweenModes = false;

    if (isDiagnosticMode())
    {
        initializeDiagnosticMode();
        return;
    }

    if (ConfigManager.configType == configManager::ConfigType::Brain)
    {
        // Display team number at the top right
        auto teamNumber = ConfigManager.getTeamNumber();
        Brain.Screen.setCursor(1, 20);
        Brain.Screen.print("Team #%s", teamNumber.c_str());

        // Start the GIF player
        gifplayer();

        // Add the autonomous prompt
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(3, 5);
        Brain.Screen.print("Run Autonomous?");
        Brain.Screen.setCursor(5, 5);
        Brain.Screen.print("Yes");

        Brain.Screen.setCursor(5, 15);
        Brain.Screen.print("No");

        bool optionSelected = false;
        bool runAutonomous = false;

        while (!optionSelected)
        {
            if (Brain.Screen.pressing())
            {
                auto x = Brain.Screen.xPosition();
                auto y = Brain.Screen.yPosition();

                // Check if 'Yes' button is clicked
                if (x > 40 && x < 100 && y > 100 && y < 140)
                { // Adjust positions based on screen layout
                    logHandler("startup", "Starting autonomous from Brain screen.", Log::Level::Trace);
                    Brain.Screen.clearScreen();
                    Brain.Screen.setCursor(3, 5);
                    Brain.Screen.print("Running Autonomous...");
                    runAutonomous = true;
                    optionSelected = true;
                }
                // Check if 'No' button is clicked
                else if (x > 140 && x < 200 && y > 100 && y < 140)
                { // Adjust positions
                    logHandler("startup", "Skipped autonomous from Brain screen.", Log::Level::Trace);
                    Brain.Screen.clearScreen();
                    Brain.Screen.setCursor(3, 5);
                    Brain.Screen.print("Skipped Autonomous.");
                    runAutonomous = false;
                    optionSelected = true;
                }
            }
            vex::this_thread::sleep_for(50); // Avoid high CPU usage
        }

        if (runAutonomous)
        {
            autonomous();
            logHandler("startup", "Finished autonomous from Brain screen.", Log::Level::Trace);
        }
        vex::this_thread::sleep_for(1000);
    }
    else if (ConfigManager.configType == configManager::ConfigType::Controller)
    {
        auto message = "Battery is at: " + std::to_string(Brain.Battery.capacity()) + "%%";
        if (Brain.Battery.capacity() < 90)
        {
            logHandler("startup", message, Log::Level::Warn, 3);
        }
        else
        {
            logHandler("startup", message, Log::Level::Info, 3);
        }

        auto autorun = getUserOption("Run Autonomous?", {"Yes", "No"});
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
    }

    clearScreen(false, true, true);
    return;
}