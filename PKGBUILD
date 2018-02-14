# Maintainer: Tomás Pinho <me(at)tomaspinho(dot)com>

pkgname=rtl8821ce-dkms-git
_pkgbase=rtl8821ce
pkgver=1.0.0.r1.da7fa93c9
pkgrel=1
pkgdesc="rtl8821CE driver with firmware"
arch=('i686' 'x86_64')
url="https://github.com/tomaspinho/rtl8821ce"
license=('GPL2')
depends=('dkms')
makedepends=('git' 'bc')
conflicts=("${_pkgbase}")
source=("git+https://github.com/tomaspinho/rtl8821ce.git"
        'dkms.conf')
sha256sums=('SKIP'
            '3495f71d4ab451e8c10ebf6e3f5f40c1e15447189cb54d859b4c9991e0c611e0')


pkgver() {
    cd ${srcdir}/rtl8821ce
    printf '%s.r%s.g%s' '1.0.0' "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

package() {
        cd ${srcdir}/rtl8821ce
        mkdir -p ${pkgdir}/usr/src/${_pkgbase}-${pkgver}
        cp -pr * ${pkgdir}/usr/src/${_pkgbase}-${pkgver}
        cp ${srcdir}/dkms.conf ${pkgdir}/usr/src/${_pkgbase}-${pkgver}
        # Set name and version
        sed -e "s/@_PKGBASE@/${_pkgbase}-dkms/" \
                        -e "s/@PKGVER@/${pkgver}/" \
                        -i "${pkgdir}"/usr/src/${_pkgbase}-${pkgver}/dkms.conf
}
