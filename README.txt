U-MEDIA Embedded Linux for SSD1935 README

1. Directory structure
	Under the package root, refered as BMLXBASE, there are 8 major 
	subdirectories:
	[BSP]		contains all BSP drivers
	[apps]		contains all oss/non-oss applications
	[configs]	kernel/uClibc/busybox and system's default config
	[image]		empty one, for storing final image package
	[kernel]	Linux kernel
	[libs]		all oss/non-oss libraries
	[target]	target application's installation path
	[rootdisk]	embedded linux's root disk
	[toolchain]	source code or vendor provided toolchain
	[tools]		some helper scripts or tools here
	[nboot]		SolomanSystech's IPL bootloader + uboot

2. Package build process
	There is a shell script under EMLXBASE, "build_all.sh" is used to generate
	the target embedded Linux package. To build the whole system, type:
	
		# sh ./build_all.sh
		
	This will compile all source codes and put the result in EMLXBASE/image.
	Four files are generated: uImage, rootdisk.cramfs , app.ext2 and emlx-x.x.x.x.img. 
	
	Buid_all.sh also supports partial build process:
	
		# sh ./build_all.sh kernel
			-> Only build kernel tree and pack the f/w image
		# sh ./build_all.sh libs
			-> Only build libraries tree and pack the f/w image	
		# sh ./build_all.sh apps
			-> Only build applications tree and pack the f/w image
		# sh ./build_all.sh image

	Another environment varable can be provided to select product while building:
	EMLX_PROFILE:

		# export EMLX_PROFILE=TEAC_WAP8900
		# sh ./build_all.sh

	This will build a EMLX package for TEAC WAP8900. All profiles can be found under
	[EMLX]/configs/profiles
	
3. Configuration modification
	[1] System-wide configuration
		System-wide configurations are stored in emlinux-ssd1933/configs/env
		
	[2] Flash partitions mapping
		Please check MTD driver for detail
	
	[3] Kernel/BusyBox
		Default configurations are under EMLXBASE/configs, they will be copied 
		to kernel/uClibc/busybox's directories for compiling at the first time.
		There are several commands supported in build_all.sh to do 
		configurations:
		
		# sh ./build_all.sh kernelconfig -> to configure kernel
		
		# sh ./build_all.sh busyboxconfig -> to configure busybox
		
4. Firmware loading
	For the first time, you need to program the flash by SolomonSystech's DeveloperTool.
	Find it under [tools] directory and follow its instruction.

	EMLX for SSD1933's flash partition map is a little different from original one:

	Bootloader: 0~1 MB (block 0 ~ 1)
	Kernel    : 1~4 MB (block 2 ~ 7)
	root disk : 4~12MB (block 8 ~ 23) <- this is different from SolomonSystech's definition.
		
