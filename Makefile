# Master Makefile for XenevaOS
# Builds both Bootloader (EFI) and Kernel (ELF)

.PHONY: all boot kernel clean run

all: boot kernel

boot:
	@echo "Building Bootloader..."
	$(MAKE) -C Boot

kernel:
	@echo "Building Kernel..."
	$(MAKE) -C Kernel

clean:
	@echo "Cleaning Build Artifacts..."
	$(MAKE) -C Boot clean
	$(MAKE) -C Kernel clean
	rm -f xeneva.img

run: all
	@echo "Running QEMU..."
	# Creating a simple disk image for testing
	dd if=/dev/zero of=xeneva.img bs=1M count=64
	mkfs.fat -F 32 xeneva.img
	mmd -i xeneva.img ::/EFI
	mmd -i xeneva.img ::/EFI/BOOT
	mmd -i xeneva.img ::/EFI/XENEVA
	# Copy files (Assume mcopy is available)
	mcopy -i xeneva.img Boot/xnldr.efi ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i xeneva.img Kernel/xnkrnl.elf ::/EFI/XENEVA/xnkrnl.elf
	mcopy -i xeneva.img Resources/resources/EFI/XENEVA/font.psf ::/EFI/XENEVA/font.psf
	mcopy -i xeneva.img Resources/resources/EFI/XENEVA/ap.bin ::/EFI/XENEVA/ap.bin
	# Create dummy files for missing drivers to avoid bootloader errors
	touch ahci.dll nvme.dll
	mcopy -i xeneva.img ahci.dll ::/ahci.dll
	mcopy -i xeneva.img nvme.dll ::/nvme.dll
	# Launch QEMU
	qemu-system-x86_64 -bios /usr/share/ovmf/OVMF.fd -drive file=xeneva.img,format=raw -m 2G -serial stdio
