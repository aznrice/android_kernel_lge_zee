#!/bin/bash
#sourcedir
SOURCE_DIR="/home/hieu/kernel/zee"
#crosscompile stuff
CROSSARCH="arm"
CROSSCC="$CROSSARCH-eabi-"
TOOLCHAIN="/home/hieu/toolchains/prebuilts/boosted-4.9/bin"
#our used directories
#PREBUILT="/PATH/TO/PREBUILTS/MEANING/ZIPCONTENTS/prebuilt"
OUT_DIR="/home/hieu/kernel/zee/out"
#compile neccesities
USERCCDIR="/home/hieu/.ccache"
CODENAME="zee"
DEFCONFIG="boosted_zee_defconfig"
NRJOBS=$(( $(nproc) * 2 ))
#ftpstuff
#RETRY="60s";
#MAXCOUNT=30;
#HOST="host.com"
#USER="user"
#PASS='pass'

#if we are not called with an argument, default to branch master
if [ -z "$1" ]; then
  BRANCH="caf-3.10-staging"
  echo "[BUILD]: WARNING: Not called with branchname, defaulting to $BRANCH!";
  echo "[BUILD]: If this is not what you want, call this script with the 
branchname.";
else
  BRANCH=$1;
fi

echo "[BUILD]: ####################################";
echo "[BUILD]: ####################################";
echo "[BUILD]: Building branch: $BRANCH";
echo "[BUILD]: ####################################";
echo "[BUILD]: ####################################";

###CCACHE CONFIGURATION STARTS HERE, DO NOT MESS WITH IT!!!
TOOLCHAIN_CCACHE="$TOOLCHAIN/../bin-ccache"
gototoolchain() {
  echo "[BUILD]: Changing directory to $TOOLCHAIN/../ ...";
  cd $TOOLCHAIN/../
}

gotocctoolchain() {
  echo "[BUILD]: Changing directory to $TOOLCHAIN_CCACHE...";
  cd $TOOLCHAIN_CCACHE
}

#check ccache configuration
#if not configured, do that now.
if [ ! -d "$TOOLCHAIN_CCACHE" ]; then
    echo "[BUILD]: CCACHE: not configured! Doing it now...";
    gototoolchain
    mkdir bin-ccache
    gotocctoolchain
    ln -s $(which ccache) "$CROSSCC""gcc"
    ln -s $(which ccache) "$CROSSCC""g++"
    ln -s $(which ccache) "$CROSSCC""cpp"
    ln -s $(which ccache) "$CROSSCC""c++"
    gototoolchain
    chmod -R 777 bin-ccache
    echo "[BUILD]: CCACHE: Done...";
fi
export CCACHE_DIR=$USERCCDIR
###CCACHE CONFIGURATION ENDS HERE, DO NOT MESS WITH IT!!!

echo "[BUILD]: Setting cross compile env vars...";
export ARCH=$CROSSARCH
export CROSS_COMPILE=$CROSSCC
export PATH=$TOOLCHAIN_CCACHE:${PATH}:$TOOLCHAIN

gotosource() {
  echo "[BUILD]: Changing directory to $SOURCE_DIR...";
  cd $SOURCE_DIR
}

gotokcontrolgpu() {
  echo "[BUILD]: Changing directory to $SOURCE_DIR...";
  cd $SOURCE_DIR/kcontrol
}

gotoout() {
  echo "[BUILD]: Changing directory to $OUT_DIR...";
  cd $OUT_DIR
}

gotosource

#Checking out latest upstream changes
#echo "[BUILD]: Checking out latest changes on $BRANCH from origin...";
#git clean -f -d
#git fetch --all
#git reset --hard origin/$BRANCH

#saving new rev
REV=$(git log --pretty=format:'%h' -n 1)
echo "[BUILD]: Saved current hash as revision: $REV...";
#date of build
DATE=$(date +%Y%m%d_%H%M%S)
echo "[BUILD]: Start of build: $DATE...";

#build the kernel
echo "[BUILD]: Cleaning kernel (make mrproper)...";
make mrproper
echo "[BUILD]: Using defconfig: $DEFCONFIG...";
make $DEFCONFIG
echo "[BUILD]: Changing CONFIG_LOCALVERSION to: -kernel-"$CODENAME"-"$BRANCH" ...";
echo "[BUILD]: Bulding the kernel...";
time make -j$NRJOBS || { exit 1; }
echo "[BUILD]: Done!...";

#make bootimg
echo "[BUILD]: Bootimg (Bootimg) to $OUT_DIR/...";
bash scripts/mkbootimg-zee.sh

gotokcontrolgpu

echo "[BUILD]: Updating KERNEL_BUILD inside the Makefile...";
sed -i '/KERNEL_BUILD := /c\KERNEL_BUILD := ../' Makefile
echo "[BUILD]: Building KControl MSM gpu module...";
make || { return 1; }
echo "[BUILD]: Done with kcontrol's MSM gpu module!...";

gotosource

#copy stuff for our zip
echo "[BUILD]: Copying kernel (Bootimg) to $OUT_DIR/...";
cp arch/arm/boot/boot.img $OUT_DIR/
echo "[BUILD]: Copying modules (*.ko) to $OUT_DIR/modules/...";
find $SOURCE_DIR/ -name \*.ko -exec cp '{}' $OUT_DIR/modules/ ';'
echo "[BUILD]: Stripping modules";
/home/hieu/toolchains/prebuilts/boosted-4.9/bin/arm-eabi-strip --strip-unneeded $OUT_DIR/modules/*.ko;
echo "[BUILD]: Done!...";

gotoout

#create zip and clean folder
echo "[BUILD]: Creating zip: att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip ...";
zip -r att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip . -x "*.zip" "*.sha1" "*.md5"


echo "[BUILD]: Creating sha1 & md5 sums...";
md5sum att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip > att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip.md5
sha1sum att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip > att_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip.sha1

echo "[BUILD]: All done!...";
