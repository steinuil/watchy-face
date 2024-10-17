SKETCH = watchy-face

SOURCES = \
	$(SKETCH)/sketch.yaml \
	$(SKETCH)/watchy-face.ino \
	$(SKETCH)/watchayy.h $(SKETCH)/watchayy.cpp \
	$(SKETCH)/settings.h \
	$(SKETCH)/pcf8563.cpp $(SKETCH)/pcf8563.h \
	$(SKETCH)/DSEG7_Classic_Bold_53.h \
	$(SKETCH)/Passing_Shot_modified.h \
	$(SKETCH)/BLE.cpp $(SKETCH)/BLE.h \
	$(SKETCH)/Display.cpp $(SKETCH)/Display.h \
	$(SKETCH)/Watchy.cpp $(SKETCH)/Watchy.h \
	$(SKETCH)/WatchyRTC.cpp $(SKETCH)/WatchyRTC.h \
	$(SKETCH)/bma.cpp $(SKETCH)/bma.h $(SKETCH)/bma4.c $(SKETCH)/bma4.h $(SKETCH)/bma423.c $(SKETCH)/bma423.h $(SKETCH)/bma4_defs.h \
	$(SKETCH)/config.h $(SKETCH)/icons.h

BUILD_DIR = ./_build
PROFILE = watchy
WARNINGS = all
PORT = /dev/ttyUSB0

_build/$(SKETCH).ino.elf: $(SOURCES)
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE) --warnings $(WARNINGS) $(SKETCH)

.PHONY: compilation-db
compilation-db:
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE) --warnings $(WARNINGS) $(SKETCH) --only-compilation-database

.PHONY: monitor
monitor:
	arduino-cli monitor --config baudrate=115200 --port $(PORT) $(SKETCH)

.PHONY: upload
upload: _build/$(SKETCH).ino.elf
	arduino-cli upload --input-dir $(BUILD_DIR) --profile $(PROFILE) --port $(PORT) $(SKETCH)
	arduino-cli monitor --config baudrate=115200 --port $(PORT) $(SKETCH)

