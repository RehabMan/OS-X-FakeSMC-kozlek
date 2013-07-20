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

Open source SMC device driver/emulator (FakeSMC) by netkas (http://netkas.org/) with 
hardware monitoring plugins.

IMPORTANT: Do not install DRIVERS on real mac! This even may work but you are doing
it at your own risk. On the other side HWMonitor.app is fully compatible with Macs 
so you can still use it to monitor hardware sensors provided by SMC.

All repositories:

Sourceforge, sources & downloads: https://sourceforge.net/projects/hwsensors/
BitBucket, sources & downloads: https://bitbucket.org/kozlek/hwsensors/overview
Assembla, sources: https://www.assembla.com/code/fakesmc/git/nodes
GitHub, sources: https://github.com/kozlek/HWSensors

HWSensors Project (c) 2013 netkas, slice, usr-sse2, kozlek, navi, THe KiNG, RehabMan 
and others. All rights reserved.

With special thanks to:
netkas for fakesmc
slice for plugins and help in developing the project
usr-sse2 for help in development and first FakeSMC plugins idea and realization 
Navi for investigation of numeric SMC values encoding/decoding and other useful stuff
droplets for testing
Michael Möller for OpenHardwareMonitor
The Real Deal & JrCs for French localization
RehabMan forced me using mutexes and other contributions to the project
Oliver Bolton for OBMenuBarWindow
Mozketo for LaunchAtLoginController
PHPdev32 for an idea to using NVRAM as storage for kext configuration
oswaldini @ www.osx86.org.pl for Polish translation
Marchrius @ insanelymac.com for Italian translation
k3nny @ www.insanelymac.com for German translation
