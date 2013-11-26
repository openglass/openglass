SUBDIRS = framework datasource model 

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS): 
	$(MAKE) -C $@

.PHONY: clean
clean: 
	for dir in $(SUBDIRS); do \
		(cd "$$dir"; $(MAKE) clean) \
	done

model: datasource

datasource: framework

