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
    Brain.Screen.print("Traction Control: %s", tractionControlEnabled ? "ON" : "OFF");
    Brain.Screen.newLine();
    Brain.Screen.print("Stability Control: %s", stabilityControlEnabled ? "ON" : "OFF");
    Brain.Screen.newLine();
    Brain.Screen.print("ABS: %s", absEnabled ? "ON" : "OFF");
}

// Function to apply stability control based on wheel RPM
void applyStabilityControl(const double &forwardVolts)
{
    // Get the current RPM of the left and right motors
    double leftRPM = LeftDriveSmart.velocity(vex::velocityUnits::rpm);
    double rightRPM = RightDriveSmart.velocity(vex::velocityUnits::rpm);

    // Compare the RPMs, check for oversteer or understeer
    double rpmDifference = std::abs(leftRPM - rightRPM);

    // If the difference is too high, apply stability control
    if (rpmDifference > 20)
    { // Threshold for detecting instability
        // Find the slowest motor speed
        double minSpeed = std::min(std::abs(leftRPM), std::abs(rightRPM));

        // Gradually reduce the motor speed to the slowest motor's speed (instead of stopping abruptly)
        if (leftRPM > rightRPM)
        {
            // Reduce the speed of the left motor to match the right motor's speed (slowest motor)
            LeftDriveSmart.spin(vex::directionType::fwd, minSpeed * 0.7, vex::voltageUnits::volt); // Adjust 0.7 if needed
        }
        else if (rightRPM > leftRPM)
        {
            // Reduce the speed of the right motor to match the left motor's speed (slowest motor)
            RightDriveSmart.spin(vex::directionType::fwd, minSpeed * 0.7, vex::voltageUnits::volt); // Adjust 0.7 if needed
        }

        // Apply the reduced speed to the other motor as well
        LeftDriveSmart.spin(vex::directionType::fwd, minSpeed * 0.7, vex::voltageUnits::volt);
        RightDriveSmart.spin(vex::directionType::fwd, minSpeed * 0.7, vex::voltageUnits::volt);

        // You can also add logic to apply more gradual braking if needed
        // Adjust the speed decrease factor (e.g., *0.7) as necessary to control the braking intensity
    }
}

// Function to apply traction control
void applyTractionControl(double &forwardVolts)
{
    // Get the current RPM of the left and right motors
    double frontLeftMotorRPM = frontLeftMotor.velocity(vex::velocityUnits::rpm);
    double rearLeftMotorRPM = rearLeftMotor.velocity(vex::velocityUnits::rpm);
    double frontRightMotorRPM = frontRightMotor.velocity(vex::velocityUnits::rpm);
    double rearRightMotorRPM = rearRightMotor.velocity(vex::velocityUnits::rpm);

    // Find the minimum RPM among all motors
    double minSpeed = std::min(std::min(std::abs(frontLeftMotorRPM), std::abs(rearLeftMotorRPM)), std::min(std::abs(frontRightMotorRPM), std::abs(rearRightMotorRPM)));

    // Check if any motor's RPM difference exceeds the threshold
    if (std::abs(frontLeftMotorRPM - minSpeed) > 5 || std::abs(rearLeftMotorRPM - minSpeed) > 5 ||
        std::abs(frontRightMotorRPM - minSpeed) > 5 || std::abs(rearRightMotorRPM - minSpeed) > 5)
    {
        // Reduce the forward voltage to match the slowest motor's speed
        forwardVolts = minSpeed * 0.7; // Adjust 0.7 if needed
    }
}

void displayDriveModeMenu()
{
    primaryController.Screen.clearScreen();
    primaryController.Screen.setCursor(1, 1);
    primaryController.Screen.print("Select Drive Mode:");
    primaryController.Screen.setCursor(2, 1);
    primaryController.Screen.print("1. Left Arcade");
    primaryController.Screen.setCursor(3, 1);
    primaryController.Screen.print("2. Right Arcade");
    primaryController.Screen.setCursor(4, 1);
    primaryController.Screen.print("3. Split Arcade");
    primaryController.Screen.setCursor(5, 1);
    primaryController.Screen.print("4. Tank");

    while (true)
    {
        auto buttonPressDurations = controllerButtonsPressed(primaryController);
        std::string buttonPressed;
        if (!buttonPressDurations.empty())
        {
            buttonPressed = buttonPressDurations.begin()->first;
        }
        if (buttonPressed == "1")
        {
            ConfigManager.setDriveMode(configManager::DriveMode::LeftArcade);
            break;
        }
        else if (buttonPressed == "2")
        {
            ConfigManager.setDriveMode(configManager::DriveMode::RightArcade);
            break;
        }
        else if (buttonPressed == "3")
        {
            ConfigManager.setDriveMode(configManager::DriveMode::SplitArcade);
            break;
        }
        else if (buttonPressed == "4")
        {
            ConfigManager.setDriveMode(configManager::DriveMode::Tank);
            break;
        }
        vex::this_thread::sleep_for(100);
    }

    primaryController.Screen.clearScreen();
    primaryController.Screen.print("Drive Mode Selected");
}

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

        // Apply the calculated voltages to the motors
        if (currentDriveMode != configManager::DriveMode::Tank)
        {
            LeftDriveSmart.spin(vex::directionType::fwd, forwardVolts + turnVolts, vex::voltageUnits::volt);
            RightDriveSmart.spin(vex::directionType::fwd, forwardVolts - turnVolts, vex::voltageUnits::volt);
        }

        vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
    }

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

    vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
}