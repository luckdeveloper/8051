#
# SDCC Makefile for mcs51
# 
# ------------------------------------------------------
# PATH

#
# use single makefile to compile multipule ihx and hex file
#
# 

INCDIR = ./inc
SRCDIR = ./src
OBJDIR = ./obj
IHXDIR = ./ihx
HEXDIR = ./hex
LIBDIR  =  
# ------------------------------------------------------
# Target and Source

_DEPS = platform.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

#C_SRC = $(SRCDIR)/helloworld.c
C_SRC = $(wildcard  $(SRCDIR)/*.c)
ASM_SRC = 

C_SRC_FILE = $(notdir $(C_SRC))

# This is called as substitution reference. 
# if C_SRC_FILE has values 'foo.c bar.c', C_OBJ_FILE will have 'foo.c.rel bar.c.rel'.
# if C_SRC_FILE has values 'foo.c bar.c', IHX_FILE will have 'foo.ihx bar.ihx'.
# if C_SRC_FILE has values 'foo.c bar.c', HEX_FILE will have 'foo.hex bar.hex'.
C_OBJ_FILE = $(C_SRC_FILE:%.c=%.c.rel)
IHX_FILE = $(C_SRC_FILE:%.c=%.ihx)
HEX_FILE = $(C_SRC_FILE:%.c=%.hex)

#OBJ = $(OBJDIR)/helloworld.c.rel      
OBJ = $(addprefix $(OBJDIR)/, $(C_OBJ_FILE)) 
IHX = $(addprefix $(IHXDIR)/, $(IHX_FILE))
HEX = $(addprefix $(HEXDIR)/, $(HEX_FILE))

# ------------------------------------------------------
# Usually SDCC's small memory model is the best choice.  If
# you run out of internal RAM, you will need to declare
# variables as "xdata", or switch to largeer model

# Memory Model (small, medium, large, huge)
MODEL  = small
# ------------------------------------------------------
# Memory Layout
# PRG Size = 4K Bytes
CODE_SIZE = --code-loc 0x0000 --code-size 4096
# INT-MEM Size = 256 Bytes
IRAM_SIZE = --idata-loc 0x0000  --iram-size 256
# EXT-MEM Size = 4K Bytes
XRAM_SIZE = --xram-loc 0x0000 --xram-size 4096



# ------------------------------------------------------
# SDCC

CC = sdcc
AS = sdas8051

MCU_MODEL = mcs51

#LIBS    = 
#LIBPATH = -L $(LIBDIR)

#DEBUG = --debug
AFLAGS = -l -s
CFLAGS = -I./inc -I$(INCDIR) -m$(MCU_MODEL) --model-$(MODEL) --out-fmt-ihx --no-xinit-opt $(DEBUG)
LFLAGS = $(LIBPATH) $(LIBS) -m$(MCU_MODEL) --model-$(MODEL) $(CODE_SIZE) $(IRAM_SIZE) $(XRAM_SIZE) --out-fmt-ihx  $(DEBUG)

# ------------------------------------------------------
# S = @

$(HEXDIR)/%.hex: $(IHXDIR)/%.ihx
	$(S) packihx $^ > $@

# link 
$(IHXDIR)/%.ihx: $(OBJDIR)/%.c.rel 
	$(S) $(CC) -o $@ $(LFLAGS) $^

# compile, assemble c source file
$(OBJDIR)/%.c.rel: $(SRCDIR)/%.c
	$(S) $(CC) -o $@ $(CFLAGS) -c $^

# compile, assemble asm source file
$(OBJDIR)/%.asm.rel: $(SRCDIR)/%.asm
	$(S) $(AS) $(AFLAGS) -o $@ $^ 


.PHONY: ihx hex clean

#default target
ihx: $(IHX)

hex: $(HEX)

clean:
	$(S) rm -rf $(OBJDIR)/*
	$(S) rm -rf $(IHXDIR)/*
	$(S) rm -rf $(HEXDIR)/*
