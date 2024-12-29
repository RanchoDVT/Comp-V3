<h1 id="advanced-startup-vex-v5">Advanced Startup Vex V5</h1>
<h2 id="getting-started-">Getting started: ▶️</h2>
<ol>
<li>Install VScode</li>
<li>Download and install these extensions:<br>(<a href="https://marketplace.visualstudio.com/items?itemName=VEXRobotics.vexcode">VEX Robotics</a>, <a href="https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools">C/C++</a>, <a href="https://marketplace.visualstudio.com/items?itemName=ms-vscode.makefile-tools">Makefile Tools</a>)  </li>
<li>Install my <a href="https://github.com/RanchoDVT/Vex-SDK">Custom SDK</a></li>
</ol>
<h2 id="rewrite-3-0-">Rewrite (3.0)</h2>
<p>Here we go! This is a rewritten version of Comp_V2 (with some exceptions).<br>We are at b2; I need Vex V5 Hardware to test with. (I don&#39;t have it; it will be long before I can get the hardware.)</p>
<h3 id="3-0">3.0:</h3>
<ol>
<li>Custom theming ✅</li>
<li>AWD/FWD/RWD Switcher ✅</li>
<li>Maintenance reminders ✅</li>
<li>Diagnostic mode ✅ </li>
<li>Traction control &amp; Stability control ✅</li>
<li>Hot swap of Ports (Requires rewrite of user_control, 🔃) </li>
<li>Odometer ☑️ (Tamper protection could not be completed)</li>
<li>Controller settings ☑️ (Have to add deadzone settings)</li>
<li>Import code updates from internet (Requires rewrite of startup, need hardware ❌) </li>
<li>Automatic Emergency Braking (Requires vision sensor or AI vision, need hardware ❌)<ul>
<li>(Requires rewrite of User_Control | Prevent acceleration when a collision is imminent)</li>
<li>Forward warning - 2 Seconds till impact</li>
<li>Caution - 1.5 Seconds till impact</li>
<li>Emergency braking - Less than 1 second till impact</li>
</ul>
</li>
<li>PID for autonomous (need hardware ❌)</li>

</ol>
<hr>
<h3 id="website-todo-">Website todo:</h3>
<ol>
<li>Update the program from the website 🔃</li>
<li>Support forum 🔃</li>
</ol>
<hr>
<h3 id="custom-sdk-todo-">Custom SDK todo:</h3>
<ol>
<li>Use the newest version of Clang and GCC. ✅</li>
<li>Use Vex&#39;s API. ✅</li>
<li>Support for V5, IQ2, EXP 🔃</li>
<li>Better templates and makefiles. 🔃</li>
</ol>
