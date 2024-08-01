############################################################################
# apps/graphics/vg_lite_tvg/Makefile
#
# Licensed to the Apache Software Foundation (ASF) under one or more
# contributor license agreements.  See the NOTICE file distributed with
# this work for additional information regarding copyright ownership.  The
# ASF licenses this file to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance with the
# License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
# License for the specific language governing permissions and limitations
# under the License.
#
############################################################################

include $(APPDIR)/Make.defs

CXXEXT   := .cpp

CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/external/libpng
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/external/libpng/libpng

ifneq ($(CONFIG_LIBYUV),)
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/graphics/libyuv/libyuv/include
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/graphics/libyuv/libyuv/include

CFLAGS   += -DLV_VG_LITE_THORVG_YUV_SUPPORT=1
CXXFLAGS += -DLV_VG_LITE_THORVG_YUV_SUPPORT=1
endif

ifneq ($(CONFIG_VG_LITE_EXAMPLE_INCLUDE), "")
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/../$(CONFIG_VG_LITE_EXAMPLE_INCLUDE)
else
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/graphics/vg_lite_tvg
endif

ifneq ($(CONFIG_VG_LITE_EXAMPLE_CUSTOM_INIT),)
CFLAGS   += -DLV_VG_LITE_USE_GPU_INIT=1

ifneq ($(CONFIG_ARCH_SIM),)
# simulators need re-initialization before rendering
CFLAGS   += -DLV_VG_LITE_USE_GPU_INIT_ONCE=0
else
CFLAGS   += -DLV_VG_LITE_USE_GPU_INIT_ONCE=1
endif

endif

ifneq ($(CONFIG_VG_LITE_SIM),)
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/external/thorvg/thorvg/inc
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/thorvg/thorvg/inc

CFLAGS   += -DLV_VG_LITE_THORVG_USE_RELEASE=1
CXXFLAGS += -DLV_VG_LITE_THORVG_USE_RELEASE=1

CSRCS    += $(wildcard *.c)
CXXSRCS  += $(wildcard *.cpp)
endif

CFLAGS   += -DLV_USE_VG_LITE_MAIN=1
CXXFLAGS += -DLV_USE_VG_LITE_MAIN=1

PROGNAME = vg_lite
PRIORITY = $(CONFIG_VG_LITE_EXAMPLE_PRIORITY)
STACKSIZE = $(CONFIG_VG_LITE_EXAMPLE_STACKSIZE)
MODULE = $(CONFIG_GRAPHICS_VG_LITE_TVG)

MAINSRC = example/vg_lite_main.c

include $(APPDIR)/Application.mk
