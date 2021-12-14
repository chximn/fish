CC = i686-w64-mingw32-g++
ARCH = -D MG_ARCH=MG_ARCH_WIN32
LIB = lib
LIBS = -L$(LIB) -lpthread -lwinpty -lws2_32
INCLUDE = -Iinclude
BUILD = build/windows

MONG_FLAGS = -fdata-sections -ffunction-sections
LINK_FLAGS = -Wl,--gc-sections -static

all: $(BUILD) $(BUILD)/client.exe

$(BUILD):
	mkdir -p $@

$(LIB)/libwinpty.a:
	make -j 8 -f winpty.mk

$(BUILD)/client.exe: src/client.c $(BUILD)/mongoose.o $(BUILD)/paper.o $(BUILD)/terminal.o $(LIB)/libwinpty.a
	$(CC) -o $@ $^ $(LIBS) $(INCLUDE) $(ARCH) $(LINK_FLAGS)

$(BUILD)/mongoose.o: src/mongoose.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE) $(MONG_FLAGS)

$(BUILD)/terminal.o: src/terminal.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE)

$(BUILD)/paper.o: src/paper.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE)

clean:
	make -f winpty.mk clean
	-rm -rf $(BUILD)/*

.PHONY: all clean