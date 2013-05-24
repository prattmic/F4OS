bin_PROGRAMS = mconf

mconf_SOURCES = mconf.c
mconf_CPPFLAGS = $(AM_CPPFLAGS)                 \
                 ${ncurses_mconf_CPPFLAGS}      \
                 $(intl_CPPFLAGS)               \
                 -I$(top_srcdir)/libs           \
                 -I$(top_srcdir)/libs/parser
mconf_CFLAGS = $(AM_CFLAGS) \
               $(kf_CFLAGS)
mconf_LDADD = $(top_builddir)/libs/parser/libkconfig-parser.la      \
              $(top_builddir)/libs/lxdialog/libkconfig-lxdialog.a   \
              $(intl_LIBS) $(ncurses_LIBS) $(mconf_EXTRA_LIBS)
