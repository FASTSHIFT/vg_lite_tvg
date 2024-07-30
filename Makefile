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

CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/external/thorvg/thorvg/inc
CFLAGS   += ${INCDIR_PREFIX}$(APPDIR)/apps/graphics/libyuv/libyuv/include
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/external/thorvg/thorvg/inc
CXXFLAGS += ${INCDIR_PREFIX}$(APPDIR)/apps/graphics/libyuv/libyuv/include

CSRCS   += $(wildcard *.c)
CXXSRCS += $(wildcard *.cpp)

CFLAGS   += -DLV_USE_VG_LITE_MAIN=0 -DLV_VG_LITE_THORVG_USE_RELEASE=1
CXXFLAGS += -DLV_USE_VG_LITE_MAIN=0 -DLV_VG_LITE_THORVG_USE_RELEASE=1

include $(APPDIR)/Application.mk
