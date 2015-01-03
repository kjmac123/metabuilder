
lua_MODULEOBJ := 

lua_INTDIR := int/lua/$(BUILDCONFIG)
lua_OUTDIR := out/lua/$(BUILDCONFIG)

lua_CC := gcc
lua_CXX := g++
lua_CPPFLAGS.Debug := \
  -Wall -g -O0 \

lua_CFLAGS.Debug := \

lua_CXXFLAGS.Debug := \
  -Wall std=c++11 \

lua_DEFINES.Debug := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DPLATFORM_CYGWIN \
  -DBUILD_CONFIG_DEBUG \

lua_CPPFLAGS.Debug += $(lua_DEFINES.Debug)
lua_INCLUDES.Debug := \
  -I"../../../../external/lua-5.2.2/src" \

lua_CPPFLAGS.Debug += $(lua_INCLUDES.Debug)
lua_CPPFLAGS.Release := \
  -Wall -g -O2 \

lua_CFLAGS.Release := \

lua_CXXFLAGS.Release := \
  -Wall std=c++11 \

lua_DEFINES.Release := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DPLATFORM_CYGWIN \
  -DBUILD_CONFIG_RELEASE \

lua_CPPFLAGS.Release += $(lua_DEFINES.Release)
lua_INCLUDES.Release := \
  -I"../../../../external/lua-5.2.2/src" \

lua_CPPFLAGS.Release += $(lua_INCLUDES.Release)
lua_CPPFLAGS.Master := \
  -Wall -O2 \

lua_CFLAGS.Master := \

lua_CXXFLAGS.Master := \
  -Wall std=c++11 \

lua_DEFINES.Master := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DPLATFORM_CYGWIN \
  -DBUILD_CONFIG_MASTER \

lua_CPPFLAGS.Master += $(lua_DEFINES.Master)
lua_INCLUDES.Master := \
  -I"../../../../external/lua-5.2.2/src" \

lua_CPPFLAGS.Master += $(lua_INCLUDES.Master)
lua_CPPFLAGS	:= -MMD -MP $(lua_CPPFLAGS.$(BUILDCONFIG))
lua_CFLAGS	:= $(lua_CFLAGS.$(BUILDCONFIG))
lua_CXXFLAGS	:= $(lua_CXXFLAGS.$(BUILDCONFIG))
lua_LD := g++
lua_CFLAGS += -c
lua_CXXFLAGS += -c
lua_LDFLAGS := 

lua_SRC := \
	../../../../external/lua-5.2.2/src/lapi.c \
	../../../../external/lua-5.2.2/src/lauxlib.c \
	../../../../external/lua-5.2.2/src/lbaselib.c \
	../../../../external/lua-5.2.2/src/lbitlib.c \
	../../../../external/lua-5.2.2/src/lcode.c \
	../../../../external/lua-5.2.2/src/lcorolib.c \
	../../../../external/lua-5.2.2/src/lctype.c \
	../../../../external/lua-5.2.2/src/ldblib.c \
	../../../../external/lua-5.2.2/src/ldebug.c \
	../../../../external/lua-5.2.2/src/ldo.c \
	../../../../external/lua-5.2.2/src/ldump.c \
	../../../../external/lua-5.2.2/src/lfunc.c \
	../../../../external/lua-5.2.2/src/lgc.c \
	../../../../external/lua-5.2.2/src/linit.c \
	../../../../external/lua-5.2.2/src/liolib.c \
	../../../../external/lua-5.2.2/src/llex.c \
	../../../../external/lua-5.2.2/src/lmathlib.c \
	../../../../external/lua-5.2.2/src/lmem.c \
	../../../../external/lua-5.2.2/src/loadlib.c \
	../../../../external/lua-5.2.2/src/lobject.c \
	../../../../external/lua-5.2.2/src/lopcodes.c \
	../../../../external/lua-5.2.2/src/loslib.c \
	../../../../external/lua-5.2.2/src/lparser.c \
	../../../../external/lua-5.2.2/src/lstate.c \
	../../../../external/lua-5.2.2/src/lstring.c \
	../../../../external/lua-5.2.2/src/lstrlib.c \
	../../../../external/lua-5.2.2/src/ltable.c \
	../../../../external/lua-5.2.2/src/ltablib.c \
	../../../../external/lua-5.2.2/src/ltm.c \
	../../../../external/lua-5.2.2/src/lundump.c \
	../../../../external/lua-5.2.2/src/lvm.c \
	../../../../external/lua-5.2.2/src/lzio.c \

lua_OBJ := \
	$(lua_INTDIR)/lapi.o \
	$(lua_INTDIR)/lauxlib.o \
	$(lua_INTDIR)/lbaselib.o \
	$(lua_INTDIR)/lbitlib.o \
	$(lua_INTDIR)/lcode.o \
	$(lua_INTDIR)/lcorolib.o \
	$(lua_INTDIR)/lctype.o \
	$(lua_INTDIR)/ldblib.o \
	$(lua_INTDIR)/ldebug.o \
	$(lua_INTDIR)/ldo.o \
	$(lua_INTDIR)/ldump.o \
	$(lua_INTDIR)/lfunc.o \
	$(lua_INTDIR)/lgc.o \
	$(lua_INTDIR)/linit.o \
	$(lua_INTDIR)/liolib.o \
	$(lua_INTDIR)/llex.o \
	$(lua_INTDIR)/lmathlib.o \
	$(lua_INTDIR)/lmem.o \
	$(lua_INTDIR)/loadlib.o \
	$(lua_INTDIR)/lobject.o \
	$(lua_INTDIR)/lopcodes.o \
	$(lua_INTDIR)/loslib.o \
	$(lua_INTDIR)/lparser.o \
	$(lua_INTDIR)/lstate.o \
	$(lua_INTDIR)/lstring.o \
	$(lua_INTDIR)/lstrlib.o \
	$(lua_INTDIR)/ltable.o \
	$(lua_INTDIR)/ltablib.o \
	$(lua_INTDIR)/ltm.o \
	$(lua_INTDIR)/lundump.o \
	$(lua_INTDIR)/lvm.o \
	$(lua_INTDIR)/lzio.o \

$(lua_INTDIR)/lapi.o : ../../../../external/lua-5.2.2/src/lapi.c $(lua_INTDIR)/lapi.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lapi.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lauxlib.o : ../../../../external/lua-5.2.2/src/lauxlib.c $(lua_INTDIR)/lauxlib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lauxlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lbaselib.o : ../../../../external/lua-5.2.2/src/lbaselib.c $(lua_INTDIR)/lbaselib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lbaselib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lbitlib.o : ../../../../external/lua-5.2.2/src/lbitlib.c $(lua_INTDIR)/lbitlib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lbitlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lcode.o : ../../../../external/lua-5.2.2/src/lcode.c $(lua_INTDIR)/lcode.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lcode.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lcorolib.o : ../../../../external/lua-5.2.2/src/lcorolib.c $(lua_INTDIR)/lcorolib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lcorolib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lctype.o : ../../../../external/lua-5.2.2/src/lctype.c $(lua_INTDIR)/lctype.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lctype.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ldblib.o : ../../../../external/lua-5.2.2/src/ldblib.c $(lua_INTDIR)/ldblib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ldblib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ldebug.o : ../../../../external/lua-5.2.2/src/ldebug.c $(lua_INTDIR)/ldebug.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ldebug.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ldo.o : ../../../../external/lua-5.2.2/src/ldo.c $(lua_INTDIR)/ldo.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ldo.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ldump.o : ../../../../external/lua-5.2.2/src/ldump.c $(lua_INTDIR)/ldump.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ldump.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lfunc.o : ../../../../external/lua-5.2.2/src/lfunc.c $(lua_INTDIR)/lfunc.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lfunc.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lgc.o : ../../../../external/lua-5.2.2/src/lgc.c $(lua_INTDIR)/lgc.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lgc.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/linit.o : ../../../../external/lua-5.2.2/src/linit.c $(lua_INTDIR)/linit.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/linit.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/liolib.o : ../../../../external/lua-5.2.2/src/liolib.c $(lua_INTDIR)/liolib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/liolib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/llex.o : ../../../../external/lua-5.2.2/src/llex.c $(lua_INTDIR)/llex.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/llex.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lmathlib.o : ../../../../external/lua-5.2.2/src/lmathlib.c $(lua_INTDIR)/lmathlib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lmathlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lmem.o : ../../../../external/lua-5.2.2/src/lmem.c $(lua_INTDIR)/lmem.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lmem.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/loadlib.o : ../../../../external/lua-5.2.2/src/loadlib.c $(lua_INTDIR)/loadlib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/loadlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lobject.o : ../../../../external/lua-5.2.2/src/lobject.c $(lua_INTDIR)/lobject.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lobject.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lopcodes.o : ../../../../external/lua-5.2.2/src/lopcodes.c $(lua_INTDIR)/lopcodes.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lopcodes.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/loslib.o : ../../../../external/lua-5.2.2/src/loslib.c $(lua_INTDIR)/loslib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/loslib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lparser.o : ../../../../external/lua-5.2.2/src/lparser.c $(lua_INTDIR)/lparser.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lparser.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lstate.o : ../../../../external/lua-5.2.2/src/lstate.c $(lua_INTDIR)/lstate.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lstate.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lstring.o : ../../../../external/lua-5.2.2/src/lstring.c $(lua_INTDIR)/lstring.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lstring.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lstrlib.o : ../../../../external/lua-5.2.2/src/lstrlib.c $(lua_INTDIR)/lstrlib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lstrlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ltable.o : ../../../../external/lua-5.2.2/src/ltable.c $(lua_INTDIR)/ltable.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ltable.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ltablib.o : ../../../../external/lua-5.2.2/src/ltablib.c $(lua_INTDIR)/ltablib.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ltablib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/ltm.o : ../../../../external/lua-5.2.2/src/ltm.c $(lua_INTDIR)/ltm.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/ltm.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lundump.o : ../../../../external/lua-5.2.2/src/lundump.c $(lua_INTDIR)/lundump.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lundump.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lvm.o : ../../../../external/lua-5.2.2/src/lvm.c $(lua_INTDIR)/lvm.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lvm.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR)/lzio.o : ../../../../external/lua-5.2.2/src/lzio.c $(lua_INTDIR)/lzio.d 
	@echo $(lua_CC) ../../../../external/lua-5.2.2/src/lzio.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 

$(lua_INTDIR)/__lua__prelink__ : $(lua_OBJ) 
	@echo ld Creating prelink obj $(lua_INTDIR)/__lua__prelink__
	@ld  -r  $(lua_OBJ) -o '$@' ;

$(lua_OUTDIR)/lua : $(lua_INTDIR)/__lua__prelink__
	@echo  ld Creating module obj $(lua_OUTDIR)/lua
	@ld  -r $(lua_MODULEOBJ) $(lua_OBJ) -o '$@' ;


$(lua_OBJ) : | $(lua_INTDIR)

$(lua_INTDIR):
	mkdir -p $(lua_INTDIR)
	mkdir -p $(lua_OUTDIR)

.PHONY: all_lua
all_lua : $(lua_OUTDIR)/lua 

.PHONY: clean_lua
clean_lua : 
	@echo Cleaning $(lua_INTDIR)
	@rm -f "$(lua_INTDIR)"/*
	@if [ -d "$(lua_INTDIR)" ]; then rmdir "$(lua_INTDIR)";fi
	@echo Cleaning $(lua_OUTDIR)
	@rm -f "$(lua_OUTDIR)"/*
	@if [ -d "$(lua_OUTDIR)" ]; then rmdir "$(lua_OUTDIR)";fi


%.d: ;
-include $(lua_INTDIR)/lapi.d
-include $(lua_INTDIR)/lauxlib.d
-include $(lua_INTDIR)/lbaselib.d
-include $(lua_INTDIR)/lbitlib.d
-include $(lua_INTDIR)/lcode.d
-include $(lua_INTDIR)/lcorolib.d
-include $(lua_INTDIR)/lctype.d
-include $(lua_INTDIR)/ldblib.d
-include $(lua_INTDIR)/ldebug.d
-include $(lua_INTDIR)/ldo.d
-include $(lua_INTDIR)/ldump.d
-include $(lua_INTDIR)/lfunc.d
-include $(lua_INTDIR)/lgc.d
-include $(lua_INTDIR)/linit.d
-include $(lua_INTDIR)/liolib.d
-include $(lua_INTDIR)/llex.d
-include $(lua_INTDIR)/lmathlib.d
-include $(lua_INTDIR)/lmem.d
-include $(lua_INTDIR)/loadlib.d
-include $(lua_INTDIR)/lobject.d
-include $(lua_INTDIR)/lopcodes.d
-include $(lua_INTDIR)/loslib.d
-include $(lua_INTDIR)/lparser.d
-include $(lua_INTDIR)/lstate.d
-include $(lua_INTDIR)/lstring.d
-include $(lua_INTDIR)/lstrlib.d
-include $(lua_INTDIR)/ltable.d
-include $(lua_INTDIR)/ltablib.d
-include $(lua_INTDIR)/ltm.d
-include $(lua_INTDIR)/lundump.d
-include $(lua_INTDIR)/lvm.d
-include $(lua_INTDIR)/lzio.d

