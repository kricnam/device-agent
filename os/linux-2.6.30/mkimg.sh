#!/bin/sh
PRJ_PATH=/home/mxx/aloka
#TFTD_PATH=${PRJ_PATH}/bin
BIN_PATH=${PRJ_PATH}/bin
#KERNEL_PATH=${PRJ_PATH}/kernel


make ARCH=arm  CROSS_COMPILE=/usr/local/arm/4.3.2/bin/arm-none-linux-gnueabi-
# zImage
echo "creating zImage ..."
cp -fv  arch/arm/boot/zImage ${BIN_PATH}/zImage.2.6.30.bin
#cp -rfv  arch/arm/boot/zImage ${TFTD_PATH}

# uImage
echo "creating uImage ..."
./mkimage -A arm -O linux -C none -T kernel -a 20008000 -e 20008000 -n linux-2.6.30 -d arch/arm/boot/zImage uImage.bin
cp -fv  uImage.bin ${BIN_PATH}/uImage.2.6.30.bin
#cp -rfv  uImage ${TFTD_PATH}


# bakup script of making kernel image. 
#echo -n "bakup script to ${KERNEL_PATH} ..."
#cp -rfv mkimg.sh mkimage ${KERNEL_PATH}

