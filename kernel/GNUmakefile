# Nuke built-in rules and variables.
MAKEFLAGS += -rR
.SUFFIXES:

override OUTPUT := Image
PREFIX := /usr/local
CC := gcc
CFLAGS := -g -O2 -pipe
CPPFLAGS :=
NASMFLAGS := -F dwarf -g
LDFLAGS :=

ifeq ($(VERBOSE), 1)
  QUIET=
else
  QUIET=@
endif

ifneq ($(shell ( test '$(MAKECMDGOALS)' = clean || test '$(MAKECMDGOALS)' = distclean ); echo $$?),0)
    ifeq ($(shell ( ! test -d freestnd-c-hdrs || ! test -d cc-runtime || ! test -f src/limine.h ); echo $$?),0)
        $(error Please run the kernel/get-deps script to get dependencies first)
    endif
endif

# Check if CC is Clang.
override CC_IS_CLANG := $(shell ! $(CC) --version 2>/dev/null | grep 'clang' >/dev/null 2>&1; echo $$?)

# If the C compiler is Clang, set the target as needed.
ifeq ($(CC_IS_CLANG),1)
    override CC += \
        -target x86_64-unknown-none
endif

# Internal C flags that should not be changed by the user.
override CFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -nostdinc \
    -fuse-ld=lld \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-PIC \
    -ffunction-sections \
    -fdata-sections \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -mcmodel=kernel \
    -Iinclude

# Internal C preprocessor flags that should not be changed by the user.
override CPPFLAGS := \
    -Isrc \
    -isystem freestnd-c-hdrs \
    $(CPPFLAGS) \
    -DLIMINE_API_REVISION=3 \
    -MMD \
    -MP

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -Wall \
    -f elf64

# Internal linker flags that should not be changed by the user.
override LDFLAGS += \
    -Wl,-m,elf_x86_64 \
    -Wl,--build-id=none \
    -nostdlib \
    -static \
    -z max-page-size=0x1000 \
    -Wl,--gc-sections \
    -T linker.ld

override SRCFILES := $(shell find -L src cc-runtime/src -type f | LC_ALL=C sort)
override CFILES := $(filter %.c,$(SRCFILES))
override ASFILES := $(filter %.S,$(SRCFILES))
override NASMFILES := $(filter %.asm,$(SRCFILES))
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

.PHONY: all
all: bin/$(OUTPUT)

-include $(HEADER_DEPS)

bin/$(OUTPUT): GNUmakefile linker.ld $(OBJ)
	@printf "\033[1;32m(  ) Linking\033[0m ...\n"
	$(QUIET)mkdir -p "$$(dirname $@)"
	$(QUIET)$(CC) $(CFLAGS) $(LDFLAGS) $(OBJ) -o $@

obj/%.c.o: %.c GNUmakefile
	@printf "\033[1;32m(  ) Compiling\033[0m $< => $@ ...\n"
	$(QUIET)mkdir -p "$$(dirname $@)"
	$(QUIET)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.S.o: %.S GNUmakefile
	@printf "\033[1;32m(  ) Compiling\033[0m $< => $@ ...\n"
	$(QUIET)mkdir -p "$$(dirname $@)"
	$(QUIET)$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

obj/%.asm.o: %.asm GNUmakefile
	@printf "\033[1;32m(  ) Compiling\033[0m $< => $@ ...\n"
	$(QUIET)mkdir -p "$$(dirname $@)"
	$(QUIET)nasm $(NASMFLAGS) $< -o $@

.PHONY: clean
clean:
	@rm -rf bin obj
	@printf "\033[1;32m( √) Successful\033[0m built target $@\n"

.PHONY: distclean
distclean: clean
	@rm -rf freestnd-c-hdrs cc-runtime src/limine.h
	@printf "\033[1;32m( √) Successful\033[0m built target $@\n"

.PHONY: install
install: all
	install -d "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
	install -m 644 bin/$(OUTPUT) "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/"

.PHONY: uninstall
uninstall:
	rm -f "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)/$(OUTPUT)"
	-rmdir "$(DESTDIR)$(PREFIX)/share/$(OUTPUT)"
