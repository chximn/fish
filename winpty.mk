AR = i686-w64-mingw32-ar
CXX = i686-w64-mingw32-g++
CXXFLAGS = -MMD -Wall -DUNICODE -D_UNICODE -D_WIN32_WINNT=0x0501
INCLUDE = -I include
SRC = src/winpty
BUILD = build/winpty
LIB = lib

all: $(BUILD) $(LIB) $(LIB)/libwinpty.a

$(LIB):
	mkdir -p $@

$(BUILD):
	mkdir -p $@

LIBWINPTY_OBJECTS = \
	$(BUILD)/winpty.o \
	$(BUILD)/Agent.o \
	$(BUILD)/AgentCreateDesktop.o \
	$(BUILD)/ConsoleFont.o \
	$(BUILD)/ConsoleInput.o \
	$(BUILD)/ConsoleInputReencoding.o \
	$(BUILD)/ConsoleLine.o \
	$(BUILD)/DebugShowInput.o \
	$(BUILD)/DefaultInputMap.o \
	$(BUILD)/EventLoop.o \
	$(BUILD)/InputMap.o \
	$(BUILD)/LargeConsoleRead.o \
	$(BUILD)/NamedPipe.o \
	$(BUILD)/Scraper.o \
	$(BUILD)/Terminal.o \
	$(BUILD)/Win32Console.o \
	$(BUILD)/Win32ConsoleBuffer.o \
	$(BUILD)/agent_main.o \
	$(BUILD)/BackgroundDesktop.o \
	$(BUILD)/Buffer.o \
	$(BUILD)/DebugClient.o \
	$(BUILD)/GenRandom.o \
	$(BUILD)/OwnedHandle.o \
	$(BUILD)/StringUtil.o \
	$(BUILD)/WindowsSecurity.o \
	$(BUILD)/WindowsVersion.o \
	$(BUILD)/WinptyAssert.o \
	$(BUILD)/WinptyException.o \
	$(BUILD)/WinptyVersion.o

$(LIB)/libwinpty.a: $(LIBWINPTY_OBJECTS)
	$(AR) rcs $@ $^

$(BUILD)/%.o : $(SRC)/%.cc
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $^

clean:
	-rm -rf $(BUILD)/*
	-rm $(LIB)/libwinpty.a

.PHONY: all clean