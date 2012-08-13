# Rules for making an executable program
# $Id$ 
# author: Bliu
# requeires:
#    PROG = name of exectable
#    VER_PATH = path to version number file
#    OBJS = object file names
#    NON_CLEAN_OBJS = object files not to be clean
#    LIB  = library name
#    STATIC_LIB = static library name

#Cross-compiler
ARMCC = arm-fsl-linux-gnueabi-gcc
ARMCPP = arm-fsl-linux-gnueabi-g++
ARMAR = arm-fsl-linux-gnueabi-ar
ARMRAN = arm-fsl-linux-gnueabi-ranlib

CFLAGS +=  -c -g

# Needed to activate the gdb debugger (only activate when available)
#CFLAGS += -ggdb

# Optimaztion Level #CFLAGS += -O0

# Option to put transaction hndlr into testing
# mode. Queue sizes and timeouts are adjusted.
# CFLAGS += -D TESTING

# Option to enable runtime debugging options.
# NOTE: If this is turned on the router cannot be run
# as a background process.
# CFLAGS += -D DEVELOPMENT

%.o : %.cpp
	$(ARMCPP) $(CFLAGS) $(CPPFLAGS) -o $@ $<
%.o : %.c
	$(ARMCC) $(CFLAGS) $(CPPFLAGS) -o $@ $<
%.o : %.cc
	$(ARMCPP) $(CFLAGS) $(CPPFLAGS) -o $@ $<

%.c : %.y
	bison -y -o $@ $<


ifdef PROG
OBJS += ver.o

.PHONY: mainentry
mainentry: buildver $(PROG)

.PHONY: buildver
buildver:
	@echo generating version files
	@perl $(MKRULES_DIR)/version.pl --product $(PROG_NAME) --path $(VER_PATH)
	$(ARMCC) $(CFLAGS) $(CPPFLAGS) -o ver.o ver.c
#	rm -f ./main.o
	@sleep 1

CFLAGS += -Wall
$(PROG): $(OBJS) $(NON_CLEAN_OBJS)
	$(ARMCPP) -g -o $(PROG) $(OBJS) $(NON_CLEAN_OBJS) $(LDFLAGS) $(LDADD)
	
all: $(PROG)

else
ifdef LIB
CFLAGS += -fPIC
LDFLAGS += -shared
$(LIB): $(OBJS)  $(NON_CLEAN_OBJS)
	$(ARMCPP) $(LDFLAGS) -o $(LIB) $(OBJS) $(NON_CLEAN_OBJS)
#	$(ARMAR) rc $(LIB) $(OBJS) $(OBJs)
#	$(ARMRAN) $@
#	cp $@ ../.
	
all: $(LIB)
else
ifdef STATIC_LIB
RANLIBFLAGS=
$(STATIC_LIB): $(OBJS) 
	$(ARMAR) rc lib$(STATIC_LIB) $(OBJS) $(OBJs)
	$(ARMRAN) lib$(STATIC_LIB) 
all: $(STATIC_LIB)

endif
endif
endif


clean:
	rm -rf $(PROG) $(OBJS) $(LIB) $(STATIC_LIB) 
	
