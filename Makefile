NAME = lxc-bind

SRCDIR = src
OBJDIR = obj
BINDIR = bin
TARGET = $(BINDIR)/$(NAME)

LIBS     = -liptc -llxc
STATIC   = /usr/lib/libargtable2.a
CC       = gcc
VALGRIND = valgrind
VFLAGS	 = --leak-check=yes
CFLAGS   = -g -Wall -O0

.PHONY: default all run clean

default: $(TARGET)
all: default
run: all
	@$(TARGET)
valgrind: all
	@$(VALGRIND) $(VFLAGS) $(TARGET)

SOURCES = $(wildcard $(SRCDIR)/*.c)
HEADERS = $(wildcard $(SRCDIR)/*.h)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, ${SOURCES})

$(OBJDIR)/%.o: $(SOURCES) $(HEADERS)
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJECTS) $(STATIC) -Wall $(LIBS) -o $@
	@echo "Linking complete!"

clean:
	-rm -rf $(OBJDIR)
	-rm -rf $(BINDIR)
