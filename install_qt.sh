# !/bin/bash

# If not root
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root (in order to fetch packages)" 
   exit 1
fi

# If not one argument given
if [ "$#" -ne 1 ]; then
    echo "Run as ./install_qt.sh /path/to/install/qt/to"
fi

$installPath = $1

# Update and install necessary packages
apt update
apt -y install libfontconfig1-dev libfreetype6-dev libx11-dev libxext-dev libxfixes-dev libxi-dev libxrender-dev libxcb1-dev libx11-xcb-dev libxcb-glx0-dev

cd /tmp

echo "Fetching Qt Source"

# Download Qt, extract
wget https://download.qt.io/archive/qt/5.9/5.9.2/single/qt-everywhere-opensource-src-5.9.2.tar.xz
tar xf qt-everywhere-opensource-src-5.9.2.tar.xz
cd qt-everywhere-opensource-src-5.9.2

echo "Configuring to install in " + $installPath

# Configure for install, skip as much as possible in order to reduce compilation time
# This will accept the open source agreement
./configure -prefix $installPath \
			-release \
			-opensource \
			-confirm-license \
			-accessibility \
			-fontconfig \
			-qt-xcb \
			-qt-zlib \
			-qt-libpng \
			-qt-libjpeg \
			-qt-pcre \
			-no-glib \
			-no-cups \
			-no-ssl \
			-no-sql-sqlite \
			-no-qml-debug \
			-no-opengl \
			-no-egl \
			-no-xinput2 \
			-no-sm \
			-no-icu \
			-nomake examples \
			-nomake tests \
			-skip qtactiveqt \
			-skip qtenginio \
			-skip qtlocation \
			-skip qtmultimedia \
			-skip qtserialport \
			-skip qtquick1 \
			-skip qtquickcontrols \
			-skip qtscript \
			-skip qtsensors \
			-skip qtwebsockets \
			-skip qtxmlpatterns \
			-skip qt3d \
			-skip qtdeclarative \
			-skip qtwebengine \
			-skip qtcharts \
			-skip qtwebchannel \
			-skip qtpurchasing \
			-skip qtwayland \
			-skip qtwebview \
			-skip qtvirtualkeyboard

echo "Compiling & Installing Qt"

# Make and install to given install path
make -j4
make install

echo "Done"