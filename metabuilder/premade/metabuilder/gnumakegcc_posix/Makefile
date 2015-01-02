BUILDCONFIG := Release


default : all 
metabuilder_MODULEOBJ := 
include dlmalloc.mk
metabuilder_MODULEOBJ += $(dlmalloc_OUTDIR)/dlmalloc
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

metabuilder_DEFINES.Debug := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_DEBUG \

metabuilder_CPPFLAGS.Debug += $(metabuilder_DEFINES.Debug)
metabuilder_INCLUDES.Debug := \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/src" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/ezOptionParser-0.2.1" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/dlmalloc-2.8.6" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/lua-5.2.2/src" \

metabuilder_CPPFLAGS.Debug += $(metabuilder_INCLUDES.Debug)
metabuilder_CPPFLAGS.Release := \
  -Wall -g -O2 \

metabuilder_CFLAGS.Release := \

metabuilder_CXXFLAGS.Release := \
  -Wall \

metabuilder_DEFINES.Release := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_RELEASE \

metabuilder_CPPFLAGS.Release += $(metabuilder_DEFINES.Release)
metabuilder_INCLUDES.Release := \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/src" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/ezOptionParser-0.2.1" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/dlmalloc-2.8.6" \
  -I"../../../../../../../:\opensource\metabuilder\metabuilder/../external/lua-5.2.2/src" \

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

metabuilder_LDLIBS.Debug += $(metabuilder_LIBS.Debug)

metabuilder_LDFLAGS.Release := \
 \

metabuilder_LIBDIRS.Release := \

metabuilder_LDFLAGS.Release += $(metabuilder_LIBDIRS.Release)
metabuilder_LIBS.Release := \

metabuilder_LDLIBS.Release += $(metabuilder_LIBS.Release)

metabuilder_LDFLAGS := $(metabuilder_LDFLAGS.$(BUILDCONFIG)) $(metabuilder_LDLIBS.$(BUILDCONFIG))

metabuilder_SRC := \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/block.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/common.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/configparam.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/core.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/luafile.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/main.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/makeglobal.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/makesetup.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/metabase.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/metabuilder_pch.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/platform.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/posix/platform_posix.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/platformparam.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/solution.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/target.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/timeutil.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/writer.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_msvc.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_utility.cpp \
	../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_xcode.cpp \

metabuilder_OBJ := \
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

$(metabuilder_INTDIR)/block.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/block.cpp $(metabuilder_INTDIR)/block.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/block.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/common.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/common.cpp $(metabuilder_INTDIR)/common.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/common.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/configparam.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/configparam.cpp $(metabuilder_INTDIR)/configparam.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/configparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/core.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/core.cpp $(metabuilder_INTDIR)/core.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/core.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/luafile.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/luafile.cpp $(metabuilder_INTDIR)/luafile.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/luafile.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/main.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/main.cpp $(metabuilder_INTDIR)/main.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/main.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/makeglobal.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/makeglobal.cpp $(metabuilder_INTDIR)/makeglobal.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/makeglobal.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/makesetup.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/makesetup.cpp $(metabuilder_INTDIR)/makesetup.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/makesetup.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/metabase.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/metabase.cpp $(metabuilder_INTDIR)/metabase.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/metabase.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/metabuilder_pch.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/metabuilder_pch.cpp $(metabuilder_INTDIR)/metabuilder_pch.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/metabuilder_pch.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platform.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/platform.cpp $(metabuilder_INTDIR)/platform.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/platform.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platform_posix.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/posix/platform_posix.cpp $(metabuilder_INTDIR)/platform_posix.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/platform/posix/platform_posix.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/platformparam.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/platformparam.cpp $(metabuilder_INTDIR)/platformparam.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/platformparam.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/solution.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/solution.cpp $(metabuilder_INTDIR)/solution.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/solution.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/target.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/target.cpp $(metabuilder_INTDIR)/target.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/target.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/timeutil.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/timeutil.cpp $(metabuilder_INTDIR)/timeutil.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/timeutil.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer.cpp $(metabuilder_INTDIR)/writer.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_msvc.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_msvc.cpp $(metabuilder_INTDIR)/writer_msvc.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_msvc.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_utility.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_utility.cpp $(metabuilder_INTDIR)/writer_utility.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_utility.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 
$(metabuilder_INTDIR)/writer_xcode.o : ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_xcode.cpp $(metabuilder_INTDIR)/writer_xcode.d 
	@echo $(metabuilder_CXX) ../../../../../../../:\opensource\metabuilder\metabuilder/src/writer_xcode.cpp
	@$(metabuilder_CXX) $(metabuilder_CPPFLAGS) $(metabuilder_CXXFLAGS) -o '$@' '$<'; 

$(metabuilder_INTDIR)/__metabuilder__prelink__ : $(metabuilder_OBJ) $(dlmalloc_OUTDIR)/dlmalloc $(lua_OUTDIR)/lua 
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
clean_metabuilder : clean_dlmalloc clean_lua 
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

