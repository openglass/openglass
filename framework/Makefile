SUBDIRS = common network

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

.PHONY: clean
clean: 
	for dir in $(SUBDIRS); do \
		(cd "$$dir"; $(MAKE) clean) \
	done

