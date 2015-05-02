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
	xcodebuild build $(OPTIONS) -project Sparkle/Sparkle.xcodeproj -target "Sparkle" -configuration Release
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Debug
	xcodebuild build $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Release
	xcodebuild build -workspace HWSensors.xcworkspace -scheme "Build Apps" -configuration Debug
	xcodebuild build -workspace HWSensors.xcworkspace -scheme "Build Apps" -configuration Release

.PHONY: clean
clean:
	xcodebuild clean $(OPTIONS) -project Sparkle/Sparkle.xcodeproj -target "Sparkle" -configuration Release
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Debug
	xcodebuild clean $(OPTIONS) -workspace HWSensors.xcworkspace -scheme "Build Kexts" -configuration Release
	xcodebuild clean -workspace HWSensors.xcworkspace -scheme "Build Apps" -configuration Debug
	xcodebuild clean -workspace HWSensors.xcworkspace -scheme "Build Apps" -configuration Release

.PHONY: install
install:
	sudo rm -rf /System/Library/Extensions/FakeSMC*.kext
	sudo cp -R Build/Products/Release/FakeSMC.kext /System/Library/Extensions
	sudo cp -R Build/Products/Release/CPUSensors.kext /System/Library/Extensions/FakeSMC_CPUSensors.kext
	sudo cp -R Build/Products/Release/ACPISensors.kext /System/Library/Extensions/FakeSMC_ACPISensors.kext
	sudo cp -R Build/Products/Release/LPCSensors.kext /System/Library/Extensions/FakeSMC_LPCSensors.kext
	sudo cp -R Build/Products/Release/GPUSensors.kext /System/Library/Extensions/FakeSMC_GPUSensors.kext
	if [ "`which tag`" != "" ]; then sudo tag -a Blue /System/Library/Extensions/FakeSMC*.kext; fi
	sudo touch /System/Library/Extensions
	sudo kextcache -update-volume /

.PHONY: install_debug
install_debug:
	sudo rm -rf /System/Library/Extensions/FakeSMC*.kext
	sudo cp -R Build/Products/Debug/FakeSMC.kext /System/Library/Extensions
	sudo cp -R Build/Products/Debug/CPUSensors.kext /System/Library/Extensions/FakeSMC_CPUSensors.kext
	sudo cp -R Build/Products/Debug/ACPISensors.kext /System/Library/Extensions/FakeSMC_ACPISensors.kext
	sudo cp -R Build/Products/Debug/LPCSensors.kext /System/Library/Extensions/FakeSMC_LPCSensors.kext
	sudo cp -R Build/Products/Debug/GPUSensors.kext /System/Library/Extensions/FakeSMC_GPUSensors.kext
	if [ "`which tag`" != "" ]; then sudo tag -a Purple /System/Library/Extensions/FakeSMC*.kext; fi
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
