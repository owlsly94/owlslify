CC      := gcc
CFLAGS  := -Wall -Wextra -O2 -std=c11 -Iinclude
LIBS    := -lncurses

TARGET   := owlslify
BINDIR   := bin
BUILDDIR := build

# Sources
SRC     := src/main.c
MODULES := $(wildcard modules/*.c)

# Object files — mirror structure into build/
SRC_OBJ    := $(patsubst src/%.c,     $(BUILDDIR)/src/%.o,     $(SRC))
MODULE_OBJ := $(patsubst modules/%.c, $(BUILDDIR)/modules/%.o, $(MODULES))
OBJS       := $(SRC_OBJ) $(MODULE_OBJ)

PREFIX      := /usr/local
BININSTALL  := $(DESTDIR)$(PREFIX)/bin
DATAINSTALL := $(DESTDIR)$(PREFIX)/share/$(TARGET)

# ------------------------------------------------------------------ #

.PHONY: all clean install uninstall

all: $(BINDIR)/$(TARGET)

# Link
$(BINDIR)/$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo ""
	@echo "  ✔  Built: $@"
	@echo "  Run locally:      ./$(BINDIR)/$(TARGET)"
	@echo "  Install system:   sudo make install"
	@echo ""

# Compile src/
$(BUILDDIR)/src/%.o: src/%.c | $(BUILDDIR)/src
	$(CC) $(CFLAGS) -c $< -o $@

# Compile modules/
$(BUILDDIR)/modules/%.o: modules/%.c | $(BUILDDIR)/modules
	$(CC) $(CFLAGS) -c $< -o $@

# Create output directories
$(BINDIR):
	mkdir -p $(BINDIR)

$(BUILDDIR)/src:
	mkdir -p $(BUILDDIR)/src

$(BUILDDIR)/modules:
	mkdir -p $(BUILDDIR)/modules

# ------------------------------------------------------------------ #

install: $(BINDIR)/$(TARGET)
	@echo "==> Installing binary..."
	install -Dm755 $(BINDIR)/$(TARGET) $(BININSTALL)/$(TARGET)
	@echo "==> Installing scripts..."
	install -d $(DATAINSTALL)/scripts
	install -Dm755 scripts/*.sh $(DATAINSTALL)/scripts/
	@echo ""
	@echo "  ✔  Installed:"
	@echo "     Binary:  $(BININSTALL)/$(TARGET)"
	@echo "     Scripts: $(DATAINSTALL)/scripts/"
	@echo ""

uninstall:
	rm -f  $(BININSTALL)/$(TARGET)
	rm -rf $(DATAINSTALL)
	@echo "  ✔  Uninstalled owlslify."

clean:
	rm -rf $(BUILDDIR) $(BINDIR)
	@echo "Cleaned."
