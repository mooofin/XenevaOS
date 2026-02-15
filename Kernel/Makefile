# Kernel Makefile

TARGET = xnkrnl.elf
CPP_SRCS = $(shell find . -name "*.cpp")
C_SRCS = $(shell find . -name "*.c")
ASM_SRCS = $(shell find . -name "*.asm")

OBJS = $(CPP_SRCS:.cpp=.o) $(C_SRCS:.c=.o) $(ASM_SRCS:.asm=.o)

CC = gcc
CXX = gcc
LD = ld
ASM = nasm

COMMON_FLAGS = -ffreestanding -mno-red-zone -nostdinc -I../BaseHdr -I../Acpica/include -I../Acpica -I. -D_XENEVA_KERNEL -DARCH_X64 -DAUXENEVA -fno-stack-protector
CFLAGS = $(COMMON_FLAGS)
CXXFLAGS = $(COMMON_FLAGS) -fno-rtti -fno-exceptions
LDFLAGS = -T linker.ld -z max-page-size=0x1000 -nostdlib
ASMFLAGS = -f elf64

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
