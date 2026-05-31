# Baby Droid Controller - Makefile
# Convenience wrapper for PlatformIO commands

.PHONY: help build upload monitor clean release

# Default target
help:
	@echo "Baby Droid Controller - Build Commands"
	@echo "======================================="
	@echo ""
	@echo "Commands:"
	@echo "  make build         - Build firmware"
	@echo "  make upload        - Upload firmware"
	@echo "  make monitor       - Open serial monitor"
	@echo "  make release       - Build + upload + monitor"
	@echo ""
	@echo "Utilities:"
	@echo "  make clean         - Clean build files"
	@echo ""

# ============================================
# BUILD COMMANDS
# ============================================

build:
	@pio run

upload:
	@pio run --target upload

monitor:
	@pio device monitor

release: upload monitor

# ============================================
# UTILITIES
# ============================================

clean:
	pio run --target clean
