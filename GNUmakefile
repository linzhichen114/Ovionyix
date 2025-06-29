# Nuke built-in rules and variables.
MAKEFLAGS += -rR
.SUFFIXES:

# Default user QEMU flags. These are appended to the QEMU command calls.
QEMUFLAGS :=

override IMAGE_NAME := Ovionyix

# Toolchain for building the 'limine' executable for the host.
HOST_CC := cc
HOST_CFLAGS := -g -O2 -pipe
HOST_CPPFLAGS :=
HOST_LDFLAGS :=
HOST_LIBS :=

.PHONY: all
all: $(IMAGE_NAME).iso
	@printf "\033[1;32m( √) Successful\033[0m built target $@\n"

.PHONY: run
run: $(IMAGE_NAME).iso
	@printf "\033[1;32m(  ) Running\033[0m ...\n"
	@qemu-system-x86_64 \
		-M q35 \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		-m 2G -vga virtio -serial stdio $(QEMUFLAGS)

.PHONY: debug
debug: $(IMAGE_NAME).iso
	@printf "\033[1;32m(  ) Running\033[0m ...\n"
	@qemu-system-x86_64 \
		-M q35 \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		-m 2G -vga virtio -serial stdio $(QEMUFLAGS) -s -S -no-reboot

.PHONY: run-uefi
run-uefi: ovmf/ovmf-code-x86_64.fd $(IMAGE_NAME).iso
	@printf "\033[1;32m(  ) Running\033[0m ...\n"
	@qemu-system-x86_64 \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		-m 2G -vga virtio -serial stdio $(QEMUFLAGS)

.PHONY: debug-uefi
debug-uefi: $(IMAGE_NAME).iso
	@printf "\033[1;32m(  ) Running\033[0m ...\n"
	@qemu-system-x86_64 \
		-M q35 \
		-drive if=pflash,unit=0,format=raw,file=ovmf/ovmf-code-x86_64.fd,readonly=on \
		-cdrom $(IMAGE_NAME).iso \
		-boot d \
		-m 2G -vga virtio -serial stdio $(QEMUFLAGS) -s -S -no-reboot

OVMF = https://github.com/osdev0/edk2-ovmf-nightly/releases/latest/download/ovmf-code-x86_64.fd
ovmf/ovmf-code-x86_64.fd:
	@printf "\033[1;32m(  ) Getting\033[0m $@ from \033[1m$(OVMF)\033[0m ...\n"
	@mkdir -p ovmf
	@curl -Lo $@ $(OVMF)

LIMINE = https://github.com/limine-bootloader/limine.git
limine/limine:
	@printf "\033[1;32m(  ) Getting\033[0m $@ from \033[1m$(LIMINE)\033[0m ...\n"
	@rm -rf limine
	@git clone $(LIMINE) --branch=v9.x-binary --depth=1
	@$(MAKE) -C limine \
		CC="$(HOST_CC)" \
		CFLAGS="$(HOST_CFLAGS)" \
		CPPFLAGS="$(HOST_CPPFLAGS)" \
		LDFLAGS="$(HOST_LDFLAGS)" \
		LIBS="$(HOST_LIBS)"

.kernel-deps:
	@printf "\033[1;32m(  ) Getting\033[0m dependencies ...\n"
	@./kernel/get-deps
	@touch .kernel-deps

.PHONY: kernel
kernel: .kernel-deps
	@printf "\033[1;32m(  ) Building\033[0m $@ ...\n"
	@$(MAKE) -C kernel
	@printf "\033[1;32m( √) Successful\033[0m built target $@\n"

$(IMAGE_NAME).iso: limine/limine kernel
	@printf "\033[1;32m(  ) Packing\033[0m $@ ...\n"
	@printf "\033[1;32m(  ) Copying\033[0m files ...\n"
	@rm -rf iso_root
	@mkdir -p iso_root/boot
	@cp -v kernel/bin/Image iso_root/boot/kernel
	@mkdir -p iso_root/boot/limine
	@cp -v limine.conf limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/
	@mkdir -p iso_root/boot/efi
	@cp -v limine/BOOTX64.EFI iso_root/boot/efi
	@cp -v limine/BOOTIA32.EFI iso_root/boot/efi
	@printf "\033[1;32m(  ) Packing\033[0m ...\n"
	@xorriso -as mkisofs -R -r -J -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table -hfsplus \
		-apm-block-size 2048 --efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	@printf "\033[1;32m(  ) Installing\033[0m Limine to $@ ...\n"
	@./limine/limine bios-install $(IMAGE_NAME).iso
	@rm -rf iso_root

.PHONY: clean
clean:
	@$(MAKE) -C kernel clean
	@rm -rf iso_root $(IMAGE_NAME).iso

.PHONY: distclean
distclean: clean
	@$(MAKE) -C kernel distclean
	@rm -rf .kernel-deps limine ovmf
