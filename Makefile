ASSEMBLER := assembler/assembler.py
OUT_DIR := out
OUT := debug-cpu auto-cpu char-display display

CFLAGS := -Wall

all: $(OUT)

clean:
	$(RM) -r $(OUT_DIR) *.bin $(OUT)

view-%: %.bin
	hexdump -x $<

run-%: %.bin cpu
	./cpu $<

debug-cpu: $(OUT_DIR)/cpu.o $(OUT_DIR)/cpu-ui.o $(OUT_DIR)/cpu-core.o
	$(CC) `pkg-config --libs ncurses` -o $@ $^

auto-cpu: $(OUT_DIR)/auto-cpu.o $(OUT_DIR)/cpu-core.o
	$(CC) -o $@ $^

char-display: $(OUT_DIR)/char-display.o
	$(CC) -o $@ $^

display: $(OUT_DIR)/display.o
	$(CC) `pkg-config --libs raylib` -o $@ $^

$(OUT_DIR)/cpu.o: cpu/cpu.c cpu/is.h cpu/cpu-ui.h cpu/cpu-core.h
$(OUT_DIR)/auto-cpu.o: cpu/cpu.c cpu/is.h cpu/cpu-core.h
$(OUT_DIR)/cpu-core.o: cpu/cpu-core.c cpu/is.h
$(OUT_DIR)/cpu-ui.o: cpu/cpu-ui.c cpu/is.h

$(OUT_DIR)/char-display.o: devices/char-display.c cpu/is.h
$(OUT_DIR)/display.o: devices/display.c cpu/is.h

$(OUT_DIR)/%.o: */%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

%.bin: examples/%.s assembler/assembler.py
	./assembler/assembler.py $< $@

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

.NOTINTERMEDIATE: %.bin
.PHONY: all clean run run-example view-example
