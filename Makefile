.PHONY: default
default: compile_commands.json
	pio run

compile_commands.json: platformio.ini
	pio run -t compiledb
