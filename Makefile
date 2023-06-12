SOURCES = watchy-face.ino watchayy.h watchayy.cpp settings.h Passing_Shot_modified.h sketch.yaml
BUILD_DIR = ./_build
SKETCH = watchy-face
PROFILE = watchy

_build/$(SKETCH).ino.elf: $(SOURCES)
	arduino-cli compile --build-path $(BUILD_DIR) --profile $(PROFILE)

.PHONY: upload
upload: _build/$(SKETCH).ino.elf
	arduino-cli upload --input-dir $(BUILD_DIR) --profile $(PROFILE)

