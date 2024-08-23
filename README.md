# EvilPortalBLE
An Evil portal app that will log in to an app on Ble-connected phone with the credentials it receives.

This is a current work in progress and for demonstration purposes, I am using Tesla as an example company 

## Game plan is

Completed! - Create a Tesla Sign-On Screen that sends both the username and password then on submit asks and sends the One-Time Passcode

Create a Ble controller to send keys to the Tesla sign-in app
	- send username, press enter, send password, press enter, send one-time passcode. press enter
 ------I have created Ble scenes that can enter in the text, but I have not completed everything for the Bluetooth side so that code is untested
 
Create an evil portal app that sends received data straight to the Tesla app
- deploy evil portal
- on receiving data control phone sends the user and pass
- send one one-time passcode to the phone
 - Optional (but maybe necessary): Upon receiving connection from one user lock out any other info from other users so that there isn't dual input


After this is completed for the Tesla example app 
- Evil portal will select an HTML file and a config file
-  Normal html file for evil portal
-  Config file should contain expected inputs and outputs
