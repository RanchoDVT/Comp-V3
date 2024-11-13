# Advanced Startup Vex V5

## Getting started: ▶️
1. Install VScode
2. Download and install these extensions:  
([VEX Robotics](https://marketplace.visualstudio.com/items?itemName=VEXRobotics.vexcode), [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools), [Makefile Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools))  
3. Install my [Custom SDK](https://github.com/RanchoDVT/Vex-SDK)


## Rewrite (3.0)
Here we go! This is a rewritten version of Comp_V2 (with some exceptions).  
We are at b2; I need Vex V5 Hardware to test with. (I don't have it; it will be long before I can get the hardware.)

### 3.0b3:
1. Make custom SDK in an extension. 🔃
2. Custom theming 🔃 (Requires rewrite of startup, and needs brain setup)
3. Controller settings 🔃 (Requires rewrite of User_Control)
4. AWD/FWD/RWD Switcher 🔃 (Requires rewrite of User_Control)
5. Odometer 🔃 (Requires tamper protection)
6. Maintenance reminders 🔃
7. Custom user messages 🔃 (Requires rewrite of startup)
8. Automatic Emergency Braking 🔃 (Requires vision sensor or AI vision, need hardware ❌)
   - (Requires rewrite of User_Control | Prevent acceleration when a collision is imminent)
   - Forward warning - 2 Seconds till impact
   - Caution - 1.5 Seconds till impact
   - Emergency braking - Less than 1 second till impact
9. Traction control & Stability control 🔃 (Requires rewrite of User_Control, need hardware ❌)
10. More debugging when connected to a computer (Loghandler, need hardware ❌) 
11. PID for autonomous (need hardware ❌)
12. Import code updates from SD card (Requires rewrite of startup, need hardware ❌) 
13. Diagnostic mode (Requires rewrite of startup, need hardware ❌) 
14. Hot swap of Ports (Requires rewrite of user_control, need hardware ❌) 

### Website todo:
1. Blob effect around curser 🔃
2. Update the program from the website 🔃
3. Caching 🔃

### Custom SDK todo:
1. Use the newest version of Clang and GCC. 🔃
2. Use vex's API. 🔃
3. Support for V5, IQ2, EXP 🔃
4. Better templates and makefiles. 🔃
5. Pretty GUI for settings 🔃
6. Optimize for performance 🔃
7. Work on web versions of VsCode 🔃
8. IntelliSense! 🔃



