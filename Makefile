#
# Top Makefile
#

#TOPDIR = $(shell dirname $(shell pwd)/$(lastword $(MAKEFILE_LIST)))

#.PHONY: all clean

#all:
#	@TOPDIR="$(TOPDIR)" $(MAKE) -s -C src all

#clean:
#	@TOPDIR="$(TOPDIR)" $(MAKE) -s -C src clean

.PHONY: all clean module mclean

all:
	@$(MAKE) -C src all

clean:
	@$(MAKE) -C src clean
	
module:
	@$(MAKE) -C src module

mclean:
	@$(MAKE) -C src mclean