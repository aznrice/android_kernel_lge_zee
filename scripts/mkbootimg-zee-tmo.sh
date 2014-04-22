#!/bin/sh
#
# Make boot image including ramdisk image and dt image.
function gettop
{
    local TOPFILE=scripts/mkbootimg.sh
    if [ -n "$TOP" -a -f "$TOP/$TOPFILE" ] ; then
        echo $TOP
    else
        if [ -f $TOPFILE ] ; then
            # The following circumlocution (repeated below as well) ensures
            # that we record the true directory name and not one that is
            # faked up with symlink names.
            PWD= /bin/pwd
        else
            # We redirect cd to /dev/null in case it's aliased to
            # a command that prints something as a side-effect
            # (like pushd)
            local HERE=$PWD
            T=
            while [ \( ! \( -f $TOPFILE \) \) -a \( $PWD != "/" \) ]; do
                cd .. > /dev/null
                T=`PWD= /bin/pwd`
            done
            cd $HERE > /dev/null
            if [ -f "$T/$TOPFILE" ]; then
                echo $T
            fi
        fi
    fi
}

KERNEL_PATH=$(gettop)
eval DTS_NAME=msm8974-z-tmo

echo $KERNEL_PATH

$KERNEL_PATH/scripts/dtc/dtc -p 1024 -O dtb -o $KERNEL_PATH/arch/arm/boot/$DTS_NAME.dtb $KERNEL_PATH/arch/arm/boot/dts/$DTS_NAME/$DTS_NAME.dts
$KERNEL_PATH/scripts/dtbTool -s 2048 -o $KERNEL_PATH/arch/arm/boot/dt.img -p $KERNEL_PATH/scripts/dtc/ $KERNEL_PATH/arch/arm/boot/
chmod a+r $KERNEL_PATH/arch/arm/boot/dt.img
$KERNEL_PATH/scripts/mkbootimg  --kernel $KERNEL_PATH/arch/arm/boot/zImage --ramdisk $KERNEL_PATH/arch/arm/boot/ramdisktmo.img \
	--cmdline "console=ttyHSL0,115200,n8 androidboot.hardware=z user_debug=31 msm_rtb.filter=0x0 cont_splash_enabled=true" \
	--base 0x00000000 --pagesize 2048 --tags-addr 0x04800000 --offset 0x05000000 \
	--dt $KERNEL_PATH/arch/arm/boot/dt.img --output $KERNEL_PATH/arch/arm/boot/boot.img
