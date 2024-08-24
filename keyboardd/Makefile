COMPILER  = gcc
CFLAGS    = -g -MMD -MP -Wall -Wextra -Winit-self -Wno-missing-field-initializers
ifeq "$(shell getconf LONG_BIT)" "64"
  LDFLAGS =
else
  LDFLAGS =
endif
LIBS      = ../common/bin/libcommon.a
INCLUDE   = -I./include -I../common/include
TARGET    = ./bin/$(shell basename `readlink -f .`) ../common/bin/$(shell basename `readlink -f ../common`)
SRCDIR    = ./src
ifeq "$(strip $(SRCDIR))" ""
  SRCDIR  = .
endif
SOURCES   = $(wildcard $(SRCDIR)/*.c)
OBJDIR    = ./obj
ifeq "$(strip $(OBJDIR))" ""
  OBJDIR  = .
endif
OBJECTS   = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.c=.o)))
DEPENDS   = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS) $(LIBS)
	-mkdir -p $(dir $@)
	$(COMPILER) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	-mkdir -p $(OBJDIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -o $@ -c $<

all: clean $(TARGET)

clean:
	-rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

.PHONY: install
install: $(TARGET)
	chmod +x ./install/install.sh
	./install/install.sh

.PHONY: uninstall
uninstall:
	chmod +x ./install/uninstall.sh
	./install/uninstall.sh

.PHONY: install-executable
install-executable:
	chmod +x ./install/install-executable.sh
	./install/install-executable.sh

.PHONY: uninstall-executable
uninstall-executable:
	chmod +x ./install/uninstall-executable.sh
	./install/uninstall-executable.sh

.PHONY: install-service
install-service:
	chmod +x ./install/install-service.sh
	./install/install-service.sh

.PHONY: uninstall-service
uninstall-service:
	chmod +x ./install/uninstall-service.sh
	./install/uninstall-service.sh

-include $(DEPENDS)
