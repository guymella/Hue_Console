# Hue_Console
 Console app for interacting with Philips Hue API.

 Displays Full State of All lights on Bridge. Then will show only state changes after.

# Arguments

 -v <URL>  :: Specify Virtual Bridge URL, default = localhost
    Use "localhost" for local virtual bridge

-b <URL> :: Specify Actual Bridge URL

-p <int> :: Port Number if diff, default = 80

-u <username> :: Specify existing Username for Bridge. Will initiate Bridge Handshake if username is not supplied.

-f <int> :: Specify Polling frequency for State Monitor, Default = 2 seconds

if -v is specified the API is set to Virtual Bridge Mode. 



