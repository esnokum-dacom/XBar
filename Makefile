CC      = gcc
TARGET  = XBar 
SRCS    = main.c drw.c src/modules/wks.c src/modules/sigr1.c
LIBS    = -lX11 -lXinerama -lImlib2 -lfontconfig $(shell pkg-config --cflags --libs xft)
CFLAGS  = -Wall -Wextra 
BINDIR  = $(HOME)/.local/bin
SRCDIR  = $(HOME)/.cache/XMenu/src
TDIR    = $(HOME)/.cache/XMenu

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

install: $(TARGET)
	mkdir -p $(BINDIR) && mkdir -p $(TDIR) && mkdir -p $(SRCDIR)
	cp $(TARGET) $(BINDIR)/$(TARGET)

clean:
	rm -f $(TARGET)

uninstall:
	rm -f $(BINDIR)/$(TARGET)
