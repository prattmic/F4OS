# Copyright 1994-2011 The MathWorks, Inc.
#
# File    : xrt_unix.tmf
#
# Abstract:
#	Code generation template makefile for building a UNIX-based, stand-
#	alone real-time version of MATLAB-code using generated C/C++ code.
#
# 	This makefile attempts to conform to the guidelines specified in the
# 	IEEE Std 1003.2-1992 (POSIX) standard. It is designed to be used
#       with GNU Make which is located in matlabroot/rtw/bin.
#
# 	Note that this template is automatically customized by the
#	code generation build procedure to create "<model>.mk"
#
#       The following defines can be used to modify the behavior of the
#	build:
#	  OPT_OPTS       - Optimization options. Default is -O. To enable
#                          debugging specify as OPT_OPTS=-g.
#                          Because of optimization problems in IBM_RS,
#                          default is no-optimization.
#	  CPP_OPTS       - C++ compiler options.
#	  OPTS           - User specific compile options.
#	  USER_SRCS      - Additional user sources, such as files needed by
#			   S-functions.
#	  USER_INCLUDES  - Additional include paths
#			   (i.e. USER_INCLUDES="-Iwhere-ever -Iwhere-ever2")
#

#------------------------ Macros read by make_rtw ------------------------------
#
# The following macros are read by the code generation build procedure:
#
#  MAKECMD         - This is the command used to invoke the make utility
#  HOST            - What platform this template makefile is targeted for
#                    (i.e. PC or UNIX)
#  BUILD           - Invoke make from thecode generation build procedure
#                    (yes/no)?
#  SYS_TARGET_FILE - Name of system target file.

MAKECMD         = make
HOST            = UNIX
BUILD           = yes
SYS_TARGET_FILE = ert.tlc
COMPILER_TOOL_CHAIN = unix


#---------------------- Tokens expanded by make_rtw ----------------------------
#
# The following tokens, when wrapped with "|>" and "<|" are expanded by the
# code generation build procedure.
#
#  MODEL_NAME          - Name of the Simulink block diagram
#  MODEL_MODULES       - Any additional generated source modules
#  MAKEFILE_NAME       - Name of makefile created from template makefile <model>.mk
#  MATLAB_ROOT         - Path to were MATLAB is installed.
#  S_FUNCTIONS         - List of S-functions.
#  S_FUNCTIONS_LIB     - List of S-functions libraries to link.
#  SOLVER              - Solver source file name
#  NUMST               - Number of sample times
#  TID01EQ             - yes (1) or no (0): Are sampling rates of continuous task
#                        (tid=0) and 1st discrete task equal.
#  NCSTATES            - Number of continuous states
#  COMPUTER            - Computer type. See the MATLAB computer command.
#  BUILDARGS           - Options passed in at the command line.
#  MULTITASKING        - yes (1) or no (0): Is solver mode multitasking
#  EXT_MODE            - yes (1) or no (0): Build for external mode
#  TMW_EXTMODE_TESTING - yes (1) or no (0): Build ext_test.c for external mode
#                        testing.
#  EXTMODE_TRANSPORT   - Index of transport mechanism (e.g. tcpip, serial) for extmode
#  EXTMODE_STATIC      - yes (1) or no (0): Use static instead of dynamic mem alloc.
#  EXTMODE_STATIC_SIZE - Size of static memory allocation buffer.
#  GEN_MDLREF_SFCN     - (1/0): are we generating model reference wrapper s-function
#  TGT_FCN_LIB         - Target Funtion library to use
#  MODELREFS           - List of referenced models

MODEL                = kalman
MODULES              = kalman_data.c kalman_initialize.c kalman_terminate.c kalman.c inv.c rt_nonfinite.c rtGetNaN.c rtGetInf.c 
MAKEFILE             = kalman_rtw.mk
MATLAB_ROOT          = /usr/local/MATLAB/R2012a
ALT_MATLAB_ROOT      = /usr/local/MATLAB/R2012a
S_FUNCTIONS          = 
S_FUNCTIONS_LIB      = 
SOLVER               = 
NUMST                = 1
TID01EQ              = 0
NCSTATES             = 0
COMPUTER             = GLNXA64
BUILDARGS            =  GENERATE_REPORT=1 ADD_MDL_NAME_TO_GLOBALS=0
MULTITASKING         = 0
EXT_MODE             = 0
TMW_EXTMODE_TESTING  = 0
EXTMODE_TRANSPORT    = 0
EXTMODE_STATIC       = 0
EXTMODE_STATIC_SIZE  = 1000000
MEXEXT               = mexa64
TGT_FCN_LIB          = GNU
MODELREFS            = 
SHARED_SRC           = 
SHARED_SRC_DIR       = 
SHARED_BIN_DIR       = 
SHARED_LIB           = 
TARGET_LANG_EXT      = c
OPTIMIZATION_FLAGS   = -O0 -fPIC
ADDITIONAL_LDFLAGS   = 

#--------------------------- Model and reference models -----------------------
MODELLIB                  = kalman.a
MODELREF_LINK_LIBS        = 
MODELREF_INC_PATH         = 
RELATIVE_PATH_TO_ANCHOR   = /mnt/prog/embedded/f4os/usr/flight/matlab
# NONE: standalone, SIM: modelref sim, RTW: modelref code-generation
MODELREF_TARGET_TYPE       = rtw:lib

#-- In the case when directory name contains space ---
ifneq ($(MATLAB_ROOT),$(ALT_MATLAB_ROOT))
MATLAB_ROOT := $(ALT_MATLAB_ROOT)
endif

#--------------------------- Tool Specifications -------------------------------

include $(MATLAB_ROOT)/rtw/c/tools/unixtools.mk

#------------------------------ Include Path -----------------------------------

MATLAB_INCLUDES = \
	-I$(MATLAB_ROOT)/simulink/include \
	-I$(MATLAB_ROOT)/extern/include \
	-I$(MATLAB_ROOT)/rtw/c/src \
	-I$(MATLAB_ROOT)/rtw/c/src/ext_mode/common
  
# Additional file include paths
ADD_INCLUDES = \
	-I/mnt/prog/embedded/f4os/usr/flight/matlab/codegen/lib/kalman \
	-I/mnt/prog/embedded/f4os/usr/flight/matlab \


SHARED_INCLUDES =
ifneq ($(SHARED_SRC_DIR),)
SHARED_INCLUDES = -I$(SHARED_SRC_DIR) 
endif

INCLUDES = -I. -I$(RELATIVE_PATH_TO_ANCHOR) $(MATLAB_INCLUDES) $(ADD_INCLUDES) $(USER_INCLUDES) \
	$(INSTRUMENT_INCLUDES)  $(MODELREF_INC_PATH) $(SHARED_INCLUDES)

#----------------------------- External mode -----------------------------------
# Uncomment -DVERBOSE to have information printed to stdout
# To add a new transport layer, see the comments in
#   <matlabroot>/toolbox/simulink/simulink/extmode_transports.m
ifeq ($(EXT_MODE),1)
  EXT_CC_OPTS = -DEXT_MODE -D$(COMPUTER) #-DVERBOSE
  EXT_LIB     =
  EXT_SRC     =
  LINT_EXT_COMMON_SRC =
  LINT_EXT_TCPIP_SRC  =
  ifeq ($(EXTMODE_TRANSPORT),0) #tcpip
    EXT_SRC = updown.c rtiostream_interface.c rtiostream_tcpip.c
    LINT_EXT_COMMON_SRC = ext_svr.c updown.c ext_work.c \
	rtiostream_interface.c
    LINT_EXT_TCPIP_SRC  = rtiostream_tcpip.c
  endif
  ifeq ($(EXTMODE_TRANSPORT),1) #serial_win32
    err:
	@echo
	@echo "### ERROR: External mode serial transport only available on win32"
	@echo
  endif
  ifeq ($(TMW_EXTMODE_TESTING),1)
    EXT_SRC     += ext_test.c
    EXT_CC_OPTS += -DTMW_EXTMODE_TESTING
  endif
  ifeq ($(EXTMODE_STATIC),1)
    EXT_SRC             += mem_mgr.c
    LINT_EXT_COMMON_SRC += mem_mgr.c
    EXT_CC_OPTS         += -DEXTMODE_STATIC -DEXTMODE_STATIC_SIZE=$(EXTMODE_STATIC_SIZE)
  endif
  ifeq ($(COMPUTER),SOL64)
    EXT_LIB = -lsocket -lnsl
  endif
endif

#----------------------------- Real-Time Model ---------------------------------
RTM_CC_OPTS = -DUSE_RTMODEL

#-------------------------------- C Flags --------------------------------------

# Optimization Options
ifndef OPT_OPTS
OPT_OPTS = $(DEFAULT_OPT_OPTS)
endif

# General User Options
OPTS =

# Compiler options, etc:
ifneq ($(OPTIMIZATION_FLAGS),)
CC_OPTS = $(OPTS) $(EXT_CC_OPTS) $(RTM_CC_OPTS) $(OPTIMIZATION_FLAGS)
else
CC_OPTS = $(OPT_OPTS) $(OPTS) $(EXT_CC_OPTS) $(RTM_CC_OPTS)
endif


CPP_REQ_DEFINES = -DMODEL=$(MODEL) -DRT -DNUMST=$(NUMST) \
                  -DTID01EQ=$(TID01EQ) -DNCSTATES=$(NCSTATES) -DUNIX \
                  -DMT=$(MULTITASKING) -DHAVESTDIO 


CFLAGS   = $(ANSI_OPTS) $(CC_OPTS) $(CPP_REQ_DEFINES) $(INCLUDES)
CPPFLAGS = $(CPP_ANSI_OPTS) $(CPP_OPTS) $(CC_OPTS) $(CPP_REQ_DEFINES) $(INCLUDES)
#-------------------------- Additional Libraries ------------------------------

SYSLIBS = $(EXT_LIB) -lm 

LIBS =
 
LIBS += $(S_FUNCTIONS_LIB) $(INSTRUMENT_LIBS)

#----------------------------- Source Files ------------------------------------

USER_SRCS =

USER_OBJS       = $(addsuffix .o, $(basename $(USER_SRCS)))
LOCAL_USER_OBJS = $(notdir $(USER_OBJS))


SRCS = $(MODULES) $(S_FUNCTIONS)

ifeq ($(MODELREF_TARGET_TYPE), NONE)
    PRODUCT            = $(RELATIVE_PATH_TO_ANCHOR)/$(MODEL)
    BIN_SETTING        = $(LD) $(LDFLAGS) $(ADDITIONAL_LDFLAGS) -o $(PRODUCT) $(SYSLIBS)
    BUILD_PRODUCT_TYPE = "executable"
    SRCS               += $(EXT_SRC) $(SOLVER)
else ifeq ($(MODELREF_TARGET_TYPE), rtw:dll)
    # Shared library code-generation target
    PRODUCT            = $(MODELLIB)
    BUILD_PRODUCT_TYPE = "shared library"
else
    # Static library code-generation target
    PRODUCT            = $(MODELLIB)
    BUILD_PRODUCT_TYPE = "static library"
endif

ifneq ($(findstring .cpp,$(suffix $(SRCS), $(USER_SRCS))),)
  LD = $(CPP)
  SYSLIBS += $(CPP_SYS_LIBS) 
endif

OBJS      = $(addsuffix .o, $(basename $(SRCS))) $(USER_OBJS)
LINK_OBJS = $(addsuffix .o, $(basename $(SRCS))) $(LOCAL_USER_OBJS)

SHARED_SRC := $(wildcard $(SHARED_SRC))
SHARED_OBJS = $(addsuffix .o, $(basename $(SHARED_SRC)))

#------------- Test Compile using gcc -Wall to look for warnings ---------------
#
# DO_GCC_TEST=1 runs gcc with compiler warning flags on all the source files
# used in this build. This includes the generated code, and any user source
# files needed for the build and placed in this directory.
#
# WARN_ON_GLNX=1 runs the linux compiler with warnings flags. On hand-written
# code we use the max amount of flags availabe while on the generated code, a
# few less.
#
# See rtw/c/tools/unixtools.mk for the definition of GCC_WARN_OPTS
GCC_TEST_CMD  := echo
GCC_TEST_OUT  := > /dev/null
ifeq ($(DO_GCC_TEST), 1)
  GCC_TEST := gcc -c -o /dev/null $(GCC_WARN_OPTS_MAX) $(CPP_REQ_DEFINES) \
                                     $(INCLUDES)
  GCC_TEST_CMD := echo; echo "\#\#\# GCC_TEST $(GCC_TEST) $<"; $(GCC_TEST)
  GCC_TEST_OUT := ; echo
endif

#----------------------------- Lint (sol2 only) --------------------------------

LINT_SRCS = 
LINT_SRCS += $(MODULES) $(USER_SRCS) $(S_FUNCTIONS)
LINTOPTSFILE = $(MODEL).lintopts

LINT_ERROFF1 = E_NAME_DEF_NOT_USED2,E_NAME_DECL_NOT_USED_DEF2
LINT_ERROFF2 = $(LINT_ERROFF1),E_FUNC_ARG_UNUSED
LINT_ERROFF  = $(LINT_ERROFF2),E_INDISTING_FROM_TRUNC2,E_NAME_USED_NOT_DEF2

#--------------------------------- Rules ---------------------------------------
ifeq ($(MODELREF_TARGET_TYPE),NONE)
#--- Stand-alone executable target ---
$(PRODUCT) : $(OBJS) $(SHARED_LIB) $(LIBS) $(MODELREF_LINK_LIBS)
	$(BIN_SETTING) $(LINK_OBJS) $(MODELREF_LINK_LIBS) $(SHARED_LIB) $(LIBS) 
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
else ifeq ($(MODELREF_TARGET_TYPE), rtw:dll)
#--- Shared library target ---
$(PRODUCT) : $(OBJS) $(SHARED_LIB)
	@echo "### Archiving ..."
	$(LD) -shared -o $(MODELLIB) $(LINK_OBJS)
	@echo "### Created $(MODELLIB)"
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
else
#--- Static library target ---
$(PRODUCT) : $(OBJS) $(SHARED_LIB)
	@rm -f $(MODELLIB)
	ar ruvs $(MODELLIB) $(LINK_OBJS)
	@echo "### Created $(MODELLIB)"
	@echo "### Created $(BUILD_PRODUCT_TYPE): $@"
endif

ifneq ($(SHARED_SRC_DIR),)
$(SHARED_BIN_DIR)/%.o : $(SHARED_SRC_DIR)/%.c
	cd $(SHARED_BIN_DIR); $(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) $(notdir $?)

$(SHARED_BIN_DIR)/%.o : $(SHARED_SRC_DIR)/%.cpp
	cd $(SHARED_BIN_DIR); $(CPP) -c $(CPPFLAGS) $(GCC_WALL_FLAG_MAX) $(notdir $?)
endif

%.o : %.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG) "$<"

%.o : %.cpp
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CPP) -c $(CPPFLAGS) $(GCC_WALL_FLAG) "$<"

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG) "$<"

%.o : $(RELATIVE_PATH_TO_ANCHOR)/%.cpp
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CPP) -c $(CPPFLAGS) "$<"





%.o : $(MATLAB_ROOT)/simulink/src/%.c
	@$(GCC_TEST_CMD) $< $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) $<

%.o : $(MATLAB_ROOT)/simulink/src/%.cpp
	@$(GCC_TEST_CMD) $< $(GCC_TEST_OUT)
	$(CPP) -c $(CPPFLAGS) $<

%.o : $(MATLAB_ROOT)/rtw/c/src/%.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) "$<"

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/common/%.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) "$<"

%.o : $(MATLAB_ROOT)/rtw/c/src/rtiostream/rtiostreamtcpip/%.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) "$<"

%.o : $(MATLAB_ROOT)/rtw/c/src/ext_mode/custom/%.c
	@$(GCC_TEST_CMD) "$<" $(GCC_TEST_OUT)
	$(CC) -c $(CFLAGS) $(GCC_WALL_FLAG_MAX) "$<"

#------------------------------- Libraries -------------------------------------





#----------------------------- Dependencies ------------------------------------

$(OBJS) : $(MAKEFILE) rtw_proj.tmw

$(SHARED_LIB) : $(SHARED_OBJS)
	@echo "### Creating $@ "
	ar ruvs $@ $(SHARED_OBJS)
	@echo "### $@ Created  "


#--------- Miscellaneous rules to purge, clean and lint (sol2 only) ------------

purge : clean
	@echo "### Deleting the generated source code for $(MODEL)"
	@\rm -f $(MODULES) rtw_proj.tmw $(MAKEFILE)

clean :
	@echo "### Deleting the objects and $(PRODUCT)"
	@\rm -f $(LINK_OBJS) $(PRODUCT)

lint  : rtwlib.ln
	@lint -errchk -errhdr=%user -errtags=yes -F -L. -lrtwlib -x -Xc \
	      -erroff=$(LINT_ERROFF) \
	      -D_POSIX_C_SOURCE $(CFLAGS) $(LINT_SRCS)
	@\rm -f $(LINTOPTSFILE)
	@echo
	@echo "### Created lint output only, no executable"
	@echo

rtwlib.ln : $(MAKEFILE) rtw_proj.tmw
	@echo
	@echo "### Linting ..."
	@echo
	@\rm -f llib-lrtwlib.ln $(LINTOPTSFILE)
	@echo "-dirout=. -errchk -errhdr=%user " >> $(LINTOPTSFILE)
	@echo "-errtags -F -ortwlib -x -Xc " >> $(LINTOPTSFILE)
	@echo "-erroff=$(LINT_ERROFF) " >> $(LINTOPTSFILE)
	@echo "-D_POSIX_C_SOURCE $(CFLAGS) " >> $(LINTOPTSFILE)
	lint -flagsrc=$(LINTOPTSFILE)

# EOF: ert_unix.tmf
