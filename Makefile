# Thin compatibility wrapper over the CMake/Ninja build.
#
# This makefile preserves a small set of useful entry points while
# delegating all real build and test logic to CMake and CTest.

BUILD_DIR ?= build/release
CMAKE ?= cmake
CTEST ?= ctest
CMAKE_GENERATOR ?= Ninja
CMAKE_CONFIGURE_ARGS ?=
CMAKE_BUILD_ARGS ?=
CTEST_ARGS ?= --output-on-failure --timeout 300

.DEFAULT_GOAL := all

.PHONY: all configure help clean \
	experimental unit-tests integration-tests test \
	microvax3900 3b2-400 \
	stub frontpaneltest extra-tools

$(BUILD_DIR)/CMakeCache.txt:
	@mkdir -p "$(BUILD_DIR)"
	$(CMAKE) -G "$(CMAKE_GENERATOR)" \
		-DCMAKE_BUILD_TYPE=Release \
		$(CMAKE_CONFIGURE_ARGS) \
		-S . \
		-B "$(BUILD_DIR)"

configure: $(BUILD_DIR)/CMakeCache.txt

all: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" $(CMAKE_BUILD_ARGS)

experimental: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" \
		--target experimental-simulators \
		$(CMAKE_BUILD_ARGS)

unit-tests: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" \
		--target unit-tests \
		$(CMAKE_BUILD_ARGS)

integration-tests: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" \
		--target integration-tests \
		$(CMAKE_BUILD_ARGS)

test: $(BUILD_DIR)/CMakeCache.txt
	$(MAKE) unit-tests
	$(MAKE) integration-tests

microvax3900: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" --target vax $(CMAKE_BUILD_ARGS)

3b2-400: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" --target 3b2 $(CMAKE_BUILD_ARGS)

stub: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" --target stub $(CMAKE_BUILD_ARGS)

frontpaneltest: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" \
		--target frontpaneltest \
		$(CMAKE_BUILD_ARGS)

extra-tools: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" \
		--target extra-tools \
		$(CMAKE_BUILD_ARGS)

help:
	@printf '%s\n' \
		'Common targets:' \
		'  make all                Build the default simulator set.' \
		'  make <simulator>        Build one simulator target.' \
		'  make experimental       Build the experimental simulator set.' \
		'  make unit-tests         Build and run host-side unit tests.' \
		'  make integration-tests  Build and run simulator tests.' \
		'  make test               Run unit-tests and integration-tests.' \
		'  make stub              Build the stub simulator skeleton.' \
		'  make frontpaneltest    Build the front panel sample program.' \
		'  make extra-tools       Build stub and frontpaneltest.' \
		'' \
		'Configured build directory:' \
		'  $(BUILD_DIR)'

clean:
	$(CMAKE) -E rm -rf "$(BUILD_DIR)"

.DEFAULT: $(BUILD_DIR)/CMakeCache.txt
	$(CMAKE) --build "$(BUILD_DIR)" --target "$@" $(CMAKE_BUILD_ARGS)
