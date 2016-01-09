/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */
/*      $Id: acconfig.h,v 5.22 2002/07/27 09:17:08 lirc Exp $      */

/*
 *  are you editing the correct file?
 *
 *  acconfig.h  - changes for distribution
 *                you must run autoheader / configure
 *  config.h.in - changes specific to your installation
 *                these will be lost if you run autoheader
 *  config.h    - changes to this configuration
 *                these will be lost if you run configure
 */

/* note.
 * if you want to change silly things like the device file names or the
 * configuration file names then remember you may also need to change
 * the Makefile.am files.
 */

/* device file names - beneath DEVDIR (default /dev) */
#define DEV_LIRC	"lirc"
#define DEV_LIRCD	"lircd"
#define DEV_LIRCM	"lircm"

/* config file names - beneath SYSCONFDIR (default /usr/local/etc) */
#define CFG_LIRCD	"lircd.conf"
#define CFG_LIRCM	"lircmd.conf"

/* config file names - beneath $HOME */
#define CFG_USER	".lircrc"

/* log files */
#define LOG_LIRCD	"lircd"
#define LOG_LIRMAND	"lirmand"

/* pid file */
#define PID_LIRCD       "lircd.pid"

/* default port number */
#define	LIRC_INET_PORT	8765

/*
 * below here are defines managed by autoheader / autoconf
 */



/* define in maintainer mode */
/* #undef MAINTAINER_MODE */

/* Define to use long long IR codes */
#define LONG_IR_CODE 1

/* Define to enable debugging output */
/* #undef DEBUG */

/* Define to run daemons as daemons */
#define DAEMONIZE 1

/* Define if forkpty is available */
#define HAVE_FORKPTY 1

/* Define if the caraca library is installed */
/* #undef HAVE_LIBCARACA */

/* Define if the libirman library is installed */
/* #undef HAVE_LIBIRMAN */

/* Define if the software only test version of libirman is installed */
/* #undef HAVE_LIBIRMAN_SW */

/* Define if the complete vga libraries (vga, vgagl) are installed */
#define HAVE_LIBVGA 1

/* define if you have vsyslog( prio, fmt, va_arg ) */
//#define HAVE_VSYSLOG 1

/* define if you want to log to syslog instead of logfile */
/* #undef USE_SYSLOG */

/* Text string signifying which driver is configured */
#define LIRC_DRIVER "edb9315"

/* Set the device major for the lirc driver */
#define LIRC_MAJOR 61

/* Set the IRQ for the lirc driver */
#define LIRC_IRQ 

/* Set the port address for the lirc driver */
#define LIRC_PORT 

/* Set the timer for the parallel port driver */
#define LIRC_TIMER 65536

/* Define if you have an animax serial port receiver */
/* #undef LIRC_SERIAL_ANIMAX */

/* Define if you have a IR diode connected to the serial port */
/* #undef LIRC_SERIAL_TRANSMITTER */

/* Define if the software needs to generate the carrier frequency */
#define LIRC_SERIAL_SOFTCARRIER 1

/* Define if you have an IRdeo serial port receiver */
/* #undef LIRC_SERIAL_IRDEO */

/* Define if you have an IRdeo remote transmitter */
/* #undef LIRC_SERIAL_IRDEO_REMOTE */

/* Define if you want to cross-compile for the SA1100 */
/* #undef LIRC_ON_SA1100 */

/* Define if you want to use a Tekram Irmate 210 */
/* #undef LIRC_SIR_TEKRAM */

/* Define if you want to use a Actisys Act200L */
/* #undef LIRC_SIR_ACTISYS_ACT200L */

/* Define if devfs support is present in current kernel */
/* #undef LIRC_HAVE_DEVFS */

/* syslog facility to use */
//#define LIRC_SYSLOG 

/* modifiable single-machine data */
#define LOCALSTATEDIR "/var"

/* Define to include most drivers */
/* #undef LIRC_DRIVER_ANY */

/* The name of the hw_* structure to use by default */
#define HW_DEFAULT hw_default

/* system configuration directory */
#define SYSCONFDIR "/etc"

/* device files directory */
#define DEVDIR "/dev"

/* This should only be set by configure */
#define PACKAGE "lirc"

/* This should only be set by configure */
#define VERSION "0.6.6"


/* Define if you have the `daemon' function. */
#define HAVE_DAEMON 1

/* Define if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define if you have the `forkpty' function. */
#define HAVE_FORKPTY 1

/* Define if you have the `gethostname' function. */
#define HAVE_GETHOSTNAME 1

/* Define if you have the `gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have the `mkfifo' function. */
#define HAVE_MKFIFO 1

/* Define if you have the `select' function. */
#define HAVE_SELECT 1

/* Define if you have the `snprintf' function. */
#define HAVE_SNPRINTF 1

/* Define if you have the `socket' function. */
#define HAVE_SOCKET 1

/* Define if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define if you have the `strdup' function. */
#define HAVE_STRDUP 1

/* Define if you have the `strerror' function. */
#define HAVE_STRERROR 1

/* Define if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define if you have the `strsep' function. */
#define HAVE_STRSEP 1

/* Define if you have the `strtoul' function. */
#define HAVE_STRTOUL 1

/* Define if you have the <syslog.h> header file. */
//#define HAVE_SYSLOG_H 1

/* Define if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define if you have the `vsyslog' function. */
//#define HAVE_VSYSLOG 1

/* Name of package */
#define PACKAGE "lirc"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>. */
#define TIME_WITH_SYS_TIME 1

/* Define if your <sys/time.h> declares `struct tm'. */
/* #undef TM_IN_SYS_TIME */

/* Version number of package */
#define VERSION "0.6.6"

/* Define if the X Window System is missing or not being used. */
#define X_DISPLAY_MISSING 1

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define as `__inline' if that's what the C compiler calls it, or to nothing
   if it is not supported. */
/* #undef inline */

/* Define to `long' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef size_t */

/*
 * compatibility and useability defines
 */

/* FIXME */
#ifdef LIRC_HAVE_DEVFS
#define LIRC_DRIVER_DEVICE	DEVDIR "/" DEV_LIRC "/0"
#else
#define LIRC_DRIVER_DEVICE      DEVDIR "/" DEV_LIRC
#endif /* LIRC_HAVE_DEVFS */

/* Set the default tty used by the irman/remotemaster driver */
#define LIRC_IRTTY ""

#define LIRCD			"/tmp/" DEV_LIRCD
#define LIRCM			DEVDIR "/" DEV_LIRCM

#define LIRCDCFGFILE		SYSCONFDIR "/" CFG_LIRCD
#define LIRCMDCFGFILE		SYSCONFDIR "/" CFG_LIRCM

#define LIRCCFGFILE		CFG_USER

#define LOGFILE			LOCALSTATEDIR "/log/" LOG_LIRCD
#define LIRMAND_LOGFILE		LOCALSTATEDIR "/log/" LOG_LIRMAND

#define PIDFILE                 LOCALSTATEDIR "/run/" PID_LIRCD

/* end of acconfig.h */
