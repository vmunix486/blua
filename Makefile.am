SUBDIRS = src

EXTRA_DIST = \
	Makefile.nongen \
	src/Makefile.nongen \
	doc \
	examples

# Install pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = blua.pc

# Generate pkg-config file
blua.pc: Makefile
	@echo "prefix=$(prefix)" > $@
	@echo "exec_prefix=$(exec_prefix)" >> $@
	@echo "libdir=$(libdir)" >> $@
	@echo "includedir=$(includedir)" >> $@
	@echo "" >> $@
	@echo "Name: blua" >> $@
	@echo "Description: Lua 5.5 with raylib and inih bindings" >> $@
	@echo "Version: @PACKAGE_VERSION@" >> $@
	@echo "Libs: -L\$$\{libdir\} -llua" >> $@
	@echo "Cflags: -I\$$\{includedir\}" >> $@

CLEANFILES = blua.pc
