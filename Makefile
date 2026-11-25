#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "DEVKITPRO is not set. Please set DEVKITPRO in your environment. export DEVKITPRO=/path/to/devkitPro")
endif

include $(DEVKITPRO)/用/devkitARM/3ds_rules

TARGET := StormScout
BUILD := build
SOURCES := source
DATA :=
INCLUDES :=

APP_TITLE := Storm Scout
APP_DESCRIPTION := A portable storm-chasing adventure
APP_AUTHOR := Storm Scout Team
APP_VERSION := 1.0.0

CFLAGS := -g -Wall -Wextra -O2 -mword-relocations -ffunction-sections -fdata-sections
CFLAGS += $(INCLUDE) -D__3DS__
LDFLAGS := -specs=3dsx.specs -g -Wl,-Map,$(notdir $*.map) -Wl,--gc-sections
LIBS := -lctru

#---------------------------------------------------------------------------------
# no user configuration below this point
#---------------------------------------------------------------------------------

ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)

export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)

CFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

export LD := $(CC)
export OFILES := $(addsuffix .o,$(BINFILES)) $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD)

.PHONY: all clean

all:
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).3dsx $(TARGET).elf

#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------

DEPENDS := $(OFILES:.o=.d)

$(OUTPUT).3dsx: $(OUTPUT).elf
$(OUTPUT).elf: $(OFILES)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
