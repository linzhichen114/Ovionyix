# Ovionyix

## BUILD

### DEPENDENCIES
You need install these in your operating system: 
 - GNU make,
 - a C compiler,
 - QEMU,
 - curl,
 - git,
 - NASM,
 - GNU as,
 - and xorriso.

### COMPILE

#### MAKEFILE TARGETS
all: Make a live ISO named `Ovionyix.iso`.
run: Run `make all` and run as BIOS mode.
run-uefi: Run `make all` and run as UEFI mode.
clean: Clean `oxImage` and remove ISO cache.
distclean: Run `make clean` and remove dependencies.
> Note: If you want only, run `make -C kernel all`, this operation
> will make kernel in [`kernel/bin/oxImage`](kernel/bin/oxImage).

## CONTRIBUTING
For contributing, please read [CONTRIBUTING.md](CONTRIBUTING.md).