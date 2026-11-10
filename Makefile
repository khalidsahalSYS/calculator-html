CC := x86_64-w64-mingw32-gcc
CFLAGS := -m64 -std=c11 -O2 -Wall -Wextra -I.
LDFLAGS :=
BUILD_DIR := build
APP_EXE := $(BUILD_DIR)/app.exe
TEST_EXE := $(BUILD_DIR)/test_calculator.exe

WINE ?= wine
ifeq ($(shell which $(WINE) 2>/dev/null),)
  WINE := wine64
endif

all: test app

test: $(TEST_EXE)
	@echo "Running unit tests..."
	$(WINE) $(TEST_EXE)

app: $(APP_EXE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TEST_EXE): | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ tests/test_calculator.c src/calculator.c

$(APP_EXE): | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ src/app.c src/calculator.c

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test app clean
