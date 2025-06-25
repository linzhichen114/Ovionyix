# Ovionyix
Ovionyix is a Unix-like operating system. ~~(Developing)~~

## Build

### Dependencies
You need install these in your operating system: 
 - GNU make
 - a C compiler
 - QEMU
 - curl
 - git
 - NASM, GNU as
 - xorriso
 - Rust Nightly toolchain (for libos-terminal)

### Compile

#### Makefile Targets
all: Make a live ISO named `Ovionyix.iso`.
run: Run `make all` and run as BIOS mode.
run-uefi: Run `make all` and run as UEFI mode.
clean: Delete `oxImage` and remove ISO cache.
distclean: Run `make clean` and remove dependencies.
> Note: If you want only, run `make -C kernel all`, this operation
> will make kernel in [`kernel/bin/Image`](kernel/bin/Image).

## Contributing
For contributing, please read [CONTRIBUTING.md](CONTRIBUTING.md).

## Last
<small>Finally, Ovionyix's name is from DeepSeek-R1 large language model :P</small>
