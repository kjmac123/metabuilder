
lua_MODULEOBJ := 

lua_CC := gcc
lua_CXX := g++

lua_INTDIR.Debug := int/lua/Debug
lua_OUTDIR.Debug := out/lua/Debug

lua_CPPFLAGS.Debug := \
  -Wall -g -O0 \

lua_CFLAGS.Debug := \

lua_CXXFLAGS.Debug := \
  -Wall \

lua_DEFINES.Debug := \
  -DMETABUILDER \
  -DPLATFORM_WINDOWS \
  -DWIN32 \
  -D_CRT_SECURE_NO_WARNINGS \
  -D_WINDOWS \
  -DBUILD_CONFIG_DEBUG \

lua_CPPFLAGS.Debug += $(lua_DEFINES.Debug)
lua_INCLUDES.Debug := \
  -I"$(DXSDK_DIR)Include" \
  -I"..\..\..\..\external\lua-5.2.2\src" \

lua_CPPFLAGS.Debug += $(lua_INCLUDES.Debug)
lua_INTDIR.Release := int/lua/Release
lua_OUTDIR.Release := out/lua/Release

lua_CPPFLAGS.Release := \
  -Wall -g -O2 \

lua_CFLAGS.Release := \

lua_CXXFLAGS.Release := \
  -Wall \

lua_DEFINES.Release := \
  -DMETABUILDER \
  -DPLATFORM_WINDOWS \
  -DWIN32 \
  -D_CRT_SECURE_NO_WARNINGS \
  -D_WINDOWS \
  -DBUILD_CONFIG_RELEASE \
  -DNDEBUG \

lua_CPPFLAGS.Release += $(lua_DEFINES.Release)
lua_INCLUDES.Release := \
  -I"$(DXSDK_DIR)Include" \
  -I"..\..\..\..\external\lua-5.2.2\src" \

lua_CPPFLAGS.Release += $(lua_INCLUDES.Release)
lua_INTDIR.Master := int/lua/Master
lua_OUTDIR.Master := out/lua/Master

lua_CPPFLAGS.Master := \
  -Wall -O2 \

lua_CFLAGS.Master := \

lua_CXXFLAGS.Master := \
  -Wall \

lua_DEFINES.Master := \
  -DMETABUILDER \
  -DPLATFORM_WINDOWS \
  -DWIN32 \
  -D_CRT_SECURE_NO_WARNINGS \
  -D_WINDOWS \
  -DBUILD_CONFIG_MASTER \
  -DNDEBUG \

lua_CPPFLAGS.Master += $(lua_DEFINES.Master)
lua_INCLUDES.Master := \
  -I"$(DXSDK_DIR)Include" \
  -I"..\..\..\..\external\lua-5.2.2\src" \

lua_CPPFLAGS.Master += $(lua_INCLUDES.Master)
lua_CPPFLAGS	:= -MMD -MP $(lua_CPPFLAGS.$(BUILDCONFIG))
lua_CFLAGS	:= $(lua_CFLAGS.$(BUILDCONFIG))
lua_CXXFLAGS	:= $(lua_CXXFLAGS.$(BUILDCONFIG))
lua_LD := g++
lua_CFLAGS += -c
lua_CXXFLAGS += -c
lua_LDFLAGS := 

lua_SRC := \
	..\..\..\..\external\lua-5.2.2\src\lapi.c \
	..\..\..\..\external\lua-5.2.2\src\lauxlib.c \
	..\..\..\..\external\lua-5.2.2\src\lbaselib.c \
	..\..\..\..\external\lua-5.2.2\src\lbitlib.c \
	..\..\..\..\external\lua-5.2.2\src\lcode.c \
	..\..\..\..\external\lua-5.2.2\src\lcorolib.c \
	..\..\..\..\external\lua-5.2.2\src\lctype.c \
	..\..\..\..\external\lua-5.2.2\src\ldblib.c \
	..\..\..\..\external\lua-5.2.2\src\ldebug.c \
	..\..\..\..\external\lua-5.2.2\src\ldo.c \
	..\..\..\..\external\lua-5.2.2\src\ldump.c \
	..\..\..\..\external\lua-5.2.2\src\lfunc.c \
	..\..\..\..\external\lua-5.2.2\src\lgc.c \
	..\..\..\..\external\lua-5.2.2\src\linit.c \
	..\..\..\..\external\lua-5.2.2\src\liolib.c \
	..\..\..\..\external\lua-5.2.2\src\llex.c \
	..\..\..\..\external\lua-5.2.2\src\lmathlib.c \
	..\..\..\..\external\lua-5.2.2\src\lmem.c \
	..\..\..\..\external\lua-5.2.2\src\loadlib.c \
	..\..\..\..\external\lua-5.2.2\src\lobject.c \
	..\..\..\..\external\lua-5.2.2\src\lopcodes.c \
	..\..\..\..\external\lua-5.2.2\src\loslib.c \
	..\..\..\..\external\lua-5.2.2\src\lparser.c \
	..\..\..\..\external\lua-5.2.2\src\lstate.c \
	..\..\..\..\external\lua-5.2.2\src\lstring.c \
	..\..\..\..\external\lua-5.2.2\src\lstrlib.c \
	..\..\..\..\external\lua-5.2.2\src\ltable.c \
	..\..\..\..\external\lua-5.2.2\src\ltablib.c \
	..\..\..\..\external\lua-5.2.2\src\ltm.c \
	..\..\..\..\external\lua-5.2.2\src\lundump.c \
	..\..\..\..\external\lua-5.2.2\src\lvm.c \
	..\..\..\..\external\lua-5.2.2\src\lzio.c \

lua_OBJ := \
	$(lua_INTDIR.$(BUILDCONFIG))/lapi.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lauxlib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lbaselib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lbitlib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lcode.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lcorolib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lctype.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ldblib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ldebug.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ldo.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ldump.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lfunc.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lgc.o \
	$(lua_INTDIR.$(BUILDCONFIG))/linit.o \
	$(lua_INTDIR.$(BUILDCONFIG))/liolib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/llex.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lmathlib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lmem.o \
	$(lua_INTDIR.$(BUILDCONFIG))/loadlib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lobject.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lopcodes.o \
	$(lua_INTDIR.$(BUILDCONFIG))/loslib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lparser.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lstate.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lstring.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lstrlib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ltable.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ltablib.o \
	$(lua_INTDIR.$(BUILDCONFIG))/ltm.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lundump.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lvm.o \
	$(lua_INTDIR.$(BUILDCONFIG))/lzio.o \

$(lua_INTDIR.$(BUILDCONFIG))/lapi.o : ..\..\..\..\external\lua-5.2.2\src\lapi.c $(lua_INTDIR.$(BUILDCONFIG))/lapi.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lapi.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lauxlib.o : ..\..\..\..\external\lua-5.2.2\src\lauxlib.c $(lua_INTDIR.$(BUILDCONFIG))/lauxlib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lauxlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lbaselib.o : ..\..\..\..\external\lua-5.2.2\src\lbaselib.c $(lua_INTDIR.$(BUILDCONFIG))/lbaselib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lbaselib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lbitlib.o : ..\..\..\..\external\lua-5.2.2\src\lbitlib.c $(lua_INTDIR.$(BUILDCONFIG))/lbitlib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lbitlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lcode.o : ..\..\..\..\external\lua-5.2.2\src\lcode.c $(lua_INTDIR.$(BUILDCONFIG))/lcode.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lcode.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lcorolib.o : ..\..\..\..\external\lua-5.2.2\src\lcorolib.c $(lua_INTDIR.$(BUILDCONFIG))/lcorolib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lcorolib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lctype.o : ..\..\..\..\external\lua-5.2.2\src\lctype.c $(lua_INTDIR.$(BUILDCONFIG))/lctype.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lctype.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ldblib.o : ..\..\..\..\external\lua-5.2.2\src\ldblib.c $(lua_INTDIR.$(BUILDCONFIG))/ldblib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ldblib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ldebug.o : ..\..\..\..\external\lua-5.2.2\src\ldebug.c $(lua_INTDIR.$(BUILDCONFIG))/ldebug.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ldebug.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ldo.o : ..\..\..\..\external\lua-5.2.2\src\ldo.c $(lua_INTDIR.$(BUILDCONFIG))/ldo.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ldo.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ldump.o : ..\..\..\..\external\lua-5.2.2\src\ldump.c $(lua_INTDIR.$(BUILDCONFIG))/ldump.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ldump.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lfunc.o : ..\..\..\..\external\lua-5.2.2\src\lfunc.c $(lua_INTDIR.$(BUILDCONFIG))/lfunc.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lfunc.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lgc.o : ..\..\..\..\external\lua-5.2.2\src\lgc.c $(lua_INTDIR.$(BUILDCONFIG))/lgc.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lgc.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/linit.o : ..\..\..\..\external\lua-5.2.2\src\linit.c $(lua_INTDIR.$(BUILDCONFIG))/linit.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\linit.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/liolib.o : ..\..\..\..\external\lua-5.2.2\src\liolib.c $(lua_INTDIR.$(BUILDCONFIG))/liolib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\liolib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/llex.o : ..\..\..\..\external\lua-5.2.2\src\llex.c $(lua_INTDIR.$(BUILDCONFIG))/llex.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\llex.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lmathlib.o : ..\..\..\..\external\lua-5.2.2\src\lmathlib.c $(lua_INTDIR.$(BUILDCONFIG))/lmathlib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lmathlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lmem.o : ..\..\..\..\external\lua-5.2.2\src\lmem.c $(lua_INTDIR.$(BUILDCONFIG))/lmem.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lmem.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/loadlib.o : ..\..\..\..\external\lua-5.2.2\src\loadlib.c $(lua_INTDIR.$(BUILDCONFIG))/loadlib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\loadlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lobject.o : ..\..\..\..\external\lua-5.2.2\src\lobject.c $(lua_INTDIR.$(BUILDCONFIG))/lobject.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lobject.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lopcodes.o : ..\..\..\..\external\lua-5.2.2\src\lopcodes.c $(lua_INTDIR.$(BUILDCONFIG))/lopcodes.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lopcodes.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/loslib.o : ..\..\..\..\external\lua-5.2.2\src\loslib.c $(lua_INTDIR.$(BUILDCONFIG))/loslib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\loslib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lparser.o : ..\..\..\..\external\lua-5.2.2\src\lparser.c $(lua_INTDIR.$(BUILDCONFIG))/lparser.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lparser.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lstate.o : ..\..\..\..\external\lua-5.2.2\src\lstate.c $(lua_INTDIR.$(BUILDCONFIG))/lstate.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lstate.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lstring.o : ..\..\..\..\external\lua-5.2.2\src\lstring.c $(lua_INTDIR.$(BUILDCONFIG))/lstring.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lstring.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lstrlib.o : ..\..\..\..\external\lua-5.2.2\src\lstrlib.c $(lua_INTDIR.$(BUILDCONFIG))/lstrlib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lstrlib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ltable.o : ..\..\..\..\external\lua-5.2.2\src\ltable.c $(lua_INTDIR.$(BUILDCONFIG))/ltable.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ltable.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ltablib.o : ..\..\..\..\external\lua-5.2.2\src\ltablib.c $(lua_INTDIR.$(BUILDCONFIG))/ltablib.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ltablib.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/ltm.o : ..\..\..\..\external\lua-5.2.2\src\ltm.c $(lua_INTDIR.$(BUILDCONFIG))/ltm.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\ltm.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lundump.o : ..\..\..\..\external\lua-5.2.2\src\lundump.c $(lua_INTDIR.$(BUILDCONFIG))/lundump.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lundump.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lvm.o : ..\..\..\..\external\lua-5.2.2\src\lvm.c $(lua_INTDIR.$(BUILDCONFIG))/lvm.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lvm.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 
$(lua_INTDIR.$(BUILDCONFIG))/lzio.o : ..\..\..\..\external\lua-5.2.2\src\lzio.c $(lua_INTDIR.$(BUILDCONFIG))/lzio.d 
	@echo $(lua_CC) ..\..\..\..\external\lua-5.2.2\src\lzio.c
	@$(lua_CC) $(lua_CPPFLAGS) $(lua_CFLAGS) -o '$@' '$<'; 

$(lua_INTDIR.$(BUILDCONFIG))/__lua__prelink__ : $(lua_OBJ) 
	@echo ld Creating prelink obj $(lua_INTDIR.$(BUILDCONFIG))/__lua__prelink__
	@ld  -r  $(lua_OBJ) -o '$@' ;

$(lua_OUTDIR.$(BUILDCONFIG))/lua : $(lua_INTDIR.$(BUILDCONFIG))/__lua__prelink__
	@echo  ld Creating module obj $(lua_OUTDIR.$(BUILDCONFIG))/lua
	@ld  -r $(lua_MODULEOBJ) $(lua_OBJ) -o '$@' ;


$(lua_OBJ) : | $(lua_INTDIR.$(BUILDCONFIG))

$(lua_INTDIR.$(BUILDCONFIG)):
	mkdir -p $(lua_INTDIR.$(BUILDCONFIG))
	mkdir -p $(lua_OUTDIR.$(BUILDCONFIG))

.PHONY: all_lua
all_lua : $(lua_OUTDIR.$(BUILDCONFIG))/lua 

.PHONY: clean_lua
clean_lua : 
	@echo Cleaning $(lua_INTDIR.$(BUILDCONFIG))
	@rm -f "$(lua_INTDIR.$(BUILDCONFIG))"/*
	@if [ -d "$(lua_INTDIR.$(BUILDCONFIG))" ]; then rmdir "$(lua_INTDIR.$(BUILDCONFIG))";fi
	@echo Cleaning $(lua_OUTDIR.$(BUILDCONFIG))
	@rm -f "$(lua_OUTDIR.$(BUILDCONFIG))"/*
	@if [ -d "$(lua_OUTDIR.$(BUILDCONFIG))" ]; then rmdir "$(lua_OUTDIR.$(BUILDCONFIG))";fi


%.d: ;
-include $(lua_INTDIR.$(BUILDCONFIG))/lapi.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lauxlib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lbaselib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lbitlib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lcode.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lcorolib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lctype.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ldblib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ldebug.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ldo.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ldump.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lfunc.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lgc.d
-include $(lua_INTDIR.$(BUILDCONFIG))/linit.d
-include $(lua_INTDIR.$(BUILDCONFIG))/liolib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/llex.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lmathlib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lmem.d
-include $(lua_INTDIR.$(BUILDCONFIG))/loadlib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lobject.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lopcodes.d
-include $(lua_INTDIR.$(BUILDCONFIG))/loslib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lparser.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lstate.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lstring.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lstrlib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ltable.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ltablib.d
-include $(lua_INTDIR.$(BUILDCONFIG))/ltm.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lundump.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lvm.d
-include $(lua_INTDIR.$(BUILDCONFIG))/lzio.d

