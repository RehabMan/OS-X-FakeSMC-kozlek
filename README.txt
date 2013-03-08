Notes about this fork by RehabMan:

Note: This version forked from kozlek's branch at: https://www.assembla.com/code/fakesmc/git/nodes 

Download link for RehabMan builds: https://code.google.com/p/os-x-fake-smc-kozlek/downloads/list

A note about the contents of the download:
Debug/FakeSMC.kext - debug FakeSMC.kext + PlugIns (install for troubleshooting)
Release/FakeSMC.kext - release FakeSMC.kext + PlugIns (recommended install)

Debug/HWMonitor.app - debug version of kozlek's HWMonitor.app
Release/HWMonitor.app - release version of kozlek's HWMonitor.app (recommended)

SL/HWMonitor.app - release version of slice's HWMonitor.app, modified to
 work with kozlek's FakeSMC.kext.  Primarily for use on Snow Leopard, but
 will also work on ML or Lion.

To install:
- install FakeSMC.kext using Kext Wizard.
  (Note: if you had previously used slice's and have the various kexts 
   spread all over /S/L/E, be sure to remove them all)
- run HWMonitor.app and set it up to your preference.



README.TXT from original repo
----------------------------------------------------------------------------

Open source fake SMC device driver/emulator by netkas (http://netkas.org/) with 
hardware monitoring plugins.


All repositories:

BitBucket, sources & downloads: https://bitbucket.org/kozlek/hwsensors/overview
Assembla, sources: https://www.assembla.com/code/fakesmc/git/nodes
GitHub, sources: https://github.com/kozlek/HWSensors

IMPORTANT: Do not install drivers on real mac! This even may work but you are doing 
it at your own risk.

HWSensors Project (c) 2013 netkas, slice, usr-sse2, kozlek, navi, THe KiNG, RehabMan 
and others. All rights reserved.

With special thanks to:
netkas for fakesmc
slice for plugins and help in developing the project
usr-sse2 for help in development and first FakeSMC plugins idea and realization
Navi for investigation of numeric SMC values encoding/decoding and other useful stuff
Michael Möller for OpenHardwareMonitor
The Real Deal for French localization
RehabMan forced me using mutexes and for other good ideas
Vadim Shpanovski for custom popover
Mozketo for LaunchAtLoginController
