#by Martijn van Iersel (amarillion@yahoo.com)

# BUILD=DEBUG
# BUILD=RELEASE

# TARGET=EMSCRIPTEN
# TARGET=LINUX

CCFLAGS = -std=c++20 -Icommon/include
CFLAGS = -W -Wall -Wno-unused -DUSE_MOUSE
LFLAGS =
LIBS =

ALLEGRO_MODULES=allegro allegro_primitives allegro_font allegro_main allegro_dialog allegro_image allegro_audio allegro_acodec allegro_ttf allegro_color

ifeq ($(BUILD),RELEASE)
	CFLAGS += -O3 -s
	ALLEGRO_LIBS = $(addsuffix -5, $(ALLEGRO_MODULES))
else
ifeq ($(BUILD),DEBUG)
	CFLAGS += -g -DDEBUG
	ALLEGRO_LIBS = $(addsuffix -debug-5, $(ALLEGRO_MODULES))
else
$(error Unknown BUILD '$(BUILD)')
endif
endif

ifeq ($(TARGET),EMSCRIPTEN)
	CC = emcc
	CXX = em++
	LD = em++
	BINSUF = .html
	LIBS += `emconfigure pkg-config --libs allegro_monolith-static-5 sdl2`
	LIBS += -s USE_FREETYPE=1 -s USE_VORBIS=1 -s USE_OGG=1 -s USE_LIBJPEG=1 -s USE_LIBPNG=1 -s FULL_ES3=1 -s ASYNCIFY -s TOTAL_MEMORY=2147418112 -O3
# 	LIBS += --preload-file data@/data
else
ifeq ($(TARGET),LINUX)
	CC = gcc
	CXX = g++
	LD = g++
	BINSUF =
	LIBS += `pkg-config --libs $(ALLEGRO_LIBS)`
else
$(error Unknown TARGET '$(TARGET)')
endif
endif

BUILDDIR=build/$(BUILD)_$(TARGET)
OBJDIR=$(BUILDDIR)/obj

# Common sources / objects
COMMON_SRCS := $(wildcard common/src/*.cpp)
COMMON_OBJS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(notdir $(COMMON_SRCS)))

# Discover day directories automatically
DAYS := $(notdir $(wildcard day*))

# TARGETS     += $(BUILDDIR)/$1/$1$(BINSUF)
TARGETS := $(patsubst %,$(BUILDDIR)/%$(BINSUF),$(DAYS))

.PHONY: all clean
all: $(TARGETS)

# Per-day sources / objects
define day_template

DAY_SRCS_$1 := $(wildcard $1/*.cpp)
DAY_OBJS_$1 := $$(DAY_SRCS_$1:%.cpp=$(OBJDIR)/%.o)

$(BUILDDIR)/$1$(BINSUF): $(COMMON_OBJS) $$(DAY_OBJS_$1)
	@mkdir -p $$(@D)
	$(CXX) $$^ -o $$@ $(LFLAGS) $(LIBS)

$$(DAY_OBJS_$1): $(OBJDIR)/$1/%.o: $1/%.cpp
	@mkdir -p $$(@D)
	$(CXX) $(CCFLAGS) $(CFLAGS) -c $$< -o $$@

endef

$(foreach d,$(DAYS),$(eval $(call collect_targets,$(d))))
$(foreach d,$(DAYS),$(eval $(call day_template,$(d))))

$(COMMON_OBJS): $(OBJDIR)/%.o: common/src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CCFLAGS) $(CFLAGS) -c $< -o $@


# # Link each day binary
# $(TARGETS) : $(BUILDDIR)/%: $(COMMON_OBJS) $(BUILDDIR)/%/*.o
# 	@mkdir -p $(@D)
# 	$(CXX) $^ -o $@

# Compile rule (preserves directory structure)
# $(BUILDDIR)/%.o: %.cpp
# 	@mkdir -p $(@D)
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

# SRC = $(wildcard day?/*.cpp) $(wildcard common/src/*.cpp)
# OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRC)))
# DEP = $(patsubst %.cpp, $(OBJDIR)/%.d, $(notdir $(SRC)))

# # Link each day binary
# $(BUILDDIR)/%/%: $(COMMON_OBJS) $(OBJDIR)/%/*.o
# 	@mkdir -p $(@D)
# 	$(CXX) $^ -o $@

# $(BIN) : $(OBJ) $(LIB)
# 	$(LD) $^ -o $@ $(LIBS) $(LFLAGS)

# $(OBJ) : $(OBJDIR)/%.o : %.cpp | $(OBJDIR)
# 	$(CXX) $(CCFLAGS) $(CFLAGS) -MMD -c $< -o $@

# $(OBJDIR):
# 	$(shell mkdir -p $(OBJDIR) >/dev/null)

clean:
	rm -r $(BUILDDIR)

print-%  : ; @echo $* = $($*)