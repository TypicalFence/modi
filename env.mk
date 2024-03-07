# ------------------------------------------------
# This file is sets up the environment for make.
# Generally you shouldn't edit it.
# It also sets "required" C flags such as -std
# ------------------------------------------------

# If no compiler is specified try figuring out what to use
ifndef C
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# default to gcc under Linux, users can still overwrite that
		CC=gcc
	endif
	ifeq ($(UNAME_S),Darwin)
		# assume clang under Macos
		CC=clang
	endif
endif

# The project is written for C99, also enable all warnings
CFLAGS := -std=c99 -Wall 

ifeq ($(DEBUG), 1)
	CFLAGS += -g
endif