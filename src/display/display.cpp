#include "vex.h"

#include <algorithm>

// define some more colors
const vex::color grey((uint32_t)0x202020);
const vex::color dgrey((uint32_t)0x2F4F4F);
const vex::color lblue((uint32_t)0x303060);
const vex::color lred((uint32_t)0x603030);

/*-----------------------------------------------------------------------------*/
/** @brief      Display data for one motor                                     */
/*-----------------------------------------------------------------------------*/
void displayMotorData(vex::motor &m) {
    int ypos = 0;
    int xpos = m.index() * 48;

    // There's no C++ API to get motor command value, use C API, this returns rpm
    int v1  = vexMotorVelocityGet(m.index());

    // The actual velocity of the motor in rpm
    int v2  = m.velocity(vex::velocityUnits::rpm);

    // The position of the motor internal encoder in revolutions
    double pos = m.position(vex::rotationUnits::rev);

    // Motor current in Amps    
    double c1 = m.current();

    // Motor temperature
    double t1 = m.temperature(vex::temperatureUnits::celsius);

    Brain.Screen.setFont(vex::mono15);

    // background color based on
    // device and whether it's left, right or other motor
    if (!m.installed()) {
        // no motor
        Brain.Screen.setFillColor(grey);
    } else if (m.index() == frontLeftMotor.index() || m.index() == rearLeftMotor.index()) {
        Brain.Screen.setFillColor(lblue);
    } else if (m.index() == frontRightMotor.index() || m.index() == rearRightMotor.index()) {
        Brain.Screen.setFillColor(lred);
    } else {
        Brain.Screen.setFillColor(dgrey);
    }
    
    // Draw outline for motor info
    Brain.Screen.setPenColor(vex::white);
    int w = (m.index() < 9) ? 49 : 48;
    Brain.Screen.drawRectangle(xpos, ypos, w, 79);
    
    // no motor, then return early
    if (!m.installed()) {
        Brain.Screen.printAt(xpos + 15, ypos + 30, true, "NC");
        return;
    }

    // Show commanded speed
    if (v1 != 0)
        Brain.Screen.setPenColor(vex::green);
    else
        Brain.Screen.setPenColor(vex::white);
    Brain.Screen.printAt(xpos + 13, ypos + 13, true, "%4d", v1);

    // Show actual speed
    Brain.Screen.setPenColor(vex::yellow);
    Brain.Screen.printAt(xpos + 13, ypos + 30, true, "%4d", v2);

    // Show position
    Brain.Screen.printAt(xpos + 5, ypos + 45, true, "%5.1f", pos);

    // Show current
    Brain.Screen.printAt(xpos + 5, ypos + 60, true, "%4.1fA", c1);

    // Show temperature
    Brain.Screen.printAt(xpos + 5, ypos + 75, true, "%4.0fC", t1);

    Brain.Screen.setPenColor(vex::white);
    Brain.Screen.drawLine(xpos, ypos + 14, xpos + 48, ypos + 14);  
}

/*----------------------------------------------------------------------------*/
/** @brief  Display task - show some useful motor data                        */
/*----------------------------------------------------------------------------*/
void displayTask() {
    Brain.Screen.setFont(vex::prop40);
    Brain.Screen.setPenColor(vex::red);
    Brain.Screen.printAt(90, 160, "DIAGNOSTIC MODE");

    vex::motor *motors[] = { &frontLeftMotor, 
                             &frontRightMotor,
                             &rearLeftMotor,
                             &rearRightMotor
                           };
                      
    while (true) {
        for (int i = 0; i < 4; i++) {
            displayMotorData(*motors[i]);
        }

        // Display battery status
        Brain.Screen.setCursor(10, 1);
        Brain.Screen.print("Battery: %d%%", Brain.Battery.capacity());

        // Display using back buffer, stops flickering
        Brain.Screen.render();

        vex::this_thread::sleep_for(100);
    }
}

void clearScreen(const bool &brainClear, const bool &primaryControllerClear, const bool &partnerControllerClear)
{
    if (brainClear)
    {
        Brain.Screen.clearScreen();
        Brain.Screen.setCursor(1, 1);
    }
    if (primaryControllerClear)
    {
        primaryController.Screen.clearScreen();
        primaryController.Screen.setCursor(1, 1);
    }
    if (partnerControllerClear)
    {
        partnerController.Screen.clearScreen();
        partnerController.Screen.setCursor(1, 1);
    }
    logHandler("clearScreen", "Finished clearScreen", Log::Level::Trace);
    return;
}

std::string getUserOption(const std::string &settingName, const std::vector<std::string> &options)
{
    if (Competition.isEnabled())
    {
        logHandler("getUserOption", "Robot is IN competition mode!", Log::Level::Error, 2);
    }

    if (options.size() > ConfigManager.getMaxOptionSize() || options.size() < 2)
    {
        logHandler("getUserOption", "`options` size error!", Log::Level::Error, 2);
        return "DEFAULT";
    }

    std::string optmessage = "Options: ";
    for (auto optionIterator = options.begin(); optionIterator != options.end(); ++optionIterator)
    {
        if (optionIterator != options.begin())
        {
            optmessage += ", "; // Separate options with a comma and space (except for the first option)
        }
        optmessage += *optionIterator;
    }

    logHandler("getUserOption", optmessage, Log::Level::Debug);

    std::size_t wrongAttemptCount = 0;
    const std::size_t maxWrongAttempts = 3;
    const std::string wrongMessages[maxWrongAttempts] = {
        "Invalid selection!", "Do you need a map?", "Rocks can do this!"};
    std::string buttonString;
    // Have a index number for what they selected and convert it to the option.
    std::size_t Index = options.size(); // Invalid selection by default
    int offset = 0;

    const std::vector<std::string> &buttons = {"A", "X", "Y", "B"};
    const std::vector<std::string> &scrollButtons = {"DOWN", "UP"};

    while (!primaryController.installed())
    {
        vex::this_thread::sleep_for(20);
    }

    while (!Competition.isEnabled() and primaryController.installed())
    {
        buttonString.clear();
        clearScreen(false, true, true);
        primaryController.Screen.print(settingName.c_str());
        partnerController.Screen.print("Waiting for #1...");

        int displayedOptions = std::min(static_cast<int>(options.size() - offset), static_cast<int>(buttons.size()));
        for (int i = 0; i < displayedOptions; ++i) // Display available options
        {
            primaryController.Screen.newLine();
            primaryController.Screen.print("%s: %s", buttons[i].c_str(), options[i + offset].c_str());
            buttonString += buttons[i];
            if (i != displayedOptions - 1) // Add comma if not the last button
            {
                buttonString += ", ";
            }
        }

        if (options.size() > buttons.size())
        {
            primaryController.Screen.newLine();
            if (offset + displayedOptions < static_cast<int>(options.size()))
            {
                primaryController.Screen.print(">");
            }
            if (offset > 0)
            {
                primaryController.Screen.print("^");
            }
        }

        optmessage = "Available buttons for current visible options: " + buttonString; // Append button string to message.
        logHandler("getUserOption", optmessage, Log::Level::Debug);

        const std::string &buttonPressed = std::get<0>(ctrl1BttnPressed());

        auto buttonIt = std::find(buttons.begin(), buttons.end(), buttonPressed);
        if (buttonIt != buttons.end())
        {
            int buttonIndex = std::distance(buttons.begin(), buttonIt);
            if (buttonIndex < displayedOptions)
            {
                Index = buttonIndex + offset;
                logHandler("getUserOption", "[Valid Selection] Index = " + std::to_string(Index) + " | Offset = " + std::to_string(offset), Log::Level::Debug);
                break;
            }
        }
        else if (buttonPressed == "DOWN" && (offset + displayedOptions < static_cast<int>(options.size())))
        {
            ++offset;
            logHandler("getUserOption", "[Scroll DOWN] Offset = " + std::to_string(offset), Log::Level::Debug);
        }
        else if (buttonPressed == "UP" && offset > 0)
        {
            --offset;
            logHandler("getUserOption", "[Scroll UP] Offset = " + std::to_string(offset), Log::Level::Debug);
        }
        else
        {
            logHandler("getUserOption", "[Invalid Selection] Index = " + std::to_string(Index) + " | Offset = " + std::to_string(offset), Log::Level::Debug);
            // Display message
            if (wrongAttemptCount < maxWrongAttempts)
            {
                clearScreen(false, true, true);
                primaryController.Screen.print(wrongMessages[wrongAttemptCount].c_str());
                ++wrongAttemptCount; // Increment wrong attempt count
                logHandler("getUserOption", "wrongAttemptCount: " + std::to_string(wrongAttemptCount), Log::Level::Debug);
                vex::this_thread::sleep_for(2000);
            }
            else
            {
                logHandler("getUserOption", "Your half arsed.", Log::Level::Fatal);
            }
        }
    }
    clearScreen(false, true, true);
    return options[Index];
}
