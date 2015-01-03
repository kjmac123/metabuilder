BUILDCONFIG := Release


default : all 
metabuilder_MODULEOBJ := 
include lua.mk
metabuilder_MODULEOBJ += $(lua_OUTDIR)/lua

metabuilder_INTDIR := int/metabuilder/$(BUILDCONFIG)
metabuilder_OUTDIR := out/metabuilder/$(BUILDCONFIG)

metabuilder_CC := gcc
metabuilder_CXX := g++
metabuilder_CPPFLAGS.Debug := \
  -Wall -g -O0 \

metabuilder_CFLAGS.Debug := \

metabuilder_CXXFLAGS.Debug := \
  -Wall \
  -Wno-unused-function \

metabuilder_DEFINES.Debug := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_DEBUG \

metabuilder_CPPFLAGS.Debug += $(metabuilder_DEFINES.Debug)
metabuilder_INCLUDES.Debug := \
  -I"../../../src" \
  -I"../../../../external/dlmalloc-2.8.6" \
  -I"../../../../external/ezOptionParser-0.2.1" \
  -I"../../../../external/lua-5.2.2/src" \
  -I"../../../../external" \

metabuilder_CPPFLAGS.Debug += $(metabuilder_INCLUDES.Debug)
metabuilder_CPPFLAGS.Release := \
  -Wall -g -O2 \

metabuilder_CFLAGS.Release := \

metabuilder_CXXFLAGS.Release := \
  -Wall \
  -Wno-unused-function \

metabuilder_DEFINES.Release := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_RELEASE \

metabuilder_CPPFLAGS.Release += $(metabuilder_DEFINES.Release)
metabuilder_INCLUDES.Release := \
  -I"../../../src" \
  -I"../../../../external/dlmalloc-2.8.6" \
  -I"../../../../external/ezOptionParser-0.2.1" \
  -I"../../../../external/lua-5.2.2/src" \
  -I"../../../../external" \

metabuilder_CPPFLAGS.Release += $(metabuilder_INCLUDES.Release)
metabuilder_CPPFLAGS	:= -MMD -MP $(metabuilder_CPPFLAGS.$(BUILDCONFIG))
metabuilder_CFLAGS	:= $(metabuilder_CFLAGS.$(BUILDCONFIG))
metabuilder_CXXFLAGS	:= $(metabuilder_CXXFLAGS.$(BUILDCONFIG))
metabuilder_CFLAGS += -c
metabuilder_CXXFLAGS += -c
metabuilder_LD := g++

metabuilder_LDFLAGS.Debug := \
 \

metabuilder_LIBDIRS.Debug := \

metabuilder_LDFLAGS.Debug += $(metabuilder_LIBDIRS.Debug)
metabuilder_LIBS.Debug := \
 -lrt \

metabuilder_LDLIBS.Debug += $(metabuilder_LIBS.Debug)

metabuilder_LDFLAGS.Release := \
 \

metabuilder_LIBDIRS.Release := \

metabuilder_LDFLAGS.Release += $(metabuilder_LIBDIRS.Release)
metabuilder_LIBS.Release := \
 -lrt \

metabuilder_LDLIBS.Release += $(metabuilder_LIBS.Release)

metabuilder_LDFLAGS := $(metabuilder_LDFLAGS.$(BUILDCONFIG)) $(metabuilder_LDLIBS.$(BUILDCONFIG))

metabuilder_SRC := \
	../../../../external/dlmalloc-2.8.6/dlmalloc.cpp \
	../../../../external/freebsd/realpath.cpp \
	../../../src/block.cpp \
	../../../src/common.cpp \
	../../../src/configparam.cpp \
	../../../src/core.cpp \
	../../../src/luafile.cpp \
	../../../src/main.cpp \
	../../../src/makeglobal.cpp \
	../../../src/makesetup.cpp \
	../../../src/metabase.cpp \
	../../../src/metabuilder_pch.cpp \
	../../../src/platform/platform.cpp \
	../../../src/platform/posix/platform_posix.cpp \
	../../../src/platformparam.cpp \
	../../../src/solution.cpp \
	../../../src/target.cpp \
	../../../src/timeutil.cpp \
	../../../src/writer.cpp \
	../../../src/writer_msvc.cpp \
	../../../src/writer_utility.cpp \
	../../../src/writer_xcode.cpp \

metabuilder_OBJ := \
	$(metabuilder_INTDIR)/dlmalloc.o \
	$(metabuilder_INTDIR)/realpath.o \
	$(metabuilder_INTDIR)/block.o \
	$(metabuilder_INTDIR)/common.o \
	$(metabuilder_INTDIR)/configparam.o \
	$(metabuilder_INTDIR)/core.o \
	$(metabuilder_INTDIR)/luafile.o \
	$(metabuilder_INTDIR)/main.o \
	$(metabuilder_INTDIR)/makeglobal.o \
	$(metabuilder_INTDIR)/makesetup.o \
	$(metabuilder_INTDIR)/metabase.o \
	$(metabuilder_INTDIR)/metabuilder_pch.o \
	$(metabuilder_INTDIR)/platform.o \
	$(metabuilder_INTDIR)/platform_posix.o \
	$(metabuilder_INTDIR)/platformparam.o \
	$(metabuilder_INTDIR)/solution.o \
	$(metabuilder_INTDIR)/target.o \
	$(metabuilder_INTDIR)/timeutil.o \
	$(metabuilder_INTDIR)/writer.o \
	$(metabuilder_INTDIR)/writer_msvc.o \
	$(metabuilder_INTDIR)/writer_utility.o \
	$(metabuilder_INTDIR)/writer_xcode.o \

$(metabuilder_INTDIR)/dlmalloc.o : ../../../../external/dlmalloc-2.8.6/dlmalloc.cpp $(metabuilder_INTDIR)/dlmalloc.d 
	@echo $(metabuilder_CXX) ../../../../external/dlmalloc-2.8.6/dlmalloc.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/realpath.o : ../../../../external/freebsd/realpath.cpp $(metabuilder_INTDIR)/realpath.d 
	@echo $(metabuilder_CXX) ../../../../external/freebsd/realpath.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/block.o : ../../../src/block.cpp $(metabuilder_INTDIR)/block.d 
	@echo $(metabuilder_CXX) ../../../src/block.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/common.o : ../../../src/common.cpp $(metabuilder_INTDIR)/common.d 
	@echo $(metabuilder_CXX) ../../../src/common.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/configparam.o : ../../../src/configparam.cpp $(metabuilder_INTDIR)/configparam.d 
	@echo $(metabuilder_CXX) ../../../src/configparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/core.o : ../../../src/core.cpp $(metabuilder_INTDIR)/core.d 
	@echo $(metabuilder_CXX) ../../../src/core.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/luafile.o : ../../../src/luafile.cpp $(metabuilder_INTDIR)/luafile.d 
	@echo $(metabuilder_CXX) ../../../src/luafile.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/main.o : ../../../src/main.cpp $(metabuilder_INTDIR)/main.d 
	@echo $(metabuilder_CXX) ../../../src/main.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/makeglobal.o : ../../../src/makeglobal.cpp $(metabuilder_INTDIR)/makeglobal.d 
	@echo $(metabuilder_CXX) ../../../src/makeglobal.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/makesetup.o : ../../../src/makesetup.cpp $(metabuilder_INTDIR)/makesetup.d 
	@echo $(metabuilder_CXX) ../../../src/makesetup.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/metabase.o : ../../../src/metabase.cpp $(metabuilder_INTDIR)/metabase.d 
	@echo $(metabuilder_CXX) ../../../src/metabase.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/metabuilder_pch.o : ../../../src/metabuilder_pch.cpp $(metabuilder_INTDIR)/metabuilder_pch.d 
	@echo $(metabuilder_CXX) ../../../src/metabuilder_pch.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platform.o : ../../../src/platform/platform.cpp $(metabuilder_INTDIR)/platform.d 
	@echo $(metabuilder_CXX) ../../../src/platform/platform.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platform_posix.o : ../../../src/platform/posix/platform_posix.cpp $(metabuilder_INTDIR)/platform_posix.d 
	@echo $(metabuilder_CXX) ../../../src/platform/posix/platform_posix.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platformparam.o : ../../../src/platformparam.cpp $(metabuilder_INTDIR)/platformparam.d 
	@echo $(metabuilder_CXX) ../../../src/platformparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/solution.o : ../../../src/solution.cpp $(metabuilder_INTDIR)/solution.d 
	@echo $(metabuilder_CXX) ../../../src/solution.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/target.o : ../../../src/target.cpp $(metabuilder_INTDIR)/target.d 
	@echo $(metabuilder_CXX) ../../../src/target.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/timeutil.o : ../../../src/timeutil.cpp $(metabuilder_INTDIR)/timeutil.d 
	@echo $(metabuilder_CXX) ../../../src/timeutil.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer.o : ../../../src/writer.cpp $(metabuilder_INTDIR)/writer.d 
	@echo $(metabuilder_CXX) ../../../src/writer.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_msvc.o : ../../../src/writer_msvc.cpp $(metabuilder_INTDIR)/writer_msvc.d 
	@echo $(metabuilder_CXX) ../../../src/writer_msvc.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_utility.o : ../../../src/writer_utility.cpp $(metabuilder_INTDIR)/writer_utility.d 
	@echo $(metabuilder_CXX) ../../../src/writer_utility.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_xcode.o : ../../../src/writer_xcode.cpp $(metabuilder_INTDIR)/writer_xcode.d 
	@echo $(metabuilder_CXX) ../../../src/writer_xcode.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 

$(metabuilder_INTDIR)/__metabuilder__prelink__ : $(metabuilder_OBJ) $(lua_OUTDIR)/lua 
	@echo ld Creating prelink obj $(metabuilder_INTDIR)/__metabuilder__prelink__
	@ld  -r  $(metabuilder_OBJ) -o '$@' ;

$(metabuilder_OUTDIR)/metabuilder : $(metabuilder_INTDIR)/__metabuilder__prelink__
	@echo $(metabuilder_LD) Linking $(metabuilder_OUTDIR)/metabuilder
	@$(metabuilder_LD) $(metabuilder_LDFLAGS) $(metabuilder_MODULEOBJ) $(metabuilder_OBJ) -o '$@' ;


$(metabuilder_OBJ) : | $(metabuilder_INTDIR)

$(metabuilder_INTDIR):
	mkdir -p $(metabuilder_INTDIR)
	mkdir -p $(metabuilder_OUTDIR)

.PHONY: all_metabuilder
all_metabuilder : $(metabuilder_OUTDIR)/metabuilder 

.PHONY: clean_metabuilder
clean_metabuilder : clean_lua 
	@echo Cleaning $(metabuilder_INTDIR)
	@rm -f "$(metabuilder_INTDIR)"/*
	@if [ -d "$(metabuilder_INTDIR)" ]; then rmdir "$(metabuilder_INTDIR)";fi
	@echo Cleaning $(metabuilder_OUTDIR)
	@rm -f "$(metabuilder_OUTDIR)"/*
	@if [ -d "$(metabuilder_OUTDIR)" ]; then rmdir "$(metabuilder_OUTDIR)";fi

.PHONY: all
all : all_metabuilder


.PHONY: clean
clean : clean_metabuilder

%.d: ;
-include $(metabuilder_INTDIR)/dlmalloc.d
-include $(metabuilder_INTDIR)/realpath.d
-include $(metabuilder_INTDIR)/block.d
-include $(metabuilder_INTDIR)/common.d
-include $(metabuilder_INTDIR)/configparam.d
-include $(metabuilder_INTDIR)/core.d
-include $(metabuilder_INTDIR)/luafile.d
-include $(metabuilder_INTDIR)/main.d
-include $(metabuilder_INTDIR)/makeglobal.d
-include $(metabuilder_INTDIR)/makesetup.d
-include $(metabuilder_INTDIR)/metabase.d
-include $(metabuilder_INTDIR)/metabuilder_pch.d
-include $(metabuilder_INTDIR)/platform.d
-include $(metabuilder_INTDIR)/platform_posix.d
-include $(metabuilder_INTDIR)/platformparam.d
-include $(metabuilder_INTDIR)/solution.d
-include $(metabuilder_INTDIR)/target.d
-include $(metabuilder_INTDIR)/timeutil.d
-include $(metabuilder_INTDIR)/writer.d
-include $(metabuilder_INTDIR)/writer_msvc.d
-include $(metabuilder_INTDIR)/writer_utility.d
-include $(metabuilder_INTDIR)/writer_xcode.d

