SOURCES = watchy-face.ino watchayy.h watchayy.cpp settings.h Passing_Shot_modified.h sketch.yaml
BUILD_DIR = ./_build
PROFILE = watchy
PORT = /dev/ttyUSB0

_build/watchy-face.ino.elf: $(SOURCES)
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE)

.PHONY: upload
upload:
	arduino-cli upload --input-dir $(BUILD_DIR) --profile $(PROFILE) --port $(PORT)

