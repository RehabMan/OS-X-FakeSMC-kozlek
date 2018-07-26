DIST1=RehabMan-FakeSMC
DIST2=RehabMan-SL-HWMonitor

VERSION_ERA=$(shell ./print_version.sh)
ifeq "$(VERSION_ERA)" "10.10-"
	INSTDIR=/System/Library/Extensions
else
	INSTDIR=/Library/Extensions
endif

ifeq ($(findstring 32,$(BITS)),32)
OPTIONS:=$(OPTIONS) -arch i386
endif

ifeq ($(findstring 64,$(BITS)),64)
OPTIONS:=$(OPTIONS) -arch x86_64
endif


.PHONY: all
all:
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Debug
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Release
	xcodebuild build -project HWMonitor.xcodeproj -configuration Debug
	xcodebuild build -project HWMonitor.xcodeproj -configuration Release

.PHONY: clean
clean:
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Debug
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Release
	xcodebuild clean -project HWMonitor.xcodeproj -configuration Debug
	xcodebuild clean -project HWMonitor.xcodeproj -configuration Release
	rm revision.txt

.PHONY: install
install:
	sudo rm -rf $(INSTDIR)/FakeSMC*.kext
	sudo cp -R Build/Products/Release/FakeSMC.kext $(INSTDIR)
	if [ "`which tag`" != "" ]; then sudo tag -a Blue $(INSTDIR)/FakeSMC*.kext; fi
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_debug
install_debug:
	sudo rm -rf $(INSTDIR)/FakeSMC*.kext
	sudo cp -R Build/Products/Debug/FakeSMC.kext $(INSTDIR)
	if [ "`which tag`" != "" ]; then sudo tag -a Purple $(INSTDIR)/FakeSMC*.kext; fi
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_all
install_all:
	sudo rm -rf $(INSTDIR)/FakeSMC*.kext
	sudo cp -R Build/Products/Release/FakeSMC.kext $(INSTDIR)
	sudo cp -R Build/Products/Release/CPUSensors.kext $(INSTDIR)/FakeSMC_CPUSensors.kext
	sudo cp -R Build/Products/Release/ACPISensors.kext $(INSTDIR)/FakeSMC_ACPISensors.kext
	sudo cp -R Build/Products/Release/LPCSensors.kext $(INSTDIR)/FakeSMC_LPCSensors.kext
	sudo cp -R Build/Products/Release/GPUSensors.kext $(INSTDIR)/FakeSMC_GPUSensors.kext
	sudo cp -R Build/Products/Release/SMMSensors.kext $(INSTDIR)/FakeSMC_SMMSensors.kext
	if [ "`which tag`" != "" ]; then sudo tag -a Blue $(INSTDIR)/FakeSMC*.kext; fi
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_debug_all
install_debug_all:
	sudo rm -rf $(INSTDIR)/FakeSMC*.kext
	sudo cp -R Build/Products/Debug/FakeSMC.kext $(INSTDIR)
	sudo cp -R Build/Products/Debug/CPUSensors.kext $(INSTDIR)/FakeSMC_CPUSensors.kext
	sudo cp -R Build/Products/Debug/ACPISensors.kext $(INSTDIR)/FakeSMC_ACPISensors.kext
	sudo cp -R Build/Products/Debug/LPCSensors.kext $(INSTDIR)/FakeSMC_LPCSensors.kext
	sudo cp -R Build/Products/Debug/GPUSensors.kext $(INSTDIR)/FakeSMC_GPUSensors.kext
	sudo cp -R Build/Products/Debug/SMMSensors.kext $(INSTDIR)/FakeSMC_SMMSensors.kext
	if [ "`which tag`" != "" ]; then sudo tag -a Purple $(INSTDIR)/FakeSMC*.kext; fi
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: distribute
distribute:
	if [ -e ./Distribute ]; then rm -r ./Distribute; fi
	mkdir ./Distribute
	cp -R ./Build/Products/Release/FakeSMC.kext ./Distribute
	cp -R ./Build/Products/Release/CPUSensors.kext ./Distribute/FakeSMC_CPUSensors.kext
	cp -R ./Build/Products/Release/ACPISensors.kext ./Distribute/FakeSMC_ACPISensors.kext
	cp -R ./Build/Products/Release/LPCSensors.kext ./Distribute/FakeSMC_LPCSensors.kext
	cp -R ./Build/Products/Release/GPUSensors.kext ./Distribute/FakeSMC_GPUSensors.kext
	cp -R ./Build/Products/Release/SMMSensors.kext ./Distribute/FakeSMC_SMMSensors.kext
	cp -R ./Build/Products/Release/HWMonitor.app ./Distribute
	find ./Distribute -path *.DS_Store -delete
	find ./Distribute -path *.dSYM -exec echo rm -r {} \; >/tmp/org.kozlek.rm.dsym.sh
	chmod +x /tmp/org.kozlek.rm.dsym.sh
	/tmp/org.kozlek.rm.dsym.sh
	rm /tmp/org.kozlek.rm.dsym.sh
	ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./Distribute ./Archive.zip
	mv ./Archive.zip ./Distribute/`date +$(DIST1)-%Y-%m%d.zip`
	#if [ -e ./DistributeSL ]; then rm -r ./DistributeSL; fi
	#mkdir ./DistributeSL
	#cp -R ../slice.git/HWSensors4/Binaries/HWMonitor.app ./DistributeSL
	#ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./DistributeSL ./Archive.zip
	#mv ./Archive.zip ./DistributeSL/`date +$(DIST2)-%Y-%m%d.zip`
