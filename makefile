DIST1=RehabMan-FakeSMC
DIST2=RehabMan-SL-HWMonitor

ifeq ($(findstring 32,$(BITS)),32)
OPTIONS:=$(OPTIONS) -arch i386
endif

ifeq ($(findstring 64,$(BITS)),64)
OPTIONS:=$(OPTIONS) -arch x86_64
endif


.PHONY: all
all:
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Drivers" -configuration Debug
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Drivers" -configuration Release
	xcodebuild build -workspace HWSensors.xcworkspace -scheme "Build Only Apps" -configuration Debug
	xcodebuild build -workspace HWSensors.xcworkspace -scheme "Build Only Apps" -configuration Release

.PHONY: clean
clean:
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Drivers" -configuration Debug
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Drivers" -configuration Release
	xcodebuild clean -workspace HWSensors.xcworkspace -scheme "Build Only Apps" -configuration Debug
	xcodebuild clean -workspace HWSensors.xcworkspace -scheme "Build Only Apps" -configuration Release

.PHONY: install
install:
	sudo rm -rf /System/Library/Extensions/FakeSMC.kext
	sudo cp -R Build/Products/Release/FakeSMC.kext /System/Library/Extensions
	sudo mkdir /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/CPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/ACPISensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/LPCSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/GPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_probook
install_probook:
	sudo rm -rf /System/Library/Extensions/FakeSMC.kext
	sudo cp -R Build/Products/Release/FakeSMC.kext /System/Library/Extensions
	sudo mkdir /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/CPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Release/ACPISensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_debug
install_debug:
	sudo rm -rf /System/Library/Extensions/FakeSMC.kext
	sudo cp -R Build/Products/Debug/FakeSMC.kext /System/Library/Extensions
	sudo mkdir /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/CPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/ACPISensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/LPCSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/GPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_probook_debug
install_probook_debug:
	sudo rm -rf /System/Library/Extensions/FakeSMC.kext
	sudo cp -R Build/Products/Debug/FakeSMC.kext /System/Library/Extensions
	sudo mkdir /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/CPUSensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo cp -R Build/Products/Debug/ACPISensors.kext /System/Library/Extensions/FakeSMC.kext/Contents/PlugIns
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: distribute
distribute:
	if [ -e ./Distribute ]; then rm -r ./Distribute; fi
	mkdir ./Distribute
	mkdir ./Distribute/Release
	mkdir ./Distribute/Debug
	cp -R ./Build/Products/Release/FakeSMC.kext ./Distribute/Release
	mkdir ./Distribute/Release/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Release/CPUSensors.kext ./Distribute/Release/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Release/ACPISensors.kext ./Distribute/Release/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Release/LPCSensors.kext ./Distribute/Release/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Release/GPUSensors.kext ./Distribute/Release/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Release/HWMonitor.app ./Distribute/Release
	cp -R ./Build/Products/Debug/FakeSMC.kext ./Distribute/Debug
	mkdir ./Distribute/Debug/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Debug/CPUSensors.kext ./Distribute/Debug/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Debug/ACPISensors.kext ./Distribute/Debug/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Debug/LPCSensors.kext ./Distribute/Debug/FakeSMC.kext/Contents/PlugIns
	cp -R ./Build/Products/Debug/GPUSensors.kext ./Distribute/Debug/FakeSMC.kext/Contents/PlugIns
	#cp -R ./Build/Products/Debug/HWMonitor.app ./Distribute/Debug
	find ./Distribute -path *.DS_Store -delete
	find ./Distribute -path *.dSYM -exec echo rm -r {} \; >/tmp/org.kozlek.rm.dsym.sh
	chmod +x /tmp/org.kozlek.rm.dsym.sh
	/tmp/org.kozlek.rm.dsym.sh
	rm /tmp/org.kozlek.rm.dsym.sh
	ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./Distribute ./Archive.zip
	mv ./Archive.zip ./Distribute/`date +$(DIST1)-%Y-%m%d.zip`
	if [ -e ./DistributeSL ]; then rm -r ./DistributeSL; fi
	mkdir ./DistributeSL
	cp -R ../slice.git/HWSensors4/Binaries/HWMonitor.app ./DistributeSL
	ditto -c -k --sequesterRsrc --zlibCompressionLevel 9 ./DistributeSL ./Archive.zip
	mv ./Archive.zip ./DistributeSL/`date +$(DIST2)-%Y-%m%d.zip`
