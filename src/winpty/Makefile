# Copyright (c) 2011-2015 Ryan Prichard
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

AR=x86_64-w64-mingw32-ar
MINGW_CXX = x86_64-w64-mingw32-g++
MINGW_CXXFLAGS = -MMD -Wall \
	-DUNICODE \
	-D_UNICODE \
	-D_WIN32_WINNT=0x0501 \

LIBWINPTY_OBJECTS = \
	build/winpty.o \
	build/Agent.o \
	build/AgentCreateDesktop.o \
	build/ConsoleFont.o \
	build/ConsoleInput.o \
	build/ConsoleInputReencoding.o \
	build/ConsoleLine.o \
	build/DebugShowInput.o \
	build/DefaultInputMap.o \
	build/EventLoop.o \
	build/InputMap.o \
	build/LargeConsoleRead.o \
	build/NamedPipe.o \
	build/Scraper.o \
	build/Terminal.o \
	build/Win32Console.o \
	build/Win32ConsoleBuffer.o \
	build/agent_main.o \
	build/BackgroundDesktop.o \
	build/Buffer.o \
	build/DebugClient.o \
	build/GenRandom.o \
	build/OwnedHandle.o \
	build/StringUtil.o \
	build/WindowsSecurity.o \
	build/WindowsVersion.o \
	build/WinptyAssert.o \
	build/WinptyException.o \
	build/WinptyVersion.o

build/libwinpty.a : $(LIBWINPTY_OBJECTS)
	$(info Linking $@)
	$(AR) rcs $@ $^	

build/%.o : %.cc
	$(MINGW_CXX) $(MINGW_CXXFLAGS) -I include -c -o $@ $<

clean:
	rm -rf build/*