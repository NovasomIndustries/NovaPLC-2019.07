#!/bin/sh
REFERENCE_FILESYSTEM_P="/Devel/NOVAsdk/FileSystem/P_plc"
REFERENCE_FILESYSTEM_U5="/Devel/NOVAsdk/FileSystem/U5_Base"
if [ "$1" = "" ]; then
	echo ""
	echo "Select the board for which compile the NOVAplc application"
	echo " 1 : NOVAsomP"
	echo " 2 : NOVAsomU5"
	echo -n "Enter your selection : "
	read VAR
else
	if [ "$1" = "1" ]; then
		VAR="1";
	fi
	if [ "$1" = "2" ]; then
		VAR="2";
	fi
fi
if [ "$VAR" = "1" ]; then
        echo "You selected NOVAsomP"
	REFERENCE_FILESYSTEM=${REFERENCE_FILESYSTEM_P}
fi
if [ "$VAR" = "2" ]; then
        echo "You selected NOVAsomU5"
	REFERENCE_FILESYSTEM=${REFERENCE_FILESYSTEM_U5}
fi
! [ -d tools ] && mkdir tools
ARMGCC="${REFERENCE_FILESYSTEM}/output/host/bin/arm-linux-g++ -std=gnu++11"
if ! [ -d ${REFERENCE_FILESYSTEM} ]; then
	echo "Please select an existing file system as base file system"
	exit 1
fi
if ! [ -f tools/iec2c ]; then
	echo "Compiling matiec"
	cd matiec
	./configure
	make
	cp iec2c ../tools/.
	cd ..
else
	echo "matiec exists, skip compilation"
fi

cd src
! [ -f ../tools/st_optimizer ] && g++ st_optimizer.cpp -o ../tools/st_optimizer >/dev/null 2>&1
! [ -f ../tools/glue_generator ] && g++ glue_generator.cpp -o ../tools/glue_generator >/dev/null 2>&1
cd ..

cd core
echo "Generating executables ... "
../tools/st_optimizer ../st/st_file.st ../st/out.st >/dev/null 2>&1
../tools/iec2c -I ../lib ../st/out.st >/dev/null 2>&1
ARCH=arm ${ARMGCC} -I./lib -c Config0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal >/dev/null 2>&1
ARCH=arm ${ARMGCC} -I./lib -c Res0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal >/dev/null 2>&1
../tools/glue_generator
ARCH=arm ${ARMGCC} *.cpp *.o -o ../novaplc -I./lib -lrt -lpthread -lmodbus -fpermissive -I${REFERENCE_FILESYSTEM}/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include/modbus
cd ..
ARCH=arm ${ARMGCC} writefifo.c -o  writefifo >/dev/null 2>&1
echo "Done"
