AM_CFLAGS = -Wundef \
        -Wstrict-prototypes \
        -Wno-trigraphs \
        -O2 \
        -fno-short-enums \
        -fpic \
        -I.

h_sources = online_upgrade.h
c_sources = online_upgrade.c

requiredlibs = 

bin_PROGRAMS = online_upgrade
online_upgrade_CC = @CC@
online_upgrade_SOURCES = $(c_sources) $(h_sources)
online_upgrade_CFLAGS = $(AM_CFLAGS) $(DIAG_CFLAGS) -fPIC
online_upgrade_CPPFLAGS =$(AM_CPPFLAGS)
#online_upgrade_LDFLAGS =  -lpthread -lcfg -lipc -lqrtdevui -lqrtmc -lqrtmmi -lqrtwms $(QRTCFG_LIBS) $(DIAG_LIBS) -shared -version-info 1:0:0
online_upgrade_LDFLAGS =  -lpthread -lcurl $(QRTCFG_LIBS) $(DIAG_LIBS) -shared -version-info 1:0:0
online_upgrade_LDADD = $(requiredlibs)