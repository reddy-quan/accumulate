################################################################################
#
# test
#
################################################################################

TEST_VERSION = 0.01
#TEST_SITE = $(TOPDIR)/package/test/
TEST_SITE = /home/reddy/test-0.01/
TEST_SOURCE = test.c
TEST_LICENSE = GPLv2
TEST_LICENSE_FILES = 
TEST_SITE_METHOD=local

define TEST_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)
endef

define TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/test $(TARGET_DIR)/usr/sbin/test
endef

$(eval $(generic-package))
