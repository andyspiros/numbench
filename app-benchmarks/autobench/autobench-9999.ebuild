# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

inherit multilib

EAPI=0

DESCRIPTION="Automated benchmarks suite"
HOMEPAGE="http://git.overlays.gentoo.org/gitweb/?p=proj/auto-numerical-bench.git"
SRC_URI=""

LICENSE="GPL-2"
KEYWORDS="~amd64 ~x86"
SLOT="0"

src_install() {
    local libdir=/usr/$(get_libdir)
    dodir $libdir
    dodir /usr/bin
    cp -r "${FILESDIR}"/python "${D}"/$libdir/autobench
    chmod +x "${D}"/$libdir/autobench/main.py
    dosym $libdir/autobench/main.py /usr/bin/autobench
#    ln -s $libdir/autobench/main.py "${D}"/usr/bin/autobench 
}
