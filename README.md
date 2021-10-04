# Hue_Console
 Console app for interacting with Philips Hue API.

 Displays Full State of All lights on Bridge. Then will show only state changes after.

 if Username is not specified in the command line arguments 
 the monitor will prompt you to initiate a handshake with the bridge device to get a username.

# Arguments

 -v URL  :: Specify Virtual Bridge URL, default = localhost. 
    Use "localhost" for local virtual bridge

-b URL :: Specify Actual Bridge URL

-p port :: Specify Custom Port Number, default = 80

-u username :: Specify existing Username for bridge. Will initiate bridge handshake if username is not supplied.

-f seconds :: Specify polling frequency for state monitor, Default = 2 seconds

if -v is specified the API is set to Virtual Bridge Mode. 



