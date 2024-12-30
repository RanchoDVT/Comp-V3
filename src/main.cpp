// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// Drivetrain           drivetrain    1, 10, 11, 20, IG
// InertialGyro         inertial      2
// Radio                radio         6
// RearBumper           bumper        ThreeWirePort.A
// ---- END VEXCODE CONFIGURED DEVICES ----

#include "vex.h"

std::string Version = "3.0b3-f2 [DEV]";
std::string BuildDate = "12/29/24";

int main()
{
    printf("\033[2J\033[1;1H\033[0m"); // Clears console and Sets color to grey.
    ConfigManager.parseConfig();
    Competition.autonomous(autonomous);
    Competition.drivercontrol(userControl);
    vexCodeInit();
    while (Competition.isEnabled())
    {
        if (!primaryController.installed())
        {
            Brain.Screen.drawRectangle(10, 10, 300, 50, vex::color::red);
            Brain.Screen.setCursor(2, 2);
            Brain.Screen.print("primaryController disconnected!");
        }
        else if (!partnerController.installed())
        {
            Brain.Screen.drawRectangle(10, 10, 300, 50, vex::color::red);
            Brain.Screen.setCursor(2, 2);
            Brain.Screen.print("partnerController disconnected!");
        }
        else
        {
            Brain.Screen.drawRectangle(10, 10, 300, 50, vex::color::black);
        }
        vex::this_thread::sleep_for(25);
    }
}
