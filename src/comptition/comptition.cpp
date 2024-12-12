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
void applyStabilityControl(double &forwardVolts)
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
    double minSpeed = std::min({std::abs(frontLeftMotorRPM), std::abs(rearLeftMotorRPM), std::abs(frontRightMotorRPM), std::abs(rearRightMotorRPM)});

    // Check if any motor's RPM difference exceeds the threshold
    if (std::abs(frontLeftMotorRPM - minSpeed) > 5 || std::abs(rearLeftMotorRPM - minSpeed) > 5 ||
        std::abs(frontRightMotorRPM - minSpeed) > 5 || std::abs(rearRightMotorRPM - minSpeed) > 5)
    {
        // Reduce the forward voltage to match the slowest motor's speed
        forwardVolts = minSpeed * 0.7; // Adjust 0.7 if needed
    }
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

    while (Competition.isEnabled())
    {
        turnVolts = primaryController.Axis1.position() * 0.12; // -12 to 12
        forwardVolts = primaryController.Axis3.position() * 0.12 * (1 - (std::abs(turnVolts) / 12));

        // Open configuration menu
        if (primaryController.ButtonUp.pressing())
        {
            configMenuActive = true;
            displaySystemStates();

            while (primaryController.ButtonUp.pressing())
            {
                vex::this_thread::sleep_for(50); // Wait for the button to release
            }
        }

        // Configuration menu logic
        if (configMenuActive)
        {
            if (primaryController.ButtonX.pressing())
            {
                // Hold X to disable stability control, tap to toggle traction control
                vex::this_thread::sleep_for(300); // Debounce
                if (primaryController.ButtonX.pressing())
                {
                    stabilityControlEnabled = !stabilityControlEnabled;
                    logHandler("userControl", "Stability Control " + std::string(stabilityControlEnabled ? "ENABLED" : "DISABLED"), Log::Level::Info);
                }
                else
                {
                    tractionControlEnabled = !tractionControlEnabled;
                    logHandler("userControl", "Traction Control " + std::string(tractionControlEnabled ? "ENABLED" : "DISABLED"), Log::Level::Info);
                }
            }

            if (primaryController.ButtonB.pressing())
            {
                // Hold B to disable ABS
                vex::this_thread::sleep_for(300); // Debounce
                if (primaryController.ButtonB.pressing())
                {
                    absEnabled = !absEnabled;
                    logHandler("userControl", "ABS " + std::string(absEnabled ? "ENABLED" : "DISABLED"), Log::Level::Info);
                }
            }

            displaySystemStates(); // Update display with current settings
            continue;              // Skip motor control when in config menu
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

        // Motor control logic
        LeftDriveSmart.spin(vex::directionType::fwd, forwardVolts + turnVolts, vex::voltageUnits::volt);
        RightDriveSmart.spin(vex::directionType::fwd, forwardVolts - turnVolts, vex::voltageUnits::volt);

        vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
    }
}