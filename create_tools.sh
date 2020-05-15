#!/bin/sh
REFERENCE_FILESYSTEM_P="/Devel/NOVAsdk/FileSystem/P_plc"
REFERENCE_FILESYSTEM_U5="/Devel/NOVAsdk/FileSystem/U5_Base"
REFERENCE_FILESYSTEM_M7="/Devel/NOVAsdk/FileSystem/M7_PLC"
if [ "$1" = "" ]; then
	echo ""
	echo "Select the board for which compile the NOVAplc application"
	echo " 1 : NOVAsomP"
	echo " 2 : NOVAsomU5"
	echo " 3 : NOVAsomM7"
	echo -n "Enter your selection : "
	read VAR
else
	if [ "$1" = "1" ]; then
		VAR="1";
	fi
	if [ "$1" = "2" ]; then
		VAR="2";
	fi
	if [ "$1" = "3" ]; then
		VAR="3";
	fi
fi
if [ "$VAR" = "1" ]; then
        echo "You selected NOVAsomP"
	REFERENCE_FILESYSTEM=${REFERENCE_FILESYSTEM_P}
	ARMGCC="${REFERENCE_FILESYSTEM}/output/host/bin/arm-linux-g++ -std=gnu++11"
	TARGET_ARC="arm"
	BOARD_TYPE="NOVASOM_P"
fi
if [ "$VAR" = "2" ]; then
        echo "You selected NOVAsomU5"
	REFERENCE_FILESYSTEM=${REFERENCE_FILESYSTEM_U5}
	ARMGCC="${REFERENCE_FILESYSTEM}/output/host/bin/arm-linux-g++ -std=gnu++11"
	TARGET_ARC="arm"
	BOARD_TYPE="NOVASOM_U5"
fi
if [ "$VAR" = "3" ]; then
        echo "You selected NOVAsomM7"
	REFERENCE_FILESYSTEM=${REFERENCE_FILESYSTEM_M7}
	ARMGCC="${REFERENCE_FILESYSTEM}/output/host/bin/aarch64-linux-g++ -std=gnu++11"
	TARGET_ARC="arm64"
	BOARD_TYPE="NOVASOM_M7"
fi
! [ -d tools ] && mkdir tools
if ! [ -d ${REFERENCE_FILESYSTEM} ]; then
	echo "Please select an existing file system as base file system"
	exit 1
fi
if ! [ -f ${REFERENCE_FILESYSTEM}/output/host/bin/aarch64-linux-g++ ]; then
	echo "Please select a compiled file system with g++ compiler"
	echo "${REFERENCE_FILESYSTEM}//output/host/bin/aarch64-linux-g++"
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
ARCH=${TARGET_ARC} ${ARMGCC} -I./lib -c Config0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal >/dev/null 2>&1
ARCH=${TARGET_ARC} ${ARMGCC} -I./lib -c Res0.c -lasiodnp3 -lasiopal -lopendnp3 -lopenpal >/dev/null 2>&1
../tools/glue_generator
echo "ARCH=${TARGET_ARC} ${ARMGCC} *.cpp *.o -o ../novaplc -D${BOARD_TYPE} -I./lib -lrt -lpthread -lmodbus -fpermissive -I${REFERENCE_FILESYSTEM}/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include/modbus"
ARCH=${TARGET_ARC} ${ARMGCC} *.cpp *.o -o ../novaplc -D${BOARD_TYPE} -I./lib -lrt -lpthread -lmodbus -fpermissive -I${REFERENCE_FILESYSTEM}/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include/modbus
cd ..
ARCH=${TARGET_ARC} ${ARMGCC} writefifo.c -o  writefifo >/dev/null 2>&1
echo "Done"
