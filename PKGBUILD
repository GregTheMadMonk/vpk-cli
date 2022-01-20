pkgname=vpk-cli-git
pkgver=1.0.0
pkgrel=1
pkgdesc="CLI VPK reader and extractor written in C++"
arch=(any)
url="https://github.com/gregthemadmonk/vpk-cli.git"
source=("git+$url")
md5sums=('SKIP')

pkgver() {
	cd vpk-cli
	printf "1.0.0r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
	cd vpk-cli
	make
}

package() {
	cd vpk-cli
	mkdir -p $pkgdir/usr/bin
	cp vpk-cli $pkgdir/usr/bin
	cp vpk-xplore $pkgdir/usr/bin
}
