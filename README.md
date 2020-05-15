# NovaPLC-2019.07 PLC
# (C) Copyright 2019 Novasom Industries
This repo contains the tools and sources for the PLC app on NOVAsdk2019.07 Novasom Industries 
project.  
It can be downloaded at at [NOVAsomIndustries on github](https://novasomindustries.github.io/Doc/)
Based on https://github.com/thiagoralves/OpenPLC_v2 by Thiago Alves

## Prerequisities
A running Debian 9 or an equivalent virtual machine

## Usage
1) Load novaplc executable and run in background ( ./novaplc & )
2) Use writefifo to send one of the three available commands ( START , PAUSE, STOP ) : ./writefifo <command>
   The STOP command shuts down novaplc and is intended as an emergency stop, while pause stops operation until a START is issued

## Authors
* Filippo Visocchi 	 - Initial work - [NOVAsomIndustries](http://www.novasomindustries.com)  
* Michele Puca     	 - Initial work - [NOVAsomIndustries](http://www.novasomindustries.com)  
See also the list of [contributors](https://github.com/NovasomIndustries/Doc/blob/master/contributors) who participated in this project.

## License
This project is licensed under the [GNU GENERAL PUBLIC LICENSE ](https://github.com/NovasomIndustries/Doc/blob/master/LICENSE.md)
