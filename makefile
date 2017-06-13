# Author: Thomas Daley
# Date: September 16, 2016
# Purpose: Basic Build Template for Windows Programs.
#
# Note: Microsoft Visual C++ Build Tools are assumed to be installed and 
# added to PATH.
#
# 1) vcvarsall
# 2) nmake

APPNAME = tomcat

CONSOLE_APP = /SUBSYSTEM:CONSOLE
GUI_APP = /SUBSYSTEM:WINDOWS

CC = CL
CFLAGS = /EHsc /openmp
INCS = 

LIBS = user32.lib \
	   Ws2_32.lib \
	   Mswsock.lib \
	   kernel32.lib \
	   Iphlpapi.lib
	   

LFLAGS = /link \
		 /ENTRY:mainCRTStartup

# Default target
all: clean build


build: objects
	$(CC) $(CFLAGS) /Fe$(APPNAME) $(INCS) *.obj $(LIBS) $(LFLAGS) $(CONSOLE_APP)
	DEL *.obj
	MOVE $(APPNAME).exe bin/$(APPNAME).exe

# Build object files
objects:	src/*.c* src/*.h
	$(CC) $(CFLAGS) /c $(INCS) $(CFLAGS) src/*.cc	

# Clean any leftover build files and executables
clean:
	DEL *.exe *.obj
	cd bin & DEL *.exe
