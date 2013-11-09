# Copyright 2008-2013 Digital Aggregates Corporation, Colorado, USA
# Licensed under the terms in README.h
# Chip Overclock
# mailto:coverclock@diag.com
# http://www.diag.com/navigation/downloads/Diminuto.html

PROJECT				=	diminuto

########## Customizations

TARGET			=	host
#TARGET			=	diminuto
#TARGET			=	arroyo
#TARGET			=	cascada
#TARGET			=	contraption
#TARGET			=	cobbler

MAJOR				=	20# API changes requiring that applications be modified.
MINOR				=	0# Only functionality or features added with no legacy API changes.
BUILD				=	3# Only bugs fixed with no API changes or new functionality.

# Some certification, defense, or intelligence agencies (e.g. the U.S. Federal
# Aviation Administration or FAA) require that software builds for safety
# critical or national security applications generate exactly the same binary
# images bit for bit if the source code has not changed. (This is theoretically
# a more stringent requirement than requiring that checksums or cryptographic
# hashes are the same, although in practice it is the same thing.) This allows
# agency inspectors to verify the integrity of the binary software images. This
# makes embedding timestamps inside compiled translation units problematic.
# If your application has this requirement, you can pass in any fixed string
# for the value of the VINTAGE make variable and you should be able to generate
# identical images with subsequent builds of Diminuto. This string is embedded
# inside the Diminuto vintage application.
VINTAGE				:=	$(shell date -u +%Y-%m-%dT%H:%M:%S.%N%z)

# This stuff all gets embedded in the vintage application.
TITLE				=	Diminuto
COPYRIGHT			=	2013 Digital Aggregates Corporation, Colorado, USA.
LICENSE				=	GNU Lesser General Public License 2.1
CONTACT				=	coverclock@diag.com
HOMEPAGE			=	http://www.diag.com/navigation/downloads/Diminuto.html

# You can change the VINFO make variable into whatever tool you use to extract
# version information from your source code control system. For example, I
# use Subversion here, and hence VINFO is the "svn info" command. But elsewhere,
# I use Git, so maybe I'd use "git describe" in those circumstances. Or maybe
# I'd have my own shell script. Or I'd just set this to echo some constant
# string. If you don't have a source code control system, don't sweat it. If
# the VINFO command fails or does not exist, all that happens is no such version
# information is included in the Diminuto vintage application.
VINFO				=	svn info
#VINFO				=	git describe

# This is where I store collateral associated with projects that I have
# downloaded off the web and use without alteration. Examples: Linux kernel
# sources, toolchains, etc.
HOME_DIR			=	$(HOME)/projects

########## Configurations

ifeq ($(TARGET),diminuto)# Build for the AT91RM9200-EK with the BuildRoot system.
ARCH				=	arm
PLATFORM			=	linux
CPPARCH				=
CARCH				=	-march=armv4t
LDARCH				=	-Bdynamic
TOOLCHAIN			=	$(ARCH)-$(PLATFORM)
CROSS_COMPILE		=	$(TOOLCHAIN)-
KERNEL_REV			=	2.6.25.10
KERNEL_DIR			=	$(HOME_DIR)/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/$(PROJECT)/builtroot/project_build_arm/$(PROJECT)/$(PLATFORM)-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),arroyo)# Build for the AT91RM9200-EK with the Arroyo custom system.
ARCH				=	arm
PLATFORM			=	linux
CPPARCH				=
CARCH				=	-march=armv4t
LDARCH				=	-Bdynamic
TOOLCHAIN			=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		=	$(TOOLCHAIN)-
KERNEL_REV			=	2.6.26.3
KERNEL_DIR			=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),cascada)# Build for the BeagleBoard C4 with the Angstrom system.
ARCH				=	arm
PLATFORM			=	linux
CPPARCH				=
CARCH				=	-mcpu=cortex-a8 -mfpu=neon -mfloat-abi=softfp -fPIC
LDARCH				=	-Bdynamic
TOOLCHAIN			=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		=	$(TOOLCHAIN)-
KERNEL_REV			=	2.6.32.7
KERNEL_DIR			=	$(HOME_DIR)/arroyo/$(PLATFORM)-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/arroyo/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),contraption)# Build for the BeagleBoard C4 with the FroYo Android 2.2 system.
ARCH				=	arm
PLATFORM			=	linux
CPPARCH				=
CARCH				=	-march=armv7-a -mfpu=neon -mfloat-abi=softfp -fPIC
#LDARCH				=	-static
LDARCH				=	-Bdynamic
TOOLCHAIN			=	$(ARCH)-none-$(PLATFORM)-gnueabi
CROSS_COMPILE		=	$(TOOLCHAIN)-
KERNEL_REV			=	2.6.32
KERNEL_DIR			=	$(HOME_DIR)/contraption/TI_Android_FroYo_DevKit-V2/Sources/Android_Linux_Kernel_2_6_32
INCLUDE_DIR			=	$(HOME_DIR)/contraption/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),cobbler)# Build for the Raspberry Pi version B with the Raspbian system.
ARCH				=	arm
PLATFORM			=	linux
CPPARCH				=
CARCH				=	
#LDARCH				=	-static
LDARCH				=	-Bdynamic
TOOLCHAIN			=	$(ARCH)-$(PLATFORM)-gnueabihf
CROSS_COMPILE		=	$(TOOLCHAIN)-
KERNEL_REV			=	rpi-3.6.y
KERNEL_DIR			=	$(HOME_DIR)/cobbler/linux-$(KERNEL_REV)
INCLUDE_DIR			=	$(HOME_DIR)/cobbler/include-$(KERNEL_REV)/include
endif

ifeq ($(TARGET),host)# Build for an Intel build server with the Ubuntu kernel.
ARCH				=	i386
PLATFORM			=	linux
CPPARCH				=
CARCH				=
LDARCH				=
TOOLCHAIN			=
CROSS_COMPILE		=
KERNEL_REV			=	3.2.0-51
KERNEL_DIR			=	/usr/src/linux-headers-$(KERNEL_REV)-generic-pae
INCLUDE_DIR			=	/usr/include
endif

########## Directory Tree

BIN_DIR				=	bin# Stripped executable binaries
DOC_DIR				=	doc# Documentation
DRV_DIR				=	drv# Loadable kernel modules
ETC_DIR				=	etc# Miscellaneous files
GEN_DIR				=	gen# Generated files
INC_DIR				=	inc# Header files
LIB_DIR				=	lib# Archives and shared objects
MOD_DIR				=	mod# Loadable user modules
OUT_DIR				=	out# Build artifacts
SRC_DIR				=	src# Library source files
SYM_DIR				=	sym# Unstripped executable binaries
TMP_DIR				=	tmp# Temporary files
TST_DIR				=	tst# Unit tests

########## Variables

HERE				:=	$(shell pwd)

OUT					=	$(OUT_DIR)/$(TARGET)

TEMP_DIR			=	/var/tmp
ROOT_DIR			=	$(HOME_DIR)/$(PROJECT)

TIMESTAMP			=	$(shell date -u +%Y%m%d%H%M%S%N%Z)
DATESTAMP			=	$(shell date +%Y%m%d)
SVNURL				=	svn://graphite/diminuto/trunk/Diminuto

PROJECT_A			=	lib$(PROJECT).a
PROJECTXX_A			=	lib$(PROJECT)xx.a
PROJECT_SO			=	lib$(PROJECT).so
PROJECTXX_SO		=	lib$(PROJECT)xx.so

PROJECT_LIB			=	$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
PROJECTXX_LIB		=	$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)

UTILITIES			=	dbdi dcscope dgdb diminuto dlib
GENERATED			=	vintage
ALIASES				=	hex oct ntohs htons ntohl htonl

TARGETOBJECTS		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.c))))
TARGETOBJECTSXX		=	$(addprefix $(OUT)/,$(addsuffix .o,$(basename $(wildcard $(SRC_DIR)/*.cpp))))
TARGETDRIVERS		=	$(addprefix $(OUT)/,$(addsuffix .ko,$(basename $(wildcard $(DRV_DIR)/*.c))))
TARGETMODULES		=	$(addprefix $(OUT)/,$(addsuffix .so,$(basename $(wildcard $(MOD_DIR)/*.c))))
TARGETSCRIPTS		=	
TARGETBINARIES		=	$(addprefix $(OUT)/,$(basename $(wildcard $(BIN_DIR)/*.c)))
TARGETGENERATED		=	$(addprefix $(OUT)/$(BIN_DIR)/,$(GENERATED))
TARGETALIASES		=	$(addprefix $(OUT)/$(BIN_DIR)/,$(ALIASES))
TARGETUNITTESTS		=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.c)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.cpp)))
TARGETUNITTESTS		+=	$(addprefix $(OUT)/,$(basename $(wildcard $(TST_DIR)/*.sh)))

TARGETARCHIVE		=	$(OUT)/$(LIB_DIR)/$(PROJECT_A)
TARGETARCHIVEXX		=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_A)
TARGETSHARED		=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR).$(MINOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO).$(MAJOR)
TARGETSHARED		+=	$(OUT)/$(LIB_DIR)/$(PROJECT_SO)
TARGETSHAREDXX		=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR).$(BUILD)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR).$(MINOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO).$(MAJOR)
TARGETSHAREDXX		+=	$(OUT)/$(LIB_DIR)/$(PROJECTXX_SO)

TARGETLIBRARIES		=	$(TARGETARCHIVE) $(TARGETSHARED)
TARGETLIBRARIESXX	=	$(TARGETARCHIVEXX) $(TARGETSHAREDXX)
TARGETPROGRAMS		=	$(TARGETSCRIPTS) $(TARGETBINARIES) $(TARGETALIASES) $(TARGETUNITTESTS) $(TARGETGENERATED)
TARGETDEFAULT		=	$(TARGETLIBRARIES) $(TARGETLIBRARIESXX) $(TARGETMODULES) $(TARGETPROGRAMS)
TARGETPACKAGE		=	$(TARGETDEFAULT) $(TARGETDRIVERS)

BUILDARTIFACTS		=	doxygen-local.cf $(ETC_DIR)/diminuto.sh dependencies.mk

COMMAND				=	dummy
SCRIPT				=	dummy

CC					=	$(CROSS_COMPILE)gcc
CXX					=	$(CROSS_COMPILE)g++
AR					=	$(CROSS_COMPILE)ar
RANLIB				=	$(CROSS_COMPILE)ranlib
STRIP				=	$(CROSS_COMPILE)strip

CDEFINES			=	-DCOM_DIAG_DIMINUTO_VINTAGE=\"$(VINTAGE)\"

ARFLAGS				=	rcv
CPPFLAGS			=	$(CPPARCH) -iquote $(INC_DIR) -isystem $(INCLUDE_DIR) $(CDEFINES)
CXXFLAGS			=	$(CARCH) -fPIC -g
CFLAGS				=	$(CARCH) -fPIC -g
#CXXFLAGS			=	$(CARCH) -fPIC -O3
#CFLAGS				=	$(CARCH) -fPIC -O3
CPFLAGS				=	-i
MVFLAGS				=	-i
LDFLAGS				=	$(LDARCH) -L$(OUT)/lib -ldiminuto -lpthread -lrt -ldl
LDXXFLAGS			=	$(LDARCH) -L$(OUT)/lib -ldiminutoxx -ldiminuto -lpthread -lrt -ldl

BROWSER				=	firefox

########## Main Entry Points

.PHONY:	default all dist clean pristine

default:	$(TARGETDEFAULT)

all:	$(UTILITIES) $(TARGETPACKAGE)

dist:	distribution

clean:
	rm -rf $(OUT) $(UTILITIES) $(BUILDARTIFACTS)
	rm -rf $(DOC_DIR)

pristine:	clean
	rm -rf $(OUT_DIR)

########## Distribution

.PHONY:	distribution

distribution:
	rm -rf $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	svn export $(SVNURL) $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD)
	( cd $(TEMP_DIR); tar cvzf - $(PROJECT)-$(MAJOR).$(MINOR).$(BUILD) ) > $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD).tgz
	( cd $(TEMP_DIR)/$(PROJECT)-$(MAJOR).$(MINOR).$(BUILD); make TARGET=host; ./out/host/bin/vintage )

########## Host Utilities

%:	$(ETC_DIR)/%.sh
	$(MAKE) TARGET=$(TARGET) script COMMAND=$@ SCRIPT=$<

$(ETC_DIR)/diminuto.sh:	Makefile
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo ARCH=\"$(ARCH)\" >> $@
	echo BDIADDRESS=\"$(BDI_IPADDR)\" >> $@
	echo BDIPORT=\"$(BDI_PORT)\" >> $@
	echo BINARIES=\"$(BINARIES_DIR)\" >> $@
	echo BUILDROOT=\"$(BUILDROOT_DIR)\" >> $@
	echo CONFIG=\"$(CONFIG_DIR)\" >> $@
	echo CROSS_COMPILE=\"$(CROSS_COMPILE)\" >> $@
	echo DATESTAMP=\"$(DATESTAMP)\" >> $@
	echo TOOLCHAIN=\"$(TOOLCHAIN_DIR)\" >> $@
	echo DIMINUTO=\"$(DIMINUTO_DIR)\" >> $@
	echo DESPERADO=\"$(DESPERADO_DIR)\" >> $@
	echo FICL=\"$(FICL_DIR)\" >> $@
	echo IMAGE=\"$(IMAGE)\" >> $@
	echo KERNEL=\"$(KERNEL_DIR)\" >> $@
	echo PLATFORM=\"$(PLATFORM)\" >> $@
	echo PROJECT=\"$(PROJECT)\" >> $@
	echo RELEASE=\"$(KERNEL_REV)\" >> $@
	echo TARGET=\"$(TARGET)\" >> $@
	echo TFTP=\"$(TFTP_DIR)\" >> $@
	echo TGTADDRESS=\"$(TGT_IPADDR)\" >> $@
	echo TMPDIR=\"$(TEMP_DIR)\" >> $@
	echo 'echo $${PATH} | grep -q "$(TOOLBIN_DIR)" || export PATH=$(TOOLBIN_DIR):$${PATH}' >> $@
	echo 'echo $${PATH} | grep -q "$(LOCALBIN_DIR)" || export PATH=$(LOCALBIN_DIR):$${PATH}' >> $@

########## Target C Libraries

$(OUT)/$(LIB_DIR)/lib$(PROJECT).a:	$(TARGETOBJECTS)
	test -d $(OUT)/$(LIB_DIR) || mkdir -p $(OUT)/$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).a
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD) -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT).so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 
	
########## Target C++ Libraries

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.a:	$(TARGETOBJECTSXX)
	test -d $(OUT)/$(LIB_DIR) || mkdir -p $(OUT)/$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.a
	$(CC) $(CARCH) -shared -Wl,-soname,lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD) -o $@ -Wl,--whole-archive $< -Wl,--no-whole-archive

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so:	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR):	$(OUT)/$(LIB_DIR)/lib$(PROJECT)xx.so.$(MAJOR).$(MINOR).$(BUILD)
	D=`dirname $<`; F=`basename $<`; T=`basename $@`; ( cd $$D; ln -s -f $$F $$T ) 

########## Target Unstripped Binaries

$(OUT)/$(SYM_DIR)/%:	$(BIN_DIR)/%.c $(TARGETLIBRARIES)
	test -d $(OUT)/$(SYM_DIR) || mkdir -p $(OUT)/$(SYM_DIR)
	$(CC) $(CPPFLAGS) -I $(KERNEL_DIR)/include $(CFLAGS) -o $@ $< $(LDFLAGS)
	
########## Target Aliases

$(OUT)/$(BIN_DIR)/hex $(OUT)/$(BIN_DIR)/oct $(OUT)/$(BIN_DIR)/ntohs $(OUT)/$(BIN_DIR)/htons $(OUT)/$(BIN_DIR)/ntohl $(OUT)/$(BIN_DIR)/htonl:	$(OUT)/$(BIN_DIR)/dec
	ln -f $< $@
	
########## Unit Tests

$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.c $(TARGETLIBRARIES)
	test -d $(OUT)/$(TST_DIR) || mkdir -p $(OUT)/$(TST_DIR)
	$(CC) -rdynamic $(CPPFLAGS) $(CFLAGS) -o $@ $< $(LDFLAGS)
	
$(OUT)/$(TST_DIR)/%:	$(TST_DIR)/%.cpp $(TARGETLIBRARIESXX) $(TARGETLIBRARIES)
	test -d $(OUT)/$(TST_DIR) || mkdir -p $(OUT)/$(TST_DIR)
	$(CXX) -rdynamic $(CPPFLAGS) $(CXXFLAGS) -o $@ $< $(LDXXFLAGS)

########## Generated

.PHONY:	$(OUT)/$(GEN_DIR)/vintage.c $(INC_DIR)/com/diag/$(PROJECT)/diminuto_release.h $(INC_DIR)/com/diag/$(PROJECT)/diminuto_vintage.h

# For embedding in a system where it can be executed from a shell.
$(OUT)/$(GEN_DIR)/vintage.c:	$(INC_DIR)/com/diag/$(PROJECT)/diminuto_release.h $(INC_DIR)/com/diag/$(PROJECT)/diminuto_vintage.h
	D=`dirname $@`; test -d $$D || mkdir -p $$D	
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#include "com/diag/diminuto/diminuto_release.h"' >> $@
	echo '#include "com/diag/diminuto/diminuto_release.h"' >> $@
	echo '#include "com/diag/diminuto/diminuto_vintage.h"' >> $@
	echo '#include "com/diag/diminuto/diminuto_vintage.h"' >> $@
	echo '#include <stdio.h>' >> $@
	echo 'static const char METADATA[] =' >> $@
	echo '"METADATA_BEGIN\n"' >> $@
	echo "\"Title: $(TITLE)\\n\"" >> $@
	echo "\"Copyright: $(COPYRIGHT)\\n\"" >> $@
	echo "\"Contact: $(CONTACT)\\n\"" >> $@
	echo "\"License: $(LICENSE)\\n\"" >> $@
	echo "\"Homepage: $(HOMEPAGE)\\n\"" >> $@
	echo "\"Release: $(MAJOR).$(MINOR).$(BUILD)\\n\"" >> $@
	echo "\"Vintage: $(VINTAGE)\\n\"" >> $@
	echo "\"Host: $(shell hostname)\\n\"" >> $@
	echo "\"Directory: $(shell pwd)\\n\"" >> $@
	echo "\"Arch: $(ARCH)\\n\"" >> $@
	echo "\"Target: $(TARGET)\\n\"" >> $@
	echo "\"Platform: $(PLATFORM)\\n\"" >> $@
	echo "\"Toolchain: $(TOOLCHAIN)\\n\"" >> $@
	$(VINFO) | sed 's/"/\\"/g' | awk '/^$$/ { next; } { print "\""$$0"\\n\""; }' >> $@ || true
	echo '"METADATA_END\n";' >> $@
	echo 'int main(void) { fputs(METADATA, stdout); fputs("$(MAJOR).$(MINOR).$(BUILD)\n", stderr); return 0; }' >> $@

# For embedding in an application where it can be interrogated or displayed.
$(INC_DIR)/com/diag/$(PROJECT)/diminuto_release.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_RELEASE_' >> $@
	echo "static const char RELEASE[] = \"RELEASE=$(MAJOR).$(MINOR).$(BUILD)\";" >> $@
	echo '#endif' >> $@

# For embedding in an application where it can be interrogated or displayed.
$(INC_DIR)/com/diag/$(PROJECT)/diminuto_vintage.h:
	echo '/* GENERATED FILE! DO NOT EDIT! */' > $@
	echo '#ifndef _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo '#define _H_COM_DIAG_DIMINUTO_VINTAGE_' >> $@
	echo "static const char VINTAGE[] = \"VINTAGE=$(VINTAGE)\";" >> $@
	echo '#endif' >> $@

$(OUT)/$(SYM_DIR)/vintage:	$(OUT)/$(GEN_DIR)/vintage.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $<

########## Modules

LDWHOLEARCHIVES=# These archives will be linked into the shared object in their entirety.

$(OUT)/$(MOD_DIR)/%.so:	$(MOD_DIR)/%.c
	test -d $(OUT)/$(MOD_DIR) || mkdir -p $(OUT)/$(MOD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -shared $< $(LDFLAGS) -Wl,--whole-archive $(LDWHOLEARCHIVES) -Wl,--no-whole-archive

########## Drivers

.PHONY:	drivers drivers-clean

OBJ_M =	$(addsuffix .o,$(basename $(shell cd $(DRV_DIR); ls *.c)))

$(OUT)/$(TMP_DIR)/Makefile:	Makefile
	test -d $(OUT)/$(TMP_DIR) || mkdir -p $(OUT)/$(TMP_DIR)
	echo "# GENERATED FILE! DO NOT EDIT!" > $@
	echo "obj-m := $(OBJ_M)" >> $@
	echo "EXTRA_CFLAGS := -iquote $(HERE)/$(INC_DIR) -iquote $(HERE)/$(TST_DIR)" >> $@
	#echo "EXTRA_CFLAGS := -iquote $(HERE)/$(INC_DIR) -iquote $(HERE)/$(TST_DIR) -DDEBUG" >> $@

$(OUT)/$(TMP_DIR)/%.c:	$(DRV_DIR)/%.c
	test -d $(OUT)/$(TMP_DIR) || mkdir -p $(OUT)/$(TMP_DIR)
	cp $< $@

$(OUT)/$(DRV_DIR)/%.ko:	$(OUT)/$(TMP_DIR)/%.ko
	test -d $(OUT)/$(DRV_DIR) || mkdir -p $(OUT)/$(DRV_DIR)
	cp $< $@

$(OUT)/$(TMP_DIR)/diminuto_mmdriver.ko $(OUT)/$(TMP_DIR)/diminuto_utmodule.ko $(OUT)/$(TMP_DIR)/diminuto_kernel_datum.ko $(OUT)/$(TMP_DIR)/diminuto_kernel_map.ko:	$(OUT)/$(TMP_DIR)/Makefile $(OUT)/$(TMP_DIR)/diminuto_mmdriver.c $(OUT)/$(TMP_DIR)/diminuto_utmodule.c $(OUT)/$(TMP_DIR)/diminuto_kernel_datum.c $(OUT)/$(TMP_DIR)/diminuto_kernel_map.c
	make -C $(KERNEL_DIR) M=$(shell cd $(OUT)/$(TMP_DIR); pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) modules

drivers-clean:
	make -C $(KERNEL_DIR) M=$(shell cd $(OUT)/$(TMP_DIR); pwd) CROSS_COMPILE=$(CROSS_COMPILE) ARCH=$(ARCH) clean
	rm -f $(OUT)/$(TMP_DIR)/Makefile

########## Helpers

.PHONY:	backup package

backup:	../$(PROJECT).bak.tgz
	mv $(MVFLAGS) ../$(PROJECT).bak.tgz ../$(PROJECT).$(TIMESTAMP).tgz

../$(PROJECT).bak.tgz:
	tar cvzf - . > ../diminuto.bak.tgz

package $(PROJECT).tgz:
	tar -C $(OUT) -cvzf - ./bin ./drv ./lib ./mod ./tst > $(PROJECT).tgz

########## Documentation

.PHONY:	documentation browse refman manpages

documentation:
	sed -e "s/\\\$$Name.*\\\$$/$(MAJOR).$(MINOR).$(BUILD)/" < doxygen.cf > doxygen-local.cf
	doxygen doxygen-local.cf
	test -d $(DOC_DIR)/pdf || mkdir -p $(DOC_DIR)/pdf
	( cd $(DOC_DIR)/latex; $(MAKE) refman.pdf; cp refman.pdf ../pdf )
	cat $(DOC_DIR)/man/man3/*.3 | groff -man -Tps - > $(DOC_DIR)/pdf/manpages.ps
	ps2pdf $(DOC_DIR)/pdf/manpages.ps $(DOC_DIR)/pdf/manpages.pdf

browse:
	$(BROWSER) file:doc/html/index.html

refman:
	$(BROWSER) file:doc/pdf/refman.pdf

manpages:
	$(BROWSER) file:doc/pdf/manpages.pdf

########## Submakes

.PHONY:	script patch

script:	$(COMMAND)

$(COMMAND):	$(SCRIPT)
	cp $< $@
	chmod 755 $@

patch:	$(OLD) $(NEW)
	diff -purN $(OLD) $(NEW)

########## Rules

$(OUT)/%.txt:	%.cpp
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CXX) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.cpp
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(OUT)/%.txt:	%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) -E $(CPPFLAGS) -c $< > $@

$(OUT)/%.o:	%.c
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $<
	
.SECONDARY:

$(OUT)/$(BIN_DIR)/%:	$(OUT)/$(SYM_DIR)/%
	D=`dirname $@`; test -d $$D || mkdir -p $$D
	$(STRIP) -o $@ $<

########## Dependencies

.PHONY:	depend

depend:
	cp /dev/null dependencies.mk
	for S in $(SRC_DIR) $(MOD_DIR) $(DRV_DIR) $(TST_DIR); do \
		for F in $$S/*.c; do \
			D=`dirname $$F`; \
			echo -n "$(OUT)/$$D/" >> dependencies.mk; \
			$(CC) $(CPPFLAGS) -MM -MG $$F >> dependencies.mk; \
		done; \
	done
	for S in $(SRC_DIR) $(TST_DIR); do \
		for F in $$S/*.cpp; do \
			D=`dirname $$F`; \
			echo -n "$(OUT)/$$D/" >> dependencies.mk; \
			$(CXX) $(CPPFLAGS) -MM -MG $$F >> dependencies.mk; \
		done; \
	done

-include dependencies.mk
