#include "vex.h"

void autonomous()
{
    logHandler("autonomous", "Test message.", Log::Level::Warn, 2);
    return;
}

void collision(const vex::axisType axis, const double x, const double y, const double z)
{
    printf("collision %d %6.2f %6.2f %6.2f\n", (int)axis, x, y, z);
}

// Drive system flags
bool tractionControlEnabled = true;
bool stabilityControlEnabled = true;
bool absEnabled = true;

/**
 * @author @DVT7125
 * @date 4/10/24
 * @brief User control task.
 * @return 0
 */

// Function to display system states on the Brain Screen
void displaySystemStates()
{
    Brain.Screen.clearScreen();
    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Traction Control: %s\nStability Control: %s\nABS: %s",
                       tractionControlEnabled ? "ON" : "OFF",
                       stabilityControlEnabled ? "ON" : "OFF",
                       absEnabled ? "ON" : "OFF");
}

// Function to apply traction control
void applyTractionControl(double &forwardVolts)
{
    double frontLeftMotorRPM = frontLeftMotor.velocity(vex::velocityUnits::rpm);
    double rearLeftMotorRPM = rearLeftMotor.velocity(vex::velocityUnits::rpm);
    double frontRightMotorRPM = frontRightMotor.velocity(vex::velocityUnits::rpm);
    double rearRightMotorRPM = rearRightMotor.velocity(vex::velocityUnits::rpm);

    double minSpeed = std::min({std::abs(frontLeftMotorRPM), std::abs(rearLeftMotorRPM), std::abs(frontRightMotorRPM), std::abs(rearRightMotorRPM)});

    forwardVolts = minSpeed;
}

// Function to apply stability control
void applyStabilityControl(const double &forwardVolts)
{
    double leftRPM = LeftDriveSmart.velocity(vex::velocityUnits::rpm);
    double rightRPM = RightDriveSmart.velocity(vex::velocityUnits::rpm);

    double minSpeed = std::min(std::abs(leftRPM), std::abs(rightRPM));

    LeftDriveSmart.spin(vex::directionType::fwd, minSpeed, vex::voltageUnits::volt);
    RightDriveSmart.spin(vex::directionType::fwd, minSpeed, vex::voltageUnits::volt);
}

// Function to apply ABS
void applyABS(double &brakeVolts)
{
    double frontLeftMotorRPM = frontLeftMotor.velocity(vex::velocityUnits::rpm);
    double rearLeftMotorRPM = rearLeftMotor.velocity(vex::velocityUnits::rpm);
    double frontRightMotorRPM = frontRightMotor.velocity(vex::velocityUnits::rpm);
    double rearRightMotorRPM = rearRightMotor.velocity(vex::velocityUnits::rpm);

    double minSpeed = std::min({std::abs(frontLeftMotorRPM), std::abs(rearLeftMotorRPM), std::abs(frontRightMotorRPM), std::abs(rearRightMotorRPM)});

    brakeVolts = minSpeed;
}

// Function to display drive mode menu
void displayDriveModeMenu()
{
    primaryController.Screen.clearScreen();
    primaryController.Screen.setCursor(1, 1);
    getUserOption("Drive Mode", {"Left Arcade", "Right Arcade", "Split Arcade", "Tank"});

    auto buttonPressDurations = controllerButtonsPressed(primaryController);
    std::string buttonPressed;

    if (buttonPressed == "A")
    {
        ConfigManager.setDriveMode(configManager::DriveMode::LeftArcade);
    }
    else if (buttonPressed == "B")
    {
        ConfigManager.setDriveMode(configManager::DriveMode::RightArcade);
    }
    else if (buttonPressed == "X")
    {
        ConfigManager.setDriveMode(configManager::DriveMode::SplitArcade);
    }
    else if (buttonPressed == "Y")
    {
        ConfigManager.setDriveMode(configManager::DriveMode::Tank);
    }

    primaryController.Screen.clearScreen();
    primaryController.Screen.setCursor(1, 1);
    primaryController.Screen.print("Drive Mode Selected");
}

// User control task
void userControl()
{
    if (!Competition.isEnabled())
    {
        logHandler("drivetrain_main", "Ctrl1 is NOT in command mode!", Log::Level::Fatal);
    }

    vex::thread motortemp(motorMonitor);
    InertialGyro.collision(collision);

    double turnVolts, forwardVolts;

    bool configMenuActive = false; // Tracks if options menu is active

    // Load drive mode from config
    configManager::DriveMode currentDriveMode = ConfigManager.getDriveMode();

    while (Competition.isEnabled())
    {
        // Open configuration menu
        if (primaryController.ButtonUp.pressing())
        {
            configMenuActive = !configMenuActive;
            if (configMenuActive)
            {
                displayDriveModeMenu();
                currentDriveMode = ConfigManager.getDriveMode(); // Update currentDriveMode after selection
            }
        }

        switch (currentDriveMode)
        {
        case configManager::DriveMode::LeftArcade:
            turnVolts = primaryController.Axis4.position() * 0.12; // -12 to 12
            forwardVolts = primaryController.Axis3.position() * 0.12;
            break;

        case configManager::DriveMode::RightArcade:
            turnVolts = primaryController.Axis1.position() * 0.12; // -12 to 12
            forwardVolts = primaryController.Axis2.position() * 0.12;
            break;

        case configManager::DriveMode::SplitArcade:
            turnVolts = primaryController.Axis1.position() * 0.12; // -12 to 12
            forwardVolts = primaryController.Axis3.position() * 0.12;
            break;

        case configManager::DriveMode::Tank:
            double leftVolts = primaryController.Axis3.position() * 0.12;  // -12 to 12
            double rightVolts = primaryController.Axis2.position() * 0.12; // -12 to 12
            LeftDriveSmart.spin(vex::directionType::fwd, leftVolts, vex::voltageUnits::volt);
            RightDriveSmart.spin(vex::directionType::fwd, rightVolts, vex::voltageUnits::volt);
            break;
        }
        if (currentDriveMode != configManager::DriveMode::Tank)
        {
            // Apply traction control if enabled
            if (tractionControlEnabled)
            {
                applyTractionControl(forwardVolts);
            }

            // Apply stability control if enabled
            if (stabilityControlEnabled)
            {
                applyStabilityControl(forwardVolts);
            }

            // Apply ABS if enabled
            if (absEnabled)
            {
                applyABS(forwardVolts);
            }

            // Apply the calculated voltages to the motors
            LeftDriveSmart.spin(vex::directionType::fwd, forwardVolts + turnVolts, vex::voltageUnits::volt);
            RightDriveSmart.spin(vex::directionType::fwd, forwardVolts - turnVolts, vex::voltageUnits::volt);
        }
        vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
    }
}