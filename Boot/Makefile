# Bootloader Makefile

TARGET = xnldr.efi
CPP_SRCS = $(shell find . -maxdepth 1 -name "*.cpp")
ASM_SRCS = $(shell find . -maxdepth 1 -name "*.asm")

OBJS = $(CPP_SRCS:.cpp=.o) $(ASM_SRCS:.asm=.o)

CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-gcc
LD = x86_64-w64-mingw32-gcc
ASM = nasm

CFLAGS = -ffreestanding -I../BaseHdr -Iinclude -Iinclude/x64 -D__x86_64__ -Wall -Wextra
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions
LDFLAGS = -nostdlib -Wl,-dll -shared -Wl,--subsystem,10 -e efi_main
ASMFLAGS = -f win64

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
