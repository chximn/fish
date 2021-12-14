BUILD = build
LIB = lib

all: 
	$(MAKE) -f windows.mk
	$(MAKE) -f unix.mk

clean:
	$(MAKE) clean -f windows.mk
	$(MAKE) clean -f unix.mk

	-rm -rf $(BUILD)
	-rm -rf $(LIB)

.PHONY: all clean