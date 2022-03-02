LICENSE = "CLOSED"
LICENSE_FLAGS = "commercial"

inherit systemd

SUB_FOLDER_arm = "arm"
SUB_FOLDER_aarch64 = "aarch64"
SUB_FOLDER_x86-64 = "x86_64"

COMPATIBLE_HOSTS = "arm|aarch64|x86_64"

FILES_${PN} = " \
    ${bindir}/mender-gateway \
"

FILES_${PN}_append_mender-systemd = " \
    ${systemd_system_unitdir}/mender-gateway.service \
"

S = "${WORKDIR}/mender-gateway-${PV}"

do_version_check() {
    if ! ${@'true' if d.getVar('MENDER_DEVMODE') else 'false'}; then
        if ! strings ${S}/${SUB_FOLDER}/mender-gateway | grep -q "^${PV}$"; then
            bbfatal "String '${PV}' not found in binary. Is it the correct version? Check with --version."
        fi
    fi
}
addtask do_version_check after do_unpack before do_install

SYSTEMD_SERVICE_${PN}_mender-systemd = "mender-gateway.service"

do_install() {
    install -d -m 755 ${D}${bindir}
    install -m 755 ${S}/${SUB_FOLDER}/mender-gateway ${D}${bindir}/mender-gateway

    # install config file if provided
    if [ -f ${WORKDIR}/mender-gateway.conf ]; then
        install -d -m 755 ${D}/${sysconfdir}/mender
        install -m 0600 ${WORKDIR}/mender-gateway.conf ${D}/${sysconfdir}/mender/mender-gateway.conf
    else
        bbwarn "No mender-gateway.conf found in SRC_URI, no mender-gateway.conf installed. The gateway will run with the default parameters"
    fi
}

do_install_append_mender-systemd() {
    install -d -m 755 ${D}${systemd_unitdir}/system/
    install -m 644 ${S}/support/mender-gateway.service ${D}${systemd_unitdir}/system/mender-gateway.service
}