TOP_DIR   := ..
OBJ_DIR   := .
OBJ_BUILD := $(OBJ_DIR)
DOC_DIR   := $(OBJ_DIR)/reference
LIBTOOL   := $(OBJ_DIR)/libtool
ifndef FT2DEMOS
  include $(TOP_DIR)/Makefile
else
  TOP_DIR_2 := $(TOP_DIR)/../ft2demos
  PROJECT   := freetype
  CONFIG_MK := $(OBJ_DIR)/config.mk
  include $(TOP_DIR_2)/Makefile
endif
