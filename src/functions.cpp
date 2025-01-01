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

    std::array<ControllerButtonInfo, 12> AllControllerButtons = createControllerButtonArray(controller);

    auto checkButtonPress = [&](const ControllerButtonInfo &info)
    {
        if (info.button->pressing())
        {
            if (buttonPressTimes[info.name].empty() || buttonPressTimes[info.name].back() != -1)
            {
                pressTimer.clear();
                buttonPressTimes[info.name].push_back(-1); // Mark the start of a press
            }
        }
        else
        {
            if (!buttonPressTimes[info.name].empty() && buttonPressTimes[info.name].back() == -1)
            {
                buttonPressTimes[info.name].back() = pressTimer.time(); // Record the press duration
            }
        }
    };

    while (!Competition.isEnabled())
    {
        for (const auto &buttonInfo : AllControllerButtons)
        {
            checkButtonPress(buttonInfo);
        }
        vex::this_thread::sleep_for(ConfigManager.getCtrlr1PollingRate());
    }

    for (const auto &[buttonName, pressDurations] : buttonPressTimes)
    {
        for (auto pressDuration : pressDurations)
        {
            std::string logMessage = std::format("Button: {}, Duration: {} ms", buttonName, pressDuration);
            logHandler("controllerButtonsPressed", logMessage, Log::Level::Debug);
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

        primaryController.Screen.clearScreen();
        primaryController.Screen.setCursor(1, 1);
        partnerController.Screen.clearScreen();
        partnerController.Screen.setCursor(1, 1);

        std::string display = std::format("FLM: {}° | FRM: {}°\nRLM: {}° | RRM: {}°\nBattery: {:.1f}V",
                                          motorTemps[0], motorTemps[1], motorTemps[2], motorTemps[3], Brain.Battery.voltage());

        primaryController.Screen.print(display.c_str());
        partnerController.Screen.print(display.c_str());
        vex::this_thread::sleep_for(5000);
    }
}

void gifplayer(bool enableVsync)
{
    if (!ConfigManager.getPrintLogo())
    {
        return;
    }

    if (enableVsync)
    {
        Brain.Screen.waitForRefresh();
    }

    if (!Competition.isEnabled())
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