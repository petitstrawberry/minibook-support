TARGETS = moused keyboardd tabletmoded
LIBS = common

.PHONY: all clean install uninstall $(TARGETS) $(LIBS)

all: $(TARGETS)

clean:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target clean; \
	done
	for lib in $(LIBS); do \
		$(MAKE) -C $$lib clean; \
	done

install:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target install; \
	done

uninstall:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target uninstall; \
	done

install-executable:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target install-executable; \
	done

uninstall-executable:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target uninstall-executable; \
	done

install-service:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target install-service; \
	done

uninstall-service:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target uninstall-service; \
	done

$(TARGETS): $(LIBS)
	$(MAKE) -C $@

$(LIBS):
	$(MAKE) -C $@