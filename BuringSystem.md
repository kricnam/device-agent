# Step by step #

  1. SW1,SW2 off, and power on, connect to the USB device port
  1. start SAM-BA program, select board type(at91sam9261ek) and port(/dev/ttyUSB1)
  1. select Nandflash tab
  1. select Enable Nandflash script and Excute
  1. {option) execute Erase All script
  1. select SendBootFile, and Excute, select the BootStrap file
  1. send U-Boot bin file for Nandflash to address 0x20000
  1. send linux uImage bin file to nand flash address 0xA0000
  1. send root file system ram-disk image(gziped) to nand flash address 0x400000
  1. SW1 on, and select Dataflash tab
  1. excute EnableDataflash script
  1. (optional) excute EraseAll script
  1. excute SendBootFile script
  1. send U-Boot bin file for dataflash tp address 0x8400
  1. send linux uImage bin file to data-flash address 0x42000
  1. type exit to quit SAM-BA
  1. now can restart the board