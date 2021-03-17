# AstroCam

AstroCam is a C based stepper motor control system including both, a
Perl and a PHP based webinterface (you can choose one of them). Astrocam
can be used for controlling webcams over the Internet/network and for
everything else you want to do with a stepper motor.

#### [Download the latest stable release from Sourceforge.net](https://sourceforge.net/projects/astrocam/files/latest/download)

<img src="https://www.wendzel.de/images/astrocam.jpg" width="400" />

## Short Project History

The first AstroCam version was released in Dec 2001 and was written in C
and libTk. After I noticed, that the combination of Tcl/Tk and C code
was not a good approach, I wrote a CGI-based webinterface and a
Gtk-based GUI. A few years later, I removed both parts, rewrote AstroCam
entirely, built a PHP webinterface and changed the communication system
between its components to message queues. However, the daemon of the
software is still written in C. Again a few years later (2007), I wrote
an additional Perl-based webinterface to give the user the choice
between both webinterfaces what lead to AstroCam 2.7.0. I applied only
small patches since 2007 to AstroCam, i.e. 2.7.x is still the stable
version.


## Supported Operating Systems

- OpenBSD i386 (tested version 3.x)
- NetBSD i386 (tested version 1.5.x)
- FreeBSD i386 (tested version 4.x or so)
- Linux (tested kernel versions 2.2/2.4/2.6 x86)

## Supported Browsers

All browsers with IFrame-support (e.g. Konqueror, Internet Explorer, Mozilla/Firefox) should work. Netscape 4.x doesn't work.

## Download

You can download the current AstroCam version from [SourceForge](http://sourceforge.net/projects/astrocam).

## Subversion / Direct Source Code Access

The SVN repository is available here: <https://sourceforge.net/p/astrocam/code/HEAD/tree/>.

## Some Impressions

This is my own AstroCam, installed in _2002_. The computer is a 333 MHz AMD K6-2 with 64 MB RAM running Slackware Linux 8.0, and a webcam. The webserver is an Apache-1.x running the old astrocam 0.9-1-1 beta(?) CGI Webinterface and the old (doesn't exist anymore) gameport joystick control of the stepengine.

<a href="http://www.wendzel.de/images/cdps_cam.jpg"><img src="https://www.wendzel.de/images/cdps_cam_s.jpg" width="300" /></a>


This picture is from _2007_:

<a href="https://www.wendzel.de/images/astrocam_2007.jpg"><img src="https://www.wendzel.de/images/astrocam_2007.jpg" width="300" /></a>


## Frequently Asked Questions (FAQ)

**Q: How does it work and how to install AstroCam?**

Take a look at the [INSTALL](https://sourceforge.net/p/astrocam/code/HEAD/tree/v2.x/INSTALL) file for installation introductions.

**Q: Please tell me more about the requirements of astrocam.**

- You need a parallel port on a x86-PC (OS must be Linux, OpenBSD, NetBSD or FreeBSD).
- You need a control-card (connect the card via the parallel port). There are only two controller components supported (and only one of them is tested): the SMC-800 card (tested!) and the SMC-1500 card (I assume it will work, but due to a lack of hardware was never able to test it). I bought my SMC-800 at http://www.conrad.com/.
- You need a stepengine (use any engine you would like to use but take a look at the card requirements from the card documentation before).
- You need a power supply for the SMC card.
- You need to install a webserver (+ PHP), a webcam and a webcam software.

Put the motor on the card (or two motors for up+down and left+right control); turn on the power supply and start the computer. Now start
astrocam, install an Apache webserver with php 4.3 or newer (or an other webserver that supports PHP) and pray that it will work. Please also take a look at astrocam(7) and the 'INSTALL'-file in tgz-file.

**Q: How to change the design of the webinterface?**
Simply change the 'design' value in your /etc/astrocam.conf. The value has to be one of the .css-files in the astrocam directory on your webserver. You also can create your own design files.

**Q: astrocam.php says 'Call to undefined function: msg_get_queue()'!**

It seems that your PHP extension module is not correctly installed in your php.ini file. Please read the 'INSTALL' file of the PHP-package and the configuration and installation chapter of the php book and make sure that you run PHP Version 4.3.0 or newer.

Don't forget to run the ./configure script with enable-sysvsem enable-sysvshm --enable-sysvmsg'. You should add the following lines in your php.ini:

```
sysvmsg.value=PHP_INI_ALL
sysvmsg.string=PHP_INI_ALL
```

**Q: What is the address of my (second) parallel port?**

The primary parallel port usually has the address 0x378 on x86 PC systems. The second parallel port usually has the address 0x278. You can take a look into */proc/ioports* under Linux to get the correct addresses of your computer. Here is an example. This hosts has one parallel port at address 0x378:

```
$ cat /proc/ioports
...
0378-037a : parport0
...
```

**Q: Were does the installer place the docs?**

In version 2.6.6 (and before) the installer does not place the docs somewere but they are included in the .tgz file. Since version 2.7.0 the documentation is placed in */usr/share/doc/astrocam*.

**Q: Is there a way to test astrocam without the webinterface?**

Yes since version 2.7.5 (or newer), you can run the included script *acam_console.pl*. This script will move your step engine to the left/right direction and then exits.

**Q: acam_console.pl prints this error: Use of uninitialized value in msgsnd at ./acam_console.pl ... -- what went wrong? (v. 2.7.5 or newer)**

Start the astrocam service first by running `astrocam` as root user.
