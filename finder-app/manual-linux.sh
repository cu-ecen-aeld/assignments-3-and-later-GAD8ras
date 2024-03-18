#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

#my code
dirv=$1
default_OUTDIR=/tmp/aeld
OUTDIR=${dirv:=$default_OUTDIR}
echo $OUTDIR
if [ ! -d "$OUTDIR" ]; then 
    if [ `mkdir -p "$OUTDIR"` ]; then
        echo "failed to make a dir" 
        exit 1
    fi
fi

KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.1.10
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-
CC_SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)


if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    #my code
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE mrproper
    make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE defconfig
    make -j2 ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE all

fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
if [ -d "rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    rm -rf ${OUTDIR}/rootfs
    cd ${OUTDIR}
    rm initramfs.cpio.gz
fi
if [ `mkdir -p "rootfs"` ]; then
    echo "failed to make a rootfs dir" 
    exit 1
fi
cd rootfs
if [ `mkdir -p bin dev etc proc sbin home lib64 sys lib var tmp usr | mkdir -p usr/bin usr/lib usr/sbin | mkdir -p var/log | mkdir -p sbin/init`]; then
    echo "failed to make a rootfs/sub dir" 
    exit 1
fi

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE
make CONFIG_PREFIX=../rootfs ARCH=$ARCH CROSS_COMPILE=$CROSS_COMPILE install

echo "Library dependencies"
# ${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
# ${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
#sudo find / -name "ld-linux-aarch64.so.1"
#[sudo] password for gone: 
#/home/gone/me/study/embedded-linux/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/aarch64-none-linux-gnu/libc/lib/ld-linux-aarch64.so.1
cp ${CC_SYSROOT}/lib/ld-linux-aarch64.so.1 ${OUTDIR}/rootfs/lib

cp ${CC_SYSROOT}/lib64/libm.so.6 ${OUTDIR}/rootfs/lib64
cp ${CC_SYSROOT}/lib64/libresolv.so.2 ${OUTDIR}/rootfs/lib64
cp ${CC_SYSROOT}/lib64/libc.so.6 ${OUTDIR}/rootfs/lib64


# TODO: Make device nodes
cd ${OUTDIR}/rootfs/
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
cd ${FINDER_APP_DIR}
if [ -f "writer" ]; then
	echo "file  exist."
	make clean
 	make CROSS_COMPILE=${CROSS_COMPILE}
else
	make CROSS_COMPILE=${CROSS_COMPILE}
fi
# TODO: Copy the finder related scripts and executables to the /home directory
cp finder.sh writer ${OUTDIR}/rootfs/home
cp finder-test.sh ${OUTDIR}/rootfs/home
mkdir ${OUTDIR}/rootfs/home/conf
cp conf/username.txt ${OUTDIR}/rootfs/home/conf
cp conf/assignment.txt ${OUTDIR}/rootfs/home/conf
cp autorun-qemu.sh ${OUTDIR}/rootfs/home
# on the target rootfs

# TODO: Chown the root directory
cd ${OUTDIR}/rootfs/
sudo chown -R root:root *


echo "TODO: Create initramfs.cpio.gz"
find . | cpio -H newc -ov --owner root:root >  ${OUTDIR}/initramfs.cpio
cd ${OUTDIR}
gzip -f initramfs.cpio
