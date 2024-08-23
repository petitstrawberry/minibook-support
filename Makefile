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

$(TARGETS):
	$(MAKE) -C $@
