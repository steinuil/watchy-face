SOURCES = \
	sketch.yaml \
	watchy-face.ino \
	watchayy.h watchayy.cpp \
	settings.h \
	DSEG7_Classic_Bold_53.h \
	Passing_Shot_modified.h \
	BLE.cpp BLE.h \
	Display.cpp Display.h \
	Watchy.cpp Watchy.h \
	WatchyRTC.cpp WatchyRTC.h \
	bma.cpp bma.h bma4.c bma4.h bma423.c bma423.h bma4_defs.h \
	config.h icons.h

BUILD_DIR = ./_build
SKETCH = watchy-face
PROFILE = watchy
WARNINGS = all
PORT = /dev/ttyUSB0

_build/$(SKETCH).ino.elf: $(SOURCES)
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE) --warnings $(WARNINGS)

.PHONY: monitor
monitor:
	arduino-cli monitor --config baudrate=115200 --port $(PORT)

.PHONY: upload
upload: _build/$(SKETCH).ino.elf
	arduino-cli upload --input-dir $(BUILD_DIR) --profile $(PROFILE) --port $(PORT)
	arduino-cli monitor --config baudrate=115200 --port $(PORT)

