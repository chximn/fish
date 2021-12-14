CC = g++
ARCH = -D MG_ARCH=MG_ARCH_UNIX -m32
LIBS = -lpthread -lutil
INCLUDE = -Iinclude
BUILD = build/unix

MONG_FLAGS = -fdata-sections -ffunction-sections
LINK_FLAGS = -Wl,--gc-sections -static

all: $(BUILD) $(BUILD)/client $(BUILD)/server

$(BUILD):
	mkdir -p $@

$(BUILD)/server: src/server.c $(BUILD)/mongoose.o $(BUILD)/paper.o $(BUILD)/terminal.o
	$(CC) -o $@ $^ $(ARCH) $(LIBS) $(INCLUDE) $(LINK_FLAGS)

$(BUILD)/client: src/client.c $(BUILD)/mongoose.o $(BUILD)/paper.o $(BUILD)/terminal.o
	$(CC) -o $@ $^ $(ARCH) $(LIBS) $(INCLUDE) $(LINK_FLAGS)

$(BUILD)/mongoose.o: src/mongoose.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE) $(MONG_FLAGS)

$(BUILD)/terminal.o: src/terminal.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE)

$(BUILD)/paper.o: src/paper.c
	$(CC) -c -o $@ $^ $(ARCH) $(INCLUDE)

clean: 
	-rm -rf $(BUILD)/*

.PHONY: all clean