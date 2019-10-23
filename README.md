# Realtek RTL8821CE Driver

## Intent
This repository hosts the code for the [ArchLinux AUR Package](https://aur.archlinux.org/packages/rtl8821ce-dkms-git/). It's targetting Linux > 4.14 and is being developed for ArchLinux and Ubuntu 18.10. No support will be provided for other Linux distributions or Linux Kernel versions outside of that range.

## Disclaimer
The maintainers of this repository are not Realtek employees and are maintaining this repository for their own usage. Further feature development (such as proper power saving, etc.) will not be pursued here, but will be gladly integrated if newer driver sources are provided by Realtek. Use at your own risk.

## DKMS
This driver can be installed using [DKMS](http://linux.dell.com/dkms/). This is a system which will automatically recompile and install a kernel module when a new kernel gets installed or updated. To make use of DKMS, install the `dkms` package.


## Installation of Driver
Make sure you have a proper build environment and `dkms` installed.

### Ubuntu & Debian
```
sudo apt install bc module-assistant build-essential dkms
sudo m-a prepare
```

### Arch Linux
Generally speaking, Arch Linux already comes with a compiler toolchain installed. If you have a weird installation, just make sure you have the `base-devel` package group installed before you proceed.

#### Installing from AUR
```
yaourt -Sy aur/rtl8821ce-dkms-git
```
And that's it.

#### Depencies for manual installation on Arch Linux
```
sudo pacman -Syu linux-headers dkms
```
If you are running a non-vanilla kernel then install the headers to match the kernel package. Proceed to the section below.

### Manual installation of driver
In order to install the driver open a terminal in the directory with the source code and execute the following command:
```
sudo ./dkms-install.sh
```

## Removal of Driver
Open a terminal window and git clone the repository to your local disk

```
git clone https://github.com/tomaspinho/rtl8821ce.git
cd rtl8821ce
```

Then run the installation script:
```
sudo ./dkms-remove.sh
```

## Reporting issues
When reporting issues, please make sure that debugging is enabled. To enable debugging either set `MAKEFLAGS="CONFIG_RTW_DEBUG = y"` before compilation or edit Makefile:
```
CONFIG_RTW_DEBUG = y
```
This will enable verbose debug logging, helpful to developers.

## Possible issues

### PCIe Activate State Power Management
Your distribution may come with PCIe Active State Power Management enabled by default. That may conflict with this driver. To disable:

```
sudo $EDITOR /etc/default/grub
```
Add pci=noaer at the end of GRUB_CMDLINE_LINUX_DEFAULT. Line should look like this:

```
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash pci=noaer"
```

Then update your GRUB configuration:
```
sudo update-grub
```

Reboot.

### Lenovo Yoga laptops

Some new Yoga laptops (like the Yoga 530) come with `rtl8821ce` as the Wi-Fi/Bluetooth chip. But the `ideapad-laptop` module, which may come included in your distribution, may conflict with this driver. To disable:

```
sudo modprobe -r ideapad_laptop
```

### BlueTooth is not working

This may be due to the Kernel loading up the wrong firmware file for this card. Please take a look at [@wahsot](https://github.com/wahsot)'s tutorial at https://github.com/tomaspinho/rtl8821ce/issues/19#issuecomment-452196840 to see if that helps you out.
