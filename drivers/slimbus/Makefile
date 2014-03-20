ifdef CONFIG_CC_OPTIMIZE_FOR_PERF
CFLAGS_slimbus.o += -Og
endif
#
# Makefile for kernel slimbus framework.
#
obj-$(CONFIG_SLIMBUS)			+= slimbus.o
obj-$(CONFIG_SLIMBUS_MSM_CTRL)		+= slim-msm.o slim-msm-ctrl.o
obj-$(CONFIG_SLIMBUS_MSM_NGD)		+= slim-msm.o slim-msm-ngd.o
