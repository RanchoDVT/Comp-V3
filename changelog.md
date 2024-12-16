<h1 id="version-history-">Version history 📕</h1>
<h2>Version 3.0b3 | 12/15/24 (Newest)</h2>
<p><strong>Build size:</strong> 229KB</p>
<h3>Fixed / Improved 🛠️</h3>
<ul>
<li>Improved <code>getuseroption</code> to support unlimited options with an SD card configuration.</li>
<li>More accurate turning.</li>
<li>Enhanced error handling in <code>StringToLong</code>.</li>
</ul>
<h3>Deprecated ✖️</h3>
<ul>
<li class="deprecated">Removed <code>LOCALLOGO</code>, <code>CTRLR2ENABLE</code>, and <code>VISIONENABLE</code>.</li>
</ul>
<h3>try/catch 🪝</h3>
<ul>
<li class="new-feature">Enabled try and catch for error handling in <code>StringToLong</code>.</li>
</ul>
<h2>Other Changes:</h2>
<ul>
<li>Updated VS Code settings to enable additional C++ file associations and error squiggles.</li>
<li>Updated VS Code configurations for the latest VEX SDK and Clang/GCC versions.</li>
<li>Significant improvements and additions to the <code>configManager</code> class, including new methods for handling complex configurations.</li>
<li>Added diagnostic mode and display task functions.</li>
<li>Improved logging with color-coded output and scrolling text on controller screens.</li>
<li>Various code cleanups and refactoring for better readability and maintainability.</li>
<li>New features and improvements in the <code>gifplayer</code> and <code>logHandler</code> functions.</li>
<li>Consolidated motor monitoring and diagnostic functionalities.</li>
</ul>
<h2 id="version-3-0b2-6-26-24">Version 3.0b2 | 6/26/24</h2>
<p>Build size: 374KB</p>
<h3 id="new-">New 🎉</h3>
<ul>
<li>New <code>configManager</code> for well, configuring things!<ul>
<li>With this, we can now get a part number for a motor, it&#39;s gear ratio, and whetherwhether it is reversed.</li>
<li> Maintenance warnings &#39;s after a certain distance are a thing now!</li>
</ul>
</li>
<li>gifplayer is back!</li>
<li>Updated custom sdk to 1.1.3!</li>
</ul>
<h3 id="fixed-improved-">Fixed / Improved 🛠️</h3>
<ul>
<li>I don&#39;t even know how many spelling fixes...</li>
<li>Better code structure to make it easy for everyone to look at.</li>
<li>Improved the Makefile, now supports any depth of folders for include and src.</li>
<li>Using -O3 rather than -Os for speed =) (Performance should be ~5% faster)</li>
</ul>
<h2 id="version-2-0pr4-5-15-24">Version 2.0pr4 | 5/15/24</h2>
<p>Build size: 339KB</p>
<h3 id="new-">New 🎉</h3>
<ul>
<li>New support for reading a cfg file on an SD card is here!  </li>
<li>New support for different gifs &#39;s for each mode (Loading, Auto, Drivercontrol).  </li>
<li><p>Redesigned <code>getUserOption</code>, and is blazing fast!  </p>
<ul>
<li>Old -&gt; Print to display: 0.6S.  </li>
<li>New -&gt; Print to display in: 0.2S.  </li>
</ul>
</li>
<li><p>Added new functions <code>resetOrInitializeConfig</code>, <code>stringtobool</code>, <code>stringtofloat</code>, <code>configParser</code>.</p>
</li>
<li><p>Custom PID!</p>
</li>
</ul>
<h3 id="fixed-improved-">Fixed / Improved 🛠️</h3>
<ul>
<li>In the function <code>setValForConfig</code>, Handling unknown keys in the configuration file by either resetting the config or ignoring them.</li>
<li>Modified variable initialization for improved readability and consistency.</li>
<li>Fixed a bug in the <code>stringtobool</code> function where the logical OR operator was incorrectly used.</li>
<li>Fixed a bug in the <code>configParser</code> function where the existence of the readme file was not properly checked.</li>
<li>Optimized many functions to use by reference, and threaded.</li>
<li>Fixed minor typos throughout the code.</li>
</ul>
<h2 id="version-2-0pr2-4-14-24">Version 2.0pr2 4/14/24</h2>
<p>Build size: 325KB</p>
<h3 id="new-">New: 🎉</h3>
<ul>
<li>New class: <code>Log::Level</code></li>
<li>New armVol for controlling the arm voltage setting.</li>
<li>New scrolling support for <code>get_User_Option</code>.</li>
<li>New function <code>motorMonitor</code> that shows motor temps and battery voltage on the controller screen.</li>
</ul>
<h3 id="fixed-updated-">Fixed / Updated: 🛠️</h3>
<ul>
<li>Updated <code>get_user_Option</code> for more protections.</li>
<li>Fixed bug in <code>get_User_Option</code> that when the user fails the log for what buttons are a repeat.</li>
<li>Fixed bug that does not show % on the controller screen. (It requires %%)</li>
<li>Performance fixes by using references.</li>
</ul>
<h2 id="version-1-0-0-3-9-24">Version 1.0.0 3/9/24</h2>
<p>Build size: 280KB</p>
<h3 id="new-">New: 🎉</h3>
<ul>
<li>Added a function <code>clear_Screen</code> to clear the controller and, optionally, the brain`s screen.</li>
<li>Added a function <code>error_handler</code> to handle and display error messages with optional output to the controller`s screen and colored output to the console.</li>
<li>Implemented autonomous and user control tasks with threading to run continuously during the respective phases of a VEX Competition.</li>
<li>Implemented a main function to set up competition callbacks for autonomous and driver control periods, start GUI startup, and handle continuous control of the drivetrain while preventing the main loop from exiting.</li>
</ul>
<h3 id="fixed-updated-">Fixed / Updated: 🛠</h3>
<ul>
<li>Faster execution time with <code>ctrl1_Bttn_Presd</code> using break to mimic a switch and case.</li>
<li>Improved <code>ctrl1_Bttn_Presd</code> print statement, now using a local variable as the way to print it on Debug_Mode.</li>
<li>Updated <code>ctrl1_Bttn_Presd</code> function to include error handling and prevent function execution if <code>controller1Command</code> flag is set.</li>
<li>Updated function <code>get_User_Option</code> to display options and get user input with error handling and prevention of function execution if <code>controller1Command</code> flag is set.</li>
<li>Updated <code>controller_L1_Pressed</code>, <code>controller_L2_Pressed</code>, <code>controller_R1_Pressed</code>, and <code>controller_R2_Pressed</code> functions to include error handling, to replace the <code>rc_auto_loop_function_Controller1</code>, and prevent function execution if <code>controller1Command</code> flag is not set.</li>
<li>Updated the <code>setup</code> function to create controller callback events, and new safety checks on startup.</li>
<li>Improved polling rate from 50Hz to 8000Hz on <code>ctrl1_Bttn_Presd</code> and the controller loop. (160X faster than the competition.)</li>
<li>Improved Global variables and function naming.</li>
<li>Improved comments.</li>
<li>Lots of performance improvements.</li>
<li>Lots of spelling fixes.</li>
</ul>
<h2 id="version-0-3-0a6-1-27-24">Version 0.3.0a6 / 1/27/24</h2>
<p>Build size: 260KB  </p>
<h3 id="new-">New: ✨</h3>
<ul>
<li>The <code>rc_auto_loop_function_Controller1</code> is now run as a separate thread using vex::thread. This allows the main program to continue executing other tasks concurrently.</li>
<li>Thread creation and execution are printed to the console for debugging purposes.</li>
<li>Added print statements to indicate the start and completion of threading and callback operations.</li>
</ul>
<h3 id="fixed-">Fixed: 🛠️</h3>
<ul>
<li>Lots of spelling fixes.</li>
</ul>
<h2 id="version-0-3-0a5-1-26-24">Version 0.3.0a5 / 1/26/24</h2>
<p>Build size: 260KB  </p>
<h3 id="new-">New: ✨</h3>
<ul>
<li>Added a debug mode for easier debugging. (Prints useful debug data to the console)</li>
<li>Created a new task (<code>rc_auto_loop_function_Controller1</code>) to monitor inputs from Controller1 in a loop, replacing multiple event handlers.</li>
<li>Added a new user control task (<code>usercontrol</code>) to manage user control operations.</li>
<li>Removed 3 global variables (<code>major</code>, <code>minor</code>, <code>patch</code>) in favor of a new one (<code>Version</code>).</li>
</ul>
<h3 id="fixed-improved-">Fixed / Improved: 🛠️</h3>
<ul>
<li>Updated print statements for better readability.</li>
<li>Improved organization and comments for better code structure.</li>
<li>Enhanced the control of motors based on joystick inputs for better responsiveness.</li>
<li>Corrected the print color for the main function.</li>
<li>Made new loops in GetUserOption for displaying options and general improvements to it.</li>
<li>Removed unnecessary delays and added appropriate waits for better performance.</li>
<li>Removed redundant statements in the setup function.</li>
</ul>
<h3 id="removed-">Removed: ❌</h3>
<ul>
<li>Removed unnecessary print statements for button presses.</li>
</ul>
<h2 id="0-3-0a4-1-23-24">0.3.0a4 / 1/23/24</h2>
<p>Build size: 259KB  </p>
<h3 id="new-">New: ✨</h3>
<ul>
<li>New images with data!</li>
</ul>
<h3 id="fixed-improved-">Fixed / Improved: 🛠️</h3>
<ul>
<li>General code fixes.</li>
</ul>
<h3 id="removed-">Removed: ❌</h3>
<ul>
<li><code>vex.h</code>, <code>robot-config.h</code>, <code>robot-config.cpp</code></li>
</ul>
<h2 id="0-3-2a1-1-22-24">0.3.2a1 / 1/22/24</h2>
<h3 id="new-">New: ✨</h3>
<ul>
<li>New files! (<code>robot-config.h</code>, <code>robot-config.cpp</code>)</li>
<li>Made all code modular.</li>
<li>Autonomous control functions ready for writing.</li>
<li>Improved <code>getUserOption</code> functions.</li>
<li>Version printing.</li>
</ul>
<h3 id="fixed-improved-">Fixed / Improved: 🛠️</h3>
<ul>
<li>Main from exiting with an infinite loop.</li>
<li>Reduced file size from 563 KB to 258KB. (a 218% size reduction!) This required me to remove some standard library headers.</li>
<li>(Removed: <code>stdlib.h</code>, <code>string.h</code>, <code>math.h</code>. Added: <code>string</code>, <code>iostream</code>)</li>
<li>Better and <em>WAY</em> more comments.</li>
<li>Fix for inconsistency with 2 spaces vs a tab. This has been converted to 2 spaces.</li>
<li>Fix <code>ctrl1_Bttn_Presd</code> for if a user presses anything other than A it would return nothing.</li>
</ul>
<h3 id="removed-">Removed: ❌</h3>
<ul>
<li>Vex Sound play helper.</li>
<li>Input reading from controller1 as a task.</li>
</ul>
