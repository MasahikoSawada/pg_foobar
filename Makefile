# pg_foobar

MODULE_big = pg_foobar
OBJS = pg_foobar.o $(WIN32RES)

EXTENSION = pg_foobar
DATA = pg_foobar--1.0.sql
PGFILEDESC = "pg_foobar - foo bar game"

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = pscan
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
