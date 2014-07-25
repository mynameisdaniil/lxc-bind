NAME = lxc-bind

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TARGET = $(BINDIR)/$(NAME)

LIBS = -liptc -llxc
STATIC = /usr/lib/libargtable2.a
CC = gcc
CFLAGS = -g -Wall -O0

.PHONY: default all clean

default: $(TARGET)
all: default

SOURCES = $(wildcard $(SRCDIR)/*.c)
HEADERS  = $(wildcard $(SRCDIR)/*.h)
OBJECTS  = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, ${SOURCES})

%.o: $(SOURCES) $(HEADERS)
	-mkdir $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	-mkdir $(dir $@)
	$(CC) $(OBJECTS) $(STATIC) -Wall $(LIBS) -o $@
	@echo "Linking complete!"

clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(BINDIR)
