#!/bin/bash
#sourcedir
USECHKS=1;
SOURCE_DIR="$(pwd)"
#crosscompile stuff
CROSSARCH="arm"
CROSSCC="$CROSSARCH-eabi-"
TOOLCHAIN="/home/hieu/toolchains/prebuilts/boosted-4.9/bin"
#our used directories
#PREBUILT="/PATH/TO/PREBUILTS/MEANING/ZIPCONTENTS/prebuilt"
OUT_DIR="$(pwd)/out"
#compile neccesities
USERCCDIR="$HOME/.ccache"
CODENAME="zee"
DEFCONFIG="boosted_zee-tmo_defconfig"
NRJOBS=$(( $(nproc) * 2 ))
#ftpstuff
#RETRY="60s";
#MAXCOUNT=30;
#HOST="host.com"
#USER="user"
#PASS='pass'

#if we are not called with an argument, default to branch master
if [ -z "$1" ]; then
  BRANCH="exp"
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

OUT_ENABLED=1;
if [ ! -d "$OUT_DIR" ]; then
    echo "[BUILD]: Directory '$OUT_DIR' which is configure as output directory does not exist!";
    VALID=0;
    while [[ $VALID -eq 0 ]]
    do
        echo "[Y|y] Create it.";
        echo "[N|n] Don't create it, this will disable the output directory.";
        echo "Choose an option:";
        read DECISION;
        case "$DECISION" in
            y|Y)
            VALID=1;
            echo "Creating directory $OUT_DIR...";
            mkdir $OUT_DIR
            mkdir $OUT_DIR/modules
            ;;
            n|N)
            VALID=1;
            OUT_ENABLED=0;
            echo "Disabling output directory...";
            ;;
            *)
            echo "Error: Unknown input ($DECISION), try again.";
        esac
    done
else
    if [ ! -d "$OUT_DIR/modules" ]; then
        echo "Creating directory $OUT_DIR/modules...";
        mkdir $OUT_DIR/modules
    fi
fi

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
SAVEDPATH=$PATH;
SAVEDCROSS_COMPILE=$CROSS_COMPILE;
SAVEDARCH=$ARCH;
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

gotoout

echo "[BUILD]: Cleaning $OUT_DIR...";
rm *.zip *.md5 *.sha1

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
echo "[BUILD]: Changing CONFIG_LOCALVERSION to: -boosted-"$CODENAME"-"$BRANCH" ...";
sed -i "/CONFIG_LOCALVERSION=\"/c\CONFIG_LOCALVERSION=\"-boosted-"$CODENAME"-"$BRANCH"\"" .config

#kcontrol necessities
if [ $(cat .config | grep 'CONFIG_ARCH_MSM=y' | tail -n1) == "CONFIG_ARCH_MSM=y" ]; then
    DEVARCH="msm";
elif [ $(cat .config | grep 'CONFIG_ARCH_TEGRA=y' | tail -n1) == "CONFIG_ARCH_TEGRA=y" ]; then
    DEVARCH="tegra";
fi
gotokcontrol() {
  echo "[BUILD]: Changing directory to $SOURCE_DIR/kcontrol...";
  cd $SOURCE_DIR/kcontrol
}

gotokcontrolgpu() {
  echo "[BUILD]: Changing directory to $SOURCE_DIR/kcontrol/kcontrol_gpu_$DEVARCH...";
  cd $SOURCE_DIR/kcontrol/kcontrol_gpu_$DEVARCH
}
#end kcontrol necessities

echo "[BUILD]: Bulding the kernel...";
time make -j$NRJOBS || { return 1; }
echo "[BUILD]: Done with kernel!...";

# BUILD KCONTROL
#done building, lets build kcontrol modulesa
echo "[BUILD]: Initializing directories for KControl modules...";
rm -rf kcontrol
mkdir kcontrol
gotokcontrol
#gpu
echo "[BUILD]: Cloning KControl msm gpu module source...";
if [ $DEVARCH == "msm" ]; then
    git clone https://git.bricked.de/kcontrol/kcontrol_gpu_msm.git
elif  [ $DEVARCH == "tegra" ]; then
    git clone https://git.bricked.de/kcontrol/kcontrol_gpu_tegra.git
fi
gotokcontrolgpu
echo "[BUILD]: Updating KERNEL_BUILD inside the Makefile...";
sed -i '/KERNEL_BUILD := /c\KERNEL_BUILD := ../../' Makefile
echo "[BUILD]: Building KControl $DEVARCH gpu module...";
make || { return 1; }
echo "[BUILD]: Done with kcontrol's $DEVARCH gpu module!...";
# END BUILD KCONTROL

if [[ ! $OUT_ENABLED -eq 0 ]]; then
    gotoout
    #prepare our zip structure
    echo "[BUILD]: Cleaning out directory...";
    find $OUT_DIR/* -maxdepth 0 ! -name '*.zip' ! -name '*.md5' ! -name '*.sha1' ! -name kernel ! -name modules ! -name out -exec rm -rf '{}' ';'
    if [ ! $USEPREBUILT -eq 0 ]; then
        if [ -d "$PREBUILT" ]; then
            echo "[BUILD]: Copying prebuilts to out directory...";
            cp -R $PREBUILT/* $OUT_DIR/
        fi
    fi
    gotosource

#make bootimg
echo "[BUILD]: Bootimg (Bootimg) to $OUT_DIR/...";
bash scripts/mkbootimg-tmo.sh

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
    echo "[BUILD]: Creating zip: boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip ...";
    zip -r boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip . -x "*.zip" "*.sha1" "*.md5"
    echo "[BUILD]: Cleaning out directory...";
    find $OUT_DIR/* -maxdepth 0 ! -name '*.zip' ! -name '*.md5' ! -name '*.sha1' ! -name out -exec rm -rf '{}' ';'
    echo "[BUILD]: Done!...";

    if [ ! $USECHKS -eq 0 ]; then
        echo "[BUILD]: Creating sha1 & md5 sums...";
        md5sum boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip > boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip.md5
        sha1sum boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip > boosted_"$CODENAME"_"$DATE"_"$BRANCH"-"$REV".zip.sha1
    fi
fi

echo "[BUILD]: All done!...";
gotosource
export PATH=$SAVEDPATH
export CROSS_COMPILE=$SAVEDCROSS_COMPILE;
export ARCH=$SAVEDARCH;
