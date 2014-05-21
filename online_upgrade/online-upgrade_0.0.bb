inherit autotools

DESCRIPTION = "QMI shutdown modem Daemon"
LICENSE = "QUALCOMM-Proprietary"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta-qcom/files/qcom-licenses/\
QUALCOMM-Proprietary;md5=92b1d0ceea78229551577d4284669bb8"
PR = "r1"

SRC_URI = "file://${WORKSPACE}/arccra/application/apps/online_upgrade"

DEPENDS = arccra-common curl
#RDEPENDS = qmi-framework

S = "${WORKDIR}/online_upgrade"

do_install_append () {
	# install -d ${D}${libdir}
	# oe_libinstall -so libipc ${D}${libdir}
	# oe_libinstall -so libipc ${STAGING_LIBDIR}
	: install -m 0644 ${S}/libipc.h ${STAGING_INCDIR}/
    : install -m 0644 ${S}/ipc_common.h ${STAGING_INCDIR}/
}
