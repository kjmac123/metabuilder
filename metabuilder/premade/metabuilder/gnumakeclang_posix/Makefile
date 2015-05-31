BUILDCONFIG := Release


default : all 
metabuilder_MODULEOBJ := 
include lua.mk
metabuilder_MODULEOBJ += $(lua_OUTDIR.$(BUILDCONFIG))/lua

metabuilder_CC := clang
metabuilder_CXX := clang++

metabuilder_INTDIR.Debug := int/metabuilder/Debug
metabuilder_OUTDIR.Debug := out/metabuilder/Debug

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
metabuilder_INTDIR.Release := int/metabuilder/Release
metabuilder_OUTDIR.Release := out/metabuilder/Release

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
metabuilder_LD := clang++

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
	../../../../external/freebsd/realpath.cpp \
	../../../src/block.cpp \
	../../../src/common.cpp \
	../../../src/configparam.cpp \
	../../../src/core.cpp \
	../../../src/corestring.cpp \
	../../../src/filepath.cpp \
	../../../src/luafile.cpp \
	../../../src/main.cpp \
	../../../src/makeglobal.cpp \
	../../../src/mbstring.cpp \
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
	$(metabuilder_INTDIR.$(BUILDCONFIG))/realpath.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/block.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/common.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/configparam.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/core.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/corestring.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/filepath.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/luafile.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/main.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/makeglobal.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/mbstring.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/metabase.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/metabuilder_pch.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/platform.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/platform_posix.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/platformparam.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/solution.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/target.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/timeutil.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/writer.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_msvc.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_utility.o \
	$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_xcode.o \

$(metabuilder_INTDIR.$(BUILDCONFIG))/realpath.o : ../../../../external/freebsd/realpath.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/realpath.d 
	@echo $(metabuilder_CXX) ../../../../external/freebsd/realpath.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/block.o : ../../../src/block.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/block.d 
	@echo $(metabuilder_CXX) ../../../src/block.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/common.o : ../../../src/common.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/common.d 
	@echo $(metabuilder_CXX) ../../../src/common.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/configparam.o : ../../../src/configparam.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/configparam.d 
	@echo $(metabuilder_CXX) ../../../src/configparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/core.o : ../../../src/core.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/core.d 
	@echo $(metabuilder_CXX) ../../../src/core.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/corestring.o : ../../../src/corestring.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/corestring.d 
	@echo $(metabuilder_CXX) ../../../src/corestring.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/filepath.o : ../../../src/filepath.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/filepath.d 
	@echo $(metabuilder_CXX) ../../../src/filepath.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/luafile.o : ../../../src/luafile.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/luafile.d 
	@echo $(metabuilder_CXX) ../../../src/luafile.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/main.o : ../../../src/main.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/main.d 
	@echo $(metabuilder_CXX) ../../../src/main.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/makeglobal.o : ../../../src/makeglobal.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/makeglobal.d 
	@echo $(metabuilder_CXX) ../../../src/makeglobal.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/mbstring.o : ../../../src/mbstring.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/mbstring.d 
	@echo $(metabuilder_CXX) ../../../src/mbstring.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/metabase.o : ../../../src/metabase.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/metabase.d 
	@echo $(metabuilder_CXX) ../../../src/metabase.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/metabuilder_pch.o : ../../../src/metabuilder_pch.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/metabuilder_pch.d 
	@echo $(metabuilder_CXX) ../../../src/metabuilder_pch.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/platform.o : ../../../src/platform/platform.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/platform.d 
	@echo $(metabuilder_CXX) ../../../src/platform/platform.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/platform_posix.o : ../../../src/platform/posix/platform_posix.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/platform_posix.d 
	@echo $(metabuilder_CXX) ../../../src/platform/posix/platform_posix.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/platformparam.o : ../../../src/platformparam.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/platformparam.d 
	@echo $(metabuilder_CXX) ../../../src/platformparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/solution.o : ../../../src/solution.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/solution.d 
	@echo $(metabuilder_CXX) ../../../src/solution.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/target.o : ../../../src/target.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/target.d 
	@echo $(metabuilder_CXX) ../../../src/target.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/timeutil.o : ../../../src/timeutil.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/timeutil.d 
	@echo $(metabuilder_CXX) ../../../src/timeutil.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/writer.o : ../../../src/writer.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/writer.d 
	@echo $(metabuilder_CXX) ../../../src/writer.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_msvc.o : ../../../src/writer_msvc.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_msvc.d 
	@echo $(metabuilder_CXX) ../../../src/writer_msvc.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_utility.o : ../../../src/writer_utility.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_utility.d 
	@echo $(metabuilder_CXX) ../../../src/writer_utility.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR.$(BUILDCONFIG))/writer_xcode.o : ../../../src/writer_xcode.cpp $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_xcode.d 
	@echo $(metabuilder_CXX) ../../../src/writer_xcode.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 

$(metabuilder_INTDIR.$(BUILDCONFIG))/__metabuilder__prelink__ : $(metabuilder_OBJ) $(lua_OUTDIR.$(BUILDCONFIG))/lua 
	@echo ld Creating prelink obj $(metabuilder_INTDIR.$(BUILDCONFIG))/__metabuilder__prelink__
	@ld  -r  $(metabuilder_OBJ) -o '$@' ;

$(metabuilder_OUTDIR.$(BUILDCONFIG))/metabuilder : $(metabuilder_INTDIR.$(BUILDCONFIG))/__metabuilder__prelink__
	@echo $(metabuilder_LD) Linking $(metabuilder_OUTDIR.$(BUILDCONFIG))/metabuilder
	@$(metabuilder_LD) $(metabuilder_MODULEOBJ) $(metabuilder_OBJ) $(metabuilder_LDFLAGS) -o '$@' ;


$(metabuilder_OBJ) : | $(metabuilder_INTDIR.$(BUILDCONFIG))

$(metabuilder_INTDIR.$(BUILDCONFIG)):
	mkdir -p $(metabuilder_INTDIR.$(BUILDCONFIG))
	mkdir -p $(metabuilder_OUTDIR.$(BUILDCONFIG))

.PHONY: all_metabuilder
all_metabuilder : $(metabuilder_OUTDIR.$(BUILDCONFIG))/metabuilder 

.PHONY: clean_metabuilder
clean_metabuilder : clean_lua 
	@echo Cleaning $(metabuilder_INTDIR.$(BUILDCONFIG))
	@rm -f "$(metabuilder_INTDIR.$(BUILDCONFIG))"/*
	@if [ -d "$(metabuilder_INTDIR.$(BUILDCONFIG))" ]; then rmdir "$(metabuilder_INTDIR.$(BUILDCONFIG))";fi
	@echo Cleaning $(metabuilder_OUTDIR.$(BUILDCONFIG))
	@rm -f "$(metabuilder_OUTDIR.$(BUILDCONFIG))"/*
	@if [ -d "$(metabuilder_OUTDIR.$(BUILDCONFIG))" ]; then rmdir "$(metabuilder_OUTDIR.$(BUILDCONFIG))";fi

.PHONY: all
all : all_metabuilder


.PHONY: clean
clean : clean_metabuilder

%.d: ;
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/realpath.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/block.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/common.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/configparam.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/core.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/corestring.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/filepath.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/luafile.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/main.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/makeglobal.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/mbstring.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/metabase.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/metabuilder_pch.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/platform.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/platform_posix.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/platformparam.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/solution.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/target.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/timeutil.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/writer.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_msvc.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_utility.d
-include $(metabuilder_INTDIR.$(BUILDCONFIG))/writer_xcode.d

