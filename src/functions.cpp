#include "vex.h"

void calibrateGyro()
{
	InertialGyro.calibrate();
	while (InertialGyro.isCalibrating())
	{
		vex::this_thread::sleep_for(20);
	}
	logHandler("calibrateGyro", "Finished calibrating Inertial Giro.", Log::Level::Trace);
	return;
}

std::pair<std::string, int> ctrl1BttnPressed()
{
	if (Competition.isEnabled())
	{
		logHandler("ctrl1BttnPressed", "Robot is IN competition mode!", Log::Level::Error, 3);
	}

	std::string buttonPressed;
	int pressDuration = 0;
	vex::timer pressTimer;
	while (!Competition.isEnabled())
	{
		if (primaryController.ButtonA.pressing())
		{
			buttonPressed = "A";
			pressTimer.clear();
			while (primaryController.ButtonA.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonX.pressing())
		{
			buttonPressed = "X";
			pressTimer.clear();
			while (primaryController.ButtonX.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonY.pressing())
		{
			buttonPressed = "Y";
			pressTimer.clear();
			while (primaryController.ButtonY.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonB.pressing())
		{
			buttonPressed = "B";
			pressTimer.clear();
			while (primaryController.ButtonB.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonUp.pressing())
		{
			buttonPressed = "UP";
			pressTimer.clear();
			while (primaryController.ButtonUp.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonDown.pressing())
		{
			buttonPressed = "DOWN";
			pressTimer.clear();
			while (primaryController.ButtonDown.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonRight.pressing())
		{
			buttonPressed = "RIGHT";
			pressTimer.clear();
			while (primaryController.ButtonRight.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonLeft.pressing())
		{
			buttonPressed = "LEFT";
			pressTimer.clear();
			while (primaryController.ButtonLeft.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonL1.pressing())
		{
			buttonPressed = "L1";
			pressTimer.clear();
			while (primaryController.ButtonL1.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonL2.pressing())
		{
			buttonPressed = "L2";
			pressTimer.clear();
			while (primaryController.ButtonL2.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonR1.pressing())
		{
			buttonPressed = "R1";
			pressTimer.clear();
			while (primaryController.ButtonR1.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}
		else if (primaryController.ButtonR2.pressing())
		{
			buttonPressed = "R2";
			pressTimer.clear();
			while (primaryController.ButtonR2.pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			pressDuration = pressTimer.time();
			break;
		}

		vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
	}

	std::string message = "Selected button: " + buttonPressed + ", Duration: " + std::to_string(pressDuration) + " ms";
	logHandler("ctrl1BttnPressed", message, Log::Level::Debug);
	return std::make_pair(buttonPressed, pressDuration);
}

void motorMonitor()
{
	logHandler("motorMonitor", "motorMonitor is starting up...", Log::Level::Trace);

	while (Competition.isEnabled())
	{
		int frontLeftTemp = frontLeftMotor.temperature(vex::temperatureUnits::celsius);
		int frontRightTemp = frontRightMotor.temperature(vex::temperatureUnits::celsius);
		int rearLeftTemp = rearLeftMotor.temperature(vex::temperatureUnits::celsius);
		int rearRightTemp = rearRightMotor.temperature(vex::temperatureUnits::celsius);

		// Get the position of both motors
		int leftMotorPosition = LeftDriveSmart.position(vex::rotationUnits::deg);
		int rightMotorPosition = RightDriveSmart.position(vex::rotationUnits::deg);

		// Check for overheat conditions for each motor
		if (frontLeftTemp >= 55)
		{
			logHandler("motorMonitor", "FLM overheat: " + std::to_string(frontLeftTemp) + "°", Log::Level::Warn, 3);
		}
		if (frontRightTemp >= 55)
		{
			logHandler("motorMonitor", "FRM overheat: " + std::to_string(frontRightTemp) + "°", Log::Level::Warn, 3);
		}
		if (rearLeftTemp >= 55)
		{
			logHandler("motorMonitor", "RLM overheat: " + std::to_string(rearLeftTemp) + "°", Log::Level::Warn, 3);
		}
		if (rearRightTemp >= 55)
		{
			logHandler("motorMonitor", "RRM overheat: " + std::to_string(rearRightTemp) + "°", Log::Level::Warn, 3);
		}
		if (Brain.Battery.voltage() < 12)
		{
			logHandler("motorMonitor", "Brain voltage at a critical level!", Log::Level::Warn, 3);
		}
		// Get the average of the two motors
		int averagePosition = (leftMotorPosition + rightMotorPosition) / 2;

		// Update the odometer with the new position
		ConfigManager.updateOdometer(averagePosition);

		// Log motor temperatures
		std::string motorTemps = "\n | LeftTemp: " + std::to_string(frontLeftTemp) + "°\n | RightTemp: " + std::to_string(frontRightTemp) + "°\n | RearLeftTemp: " + std::to_string(rearLeftTemp) + "°\n | RearRightTemp: " + std::to_string(rearRightTemp) + "°\n | Battery Voltage: " + std::to_string(Brain.Battery.voltage()) + "V\n";
		logHandler("motorMonitor", motorTemps, Log::Level::Info);
		std::string dataBuffer = "\nX Axis: " + std::to_string(InertialGyro.pitch(vex::rotationUnits::deg)) + "\nY Axis: " + std::to_string(InertialGyro.roll(vex::rotationUnits::deg)) + "\nZ Axis: " + std::to_string(InertialGyro.yaw(vex::rotationUnits::deg));
		logHandler("motorMonitor", dataBuffer, Log::Level::Info);
		clearScreen(false, true, true);
		primaryController.Screen.print("FLM: %d° | FRM: %d°", frontLeftTemp, frontRightTemp);
		primaryController.Screen.newLine();
		primaryController.Screen.print("RLM: %d° | RRM: %d°", rearLeftTemp, rearRightTemp);
		primaryController.Screen.newLine();
		primaryController.Screen.print("Battery: %.1fV", Brain.Battery.voltage());
		vex::this_thread::sleep_for(5000);
	}
	return;
}

void gifplayer()
{
	if (!ConfigManager.getPrintLogo())
	{
		return;
	}
	else if (!Competition.isEnabled())
	{
		vex::Gif gif("assets/loading.gif", 0, 0);
		while (!Competition.isEnabled())
		{
			Brain.Screen.print("");
		}
	}
	else if (Competition.isAutonomous())
	{
		vex::Gif gif("assets/auto.gif", 0, 0);
		while (Competition.isAutonomous())
		{
			Brain.Screen.print("");
		}
	}
	else
	{
		vex::Gif gif("assets/driver.gif", 0, 0);
		while (Competition.isDriverControl())
		{
			Brain.Screen.print("");
		}
	}
}