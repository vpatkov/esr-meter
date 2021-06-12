TARGET = esr-meter

CXX_SOURCES = $(wildcard *.cpp)

MCU = atmega88a
MCU_AVRDUDE = m88p
F_CPU = 8e6
FUSES = -U lfuse:w:0xe2:m -U hfuse:w:0xdf:m -U efuse:w:0xf9:m

CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size -C --mcu=$(MCU)
AVRDUDE = avrdude -c avrftdi -P usb -p $(MCU_AVRDUDE)

CXXFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CXXFLAGS += -c -std=c++17
CXXFLAGS += -Wall -Wextra -Woverloaded-virtual -Wcast-align -Wundef
CXXFLAGS += -Wlogical-op -Wredundant-decls -Wshadow -Wsuggest-override
CXXFLAGS += -Os -flto -fshort-enums
CXXFLAGS += -fno-exceptions -funsigned-bitfields
CXXFLAGS += -g -gdwarf-2
CXXFLAGS += -MMD -MP

LDFLAGS = -mmcu=$(MCU)
LDFLAGS += -Os -flto
LDFLAGS += -g

.PHONY: all listing clean program-flash program-fuses size

all: $(TARGET).hex size

listing: $(TARGET).lss

%.hex: %.elf
	$(OBJCOPY) -O ihex -j .data -j .text $< $@

%.lss: %.elf
	$(OBJDUMP) -h -d -S -z $< >$@

%.elf: %.o
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f *.o *.d *.elf *.lss

program-flash: $(TARGET).hex
	$(AVRDUDE) -U flash:w:$<:i

program-fuses:
	$(AVRDUDE) $(FUSES)

size: $(TARGET).elf
	$(SIZE) $<

$(TARGET).elf: $(CXX_SOURCES:.cpp=.o)

-include $(CXX_SOURCES:.cpp=.d)
