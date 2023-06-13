SOURCES = watchy-face.ino watchayy.h watchayy.cpp settings.h Passing_Shot_modified.h sketch.yaml
BUILD_DIR = ./_build
SKETCH = watchy-face
PROFILE = watchy
PORT = /dev/ttyUSB0

_build/$(SKETCH).ino.elf: $(SOURCES)
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE)

.PHONY: monitor
monitor:
	arduino-cli monitor --config baudrate=115200 --port $(PORT)

.PHONY: upload
upload: _build/$(SKETCH).ino.elf
	arduino-cli upload --input-dir $(BUILD_DIR) --profile $(PROFILE) --port $(PORT)
	arduino-cli monitor --config baudrate=115200 --port $(PORT)

