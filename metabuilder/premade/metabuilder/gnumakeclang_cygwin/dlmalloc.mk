
dlmalloc_MODULEOBJ := 

dlmalloc_INTDIR := int/dlmalloc/$(BUILDCONFIG)
dlmalloc_OUTDIR := out/dlmalloc/$(BUILDCONFIG)

dlmalloc_CC := clang
dlmalloc_CXX := clang++
dlmalloc_CPPFLAGS.Debug := \
  -Wall -g -O0 \

dlmalloc_CFLAGS.Debug := \

dlmalloc_CXXFLAGS.Debug := \
  -Wall -std=c++11 \

dlmalloc_DEFINES.Debug := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_DEBUG \

dlmalloc_CPPFLAGS.Debug += $(dlmalloc_DEFINES.Debug)
dlmalloc_INCLUDES.Debug := \
  -I"../../../../external/dlmalloc-2.8.6/src" \

dlmalloc_CPPFLAGS.Debug += $(dlmalloc_INCLUDES.Debug)
dlmalloc_CPPFLAGS.Release := \
  -Wall -g -O2 \

dlmalloc_CFLAGS.Release := \

dlmalloc_CXXFLAGS.Release := \
  -Wall -std=c++11 \

dlmalloc_DEFINES.Release := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_RELEASE \

dlmalloc_CPPFLAGS.Release += $(dlmalloc_DEFINES.Release)
dlmalloc_INCLUDES.Release := \
  -I"../../../../external/dlmalloc-2.8.6/src" \

dlmalloc_CPPFLAGS.Release += $(dlmalloc_INCLUDES.Release)
dlmalloc_CPPFLAGS.Master := \
  -Wall -O2 \

dlmalloc_CFLAGS.Master := \

dlmalloc_CXXFLAGS.Master := \
  -Wall -std=c++11 \

dlmalloc_DEFINES.Master := \
  -DMETABUILDER \
  -DPLATFORM_POSIX \
  -DBUILD_CONFIG_MASTER \

dlmalloc_CPPFLAGS.Master += $(dlmalloc_DEFINES.Master)
dlmalloc_INCLUDES.Master := \
  -I"../../../../external/dlmalloc-2.8.6/src" \

dlmalloc_CPPFLAGS.Master += $(dlmalloc_INCLUDES.Master)
dlmalloc_CPPFLAGS	:= -MMD -MP $(dlmalloc_CPPFLAGS.$(BUILDCONFIG))
dlmalloc_CFLAGS	:= $(dlmalloc_CFLAGS.$(BUILDCONFIG))
dlmalloc_CXXFLAGS	:= $(dlmalloc_CXXFLAGS.$(BUILDCONFIG))
dlmalloc_LD := clang++
dlmalloc_CFLAGS += -c
dlmalloc_CXXFLAGS += -c
dlmalloc_LDFLAGS := 

dlmalloc_SRC := \
	../../../../external/dlmalloc-2.8.6/dlmalloc.cpp \

dlmalloc_OBJ := \
	$(dlmalloc_INTDIR)/dlmalloc.o \

$(dlmalloc_INTDIR)/dlmalloc.o : ../../../../external/dlmalloc-2.8.6/dlmalloc.cpp $(dlmalloc_INTDIR)/dlmalloc.d 
	@echo $(dlmalloc_CXX) ../../../../external/dlmalloc-2.8.6/dlmalloc.cpp
	@$(dlmalloc_CXX) $(dlmalloc_CPPFLAGS) $(dlmalloc_CXXFLAGS) -o '$@' '$<'; 

$(dlmalloc_INTDIR)/__dlmalloc__prelink__ : $(dlmalloc_OBJ) 
	@echo ld Creating prelink obj $(dlmalloc_INTDIR)/__dlmalloc__prelink__
	@ld  -r  $(dlmalloc_OBJ) -o '$@' ;

$(dlmalloc_OUTDIR)/dlmalloc : $(dlmalloc_INTDIR)/__dlmalloc__prelink__
	@echo  ld Creating module obj $(dlmalloc_OUTDIR)/dlmalloc
	@ld  -r $(dlmalloc_MODULEOBJ) $(dlmalloc_OBJ) -o '$@' ;


$(dlmalloc_OBJ) : | $(dlmalloc_INTDIR)

$(dlmalloc_INTDIR):
	mkdir -p $(dlmalloc_INTDIR)
	mkdir -p $(dlmalloc_OUTDIR)

.PHONY: all_dlmalloc
all_dlmalloc : $(dlmalloc_OUTDIR)/dlmalloc 

.PHONY: clean_dlmalloc
clean_dlmalloc : 
	@echo Cleaning $(dlmalloc_INTDIR)
	@rm -f "$(dlmalloc_INTDIR)"/*
	@if [ -d "$(dlmalloc_INTDIR)" ]; then rmdir "$(dlmalloc_INTDIR)";fi
	@echo Cleaning $(dlmalloc_OUTDIR)
	@rm -f "$(dlmalloc_OUTDIR)"/*
	@if [ -d "$(dlmalloc_OUTDIR)" ]; then rmdir "$(dlmalloc_OUTDIR)";fi


%.d: ;
-include $(dlmalloc_INTDIR)/dlmalloc.d

