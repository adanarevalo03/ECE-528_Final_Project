# ECE528 Final Project: Line Follower with Bluetooth Control Arm

<img width="2048" height="1536" alt="line_follower" src="https://github.com/user-attachments/assets/732a746f-ba96-4178-bc31-0348d5b7d19e" />

# INTRODUCTION
This is my final project for ECE 528, where I demonstrate a line-following rover with a Bluetooth-controlled robotic arm using the MSP432 microcontroller. The rover includes a mechanical arm that is controlled through Bluetooth using the Adafruit Bluefruit LE UART module. We are using Adafruit’s GUI application, which allows the user to send commands to the robotic arm.

The user can control the movement of the arm by moving it up or down, as well as adjusting the wrist position of the claw. In addition, four buttons were implemented in the GUI. The first two buttons are used to open and close the claw, while the last two buttons are used to stop the rover or make the rover turn around and continue moving forward.

The line-following algorithm used for the rover is basic and relies on simple if statements. The rover continuously reads values from the sensors and adjusts its movement left or right whenever the correct line values are not detected. 

