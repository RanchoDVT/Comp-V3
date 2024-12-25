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

std::map<std::string, std::vector<int>> controllerButtonsPressed(const vex::controller &controller)
{

	if (Competition.isEnabled())
	{
		logHandler("controllerButtonsPressed", "Robot is IN competition mode!", Log::Level::Error, 3);
	}

	std::map<std::string, std::vector<int>> buttonPressTimes;
	vex::timer pressTimer;

	auto checkButtonPress = [&](const ControllerButtonInfo &info)
	{
		if (info.button->pressing())
		{
			pressTimer.clear();
			while (info.button->pressing())
			{
				vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
			}
			buttonPressTimes[info.name].push_back(pressTimer.time());
		}
	};

	while (!Competition.isEnabled())
	{
		for (const auto &btn : AllControllerButtons)
		{
			checkButtonPress(btn);
		}
		vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
	}

	for (const auto &[button, durations] : buttonPressTimes)
	{
		for (auto duration : durations)
		{
			std::string message = std::format("Button: {}, Duration: {} ms", button, duration);
			logHandler("controllerButtonsPressed", message, Log::Level::Debug);
		}
	}

	return buttonPressTimes;
}

void motorMonitor()
{
	logHandler("motorMonitor", "motorMonitor is starting up...", Log::Level::Trace);

	auto logOverheat = [&](const std::string &motorName, int temperature)
	{
		if (temperature >= 55)
		{
			logHandler("motorMonitor", std::format("{} overheat: {}°", motorName, temperature), Log::Level::Warn, 3);
		}
	};

	while (Competition.isEnabled())
	{
		std::array motorTemps = {
			static_cast<int>(frontLeftMotor.temperature(vex::temperatureUnits::celsius)),
			static_cast<int>(frontRightMotor.temperature(vex::temperatureUnits::celsius)),
			static_cast<int>(rearLeftMotor.temperature(vex::temperatureUnits::celsius)),
			static_cast<int>(rearRightMotor.temperature(vex::temperatureUnits::celsius))};

		constexpr std::array motorNames = {"FLM", "FRM", "RLM", "RRM"};

		for (size_t i = 0; i < motorTemps.size(); ++i)
		{
			logOverheat(motorNames[i], motorTemps[i]);
		}

		if (Brain.Battery.voltage() < 12)
		{
			logHandler("motorMonitor", "Brain voltage at a critical level!", Log::Level::Warn, 3);
		}

		int leftMotorPosition = LeftDriveSmart.position(vex::rotationUnits::deg);
		int rightMotorPosition = RightDriveSmart.position(vex::rotationUnits::deg);
		int averagePosition = (leftMotorPosition + rightMotorPosition) / 2;

		ConfigManager.updateOdometer(averagePosition);

		std::string motorTempsStr = std::format("\n | LeftTemp: {}°\n | RightTemp: {}°\n | RearLeftTemp: {}°\n | RearRightTemp: {}°\n | Battery Voltage: {}V\n",
												motorTemps[0], motorTemps[1], motorTemps[2], motorTemps[3], Brain.Battery.voltage());
		logHandler("motorMonitor", motorTempsStr, Log::Level::Info);

		std::string dataBuffer = std::format("\nX Axis: {}\nY Axis: {}\nZ Axis: {}",
											 InertialGyro.pitch(vex::rotationUnits::deg),
											 InertialGyro.roll(vex::rotationUnits::deg),
											 InertialGyro.yaw(vex::rotationUnits::deg));
		logHandler("motorMonitor", dataBuffer, Log::Level::Info);

		clearScreen(false, true, true);
		primaryController.Screen.print("FLM: %d° | FRM: %d°", motorTemps[0], motorTemps[1]);
		primaryController.Screen.newLine();
		primaryController.Screen.print("RLM: %d° | RRM: %d°", motorTemps[2], motorTemps[3]);
		primaryController.Screen.newLine();
		primaryController.Screen.print("Battery: %.1fV", Brain.Battery.voltage());
		vex::this_thread::sleep_for(5000);
	}
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