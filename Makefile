TARGETS = common moused keyboardd tabletmoded

.PHONY: all clean install uninstall $(TARGETS)

ll: $(TARGETS)

clean:
	for target in $(TARGETS); do \
		$(MAKE) -C $$target clean; \
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

$(TARGETS):
	$(MAKE) -C $@
