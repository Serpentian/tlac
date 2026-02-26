# ==============================
# Configuration
# ==============================

BUILD_DIR := build
GENERATOR := Ninja
BUILD_TYPE ?= Debug

# ==============================
# Default target
# ==============================

all: configure build

# ==============================
# Configure (with Ninja + build type)
# ==============================

configure:
	cmake -S . -B $(BUILD_DIR) \
	      -G "$(GENERATOR)" \
	      -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)

# ==============================
# Build
# ==============================

build:
	cmake --build $(BUILD_DIR)

# ==============================
# Rebuild
# ==============================

rebuild: clean all

# ==============================
# Clean
# ==============================

clean:
	rm -rf $(BUILD_DIR)

# ==============================
# Release build shortcut
# ==============================

release:
	$(MAKE) BUILD_TYPE=Release

# ==============================
# Debug build shortcut
# ==============================

debug:
	$(MAKE) BUILD_TYPE=Debug

.PHONY: all configure build clean rebuild release debug
