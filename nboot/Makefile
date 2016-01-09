#
# nboot/Makefile
# creator: Cody Liang
# creation date: 10 July 2008
#

board ?=
accio ?=
bpp ?=

ifeq ($(board), cm5208)
accio := y
endif
ifeq ($(board), we8623_p0)
accio := y
endif
ifeq ($(board), accio_p1)
accio := y
endif
ifeq ($(board), accio_p0)
accio := y
endif
ifeq ($(board), a2818t)
accio := y
endif
ifeq ($(board), cm5210)
accio := y
endif

default:
ifeq ($(board), cm5208)
	@echo "#define CONFIG_CM5208 1" > include/config.h
	@make -C u-boot cm5208_config
endif

ifeq ($(board), we8623_p0)
	@echo "#define CONFIG_WE8623_P0 1" > include/config.h
	@echo "#define CONFIG_CPT480X272 1" >> include/config.h
	@make -C u-boot we8623_config
endif

ifeq ($(board), accio_p1)
	@echo "#define CONFIG_ACCIO_P1 1" > include/config.h
ifeq ($(platform), sk01)
	@echo "#define CONFIG_ACCIO_P1_SK01 1" >> include/config.h
endif
ifeq ($(platform), lite)
	@echo "#define CONFIG_ACCIO_P1_LITE	1" >> include/config.h
endif
	@make -C u-boot accio_config
endif

ifeq ($(board), accio_p0)
	@echo "#define CONFIG_ACCIO_P0 1" > include/config.h
	@make -C u-boot accio_config
endif

ifeq ($(board), a2818t)
	@echo "#define CONFIG_A2818T 1" > include/config.h
	@make -C u-boot a2818t_config
endif

ifeq ($(board), cm5210)
	@echo "#define CONFIG_A2818T 1" > include/config.h
	@make -C u-boot a2818t_config
endif

ifneq ($(bpp), 16)
	@echo "#define CONFIG_BPP32 1" >> include/config.h
endif

ifdef ddr
	@echo "#define DDR_TUNE 	1" >> include/config.h
endif

ifneq ($(accio), y)
	@echo "------------------------------------------------------------"
	@echo "Usage (Default bpp=16):"
	@echo "	make clean"
	@echo "	make board=<accio_p1|accio_p0|cm5208|a2818t|cm5210|we8623_p0> [platform=<sk01|lite>] [bpp=<32|16>] [ddr=1]"
	@echo "	sk01 for semisky|lite for accio_lite, bpp for LCD 32B or 16B, ddr for ddrtune"
	@echo "------------------------------------------------------------"
	@exit 1
endif
	make -C ipl
ifdef ddr
	make -C ddrtune
endif
	make -C u-boot
	make -C tool
ifdef ddr
	tool/mknboot -i ipl/ipl.bin -s u-boot/u-boot.bin -d ddrtune/ddrtune.bin
else
	tool/mknboot -i ipl/ipl.bin -s u-boot/u-boot.bin
endif
clean:
	make -C ipl clean
	make -C ddrtune clean
	make -C u-boot distclean
	make -C tool clean
	rm -f nboot.bin include/config.h
