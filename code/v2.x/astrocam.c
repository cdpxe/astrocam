/*
 * This is the AstroCam Sourcecode - https://www.wendzel.de
 *
 * Copyright (C) 2001-2009 Steffen Wendzel - All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/**
 * THIS CODE FILE IS BASED ON:
 *
 * stepper.c
 *
 * (C) 1998 Denis Boehme, Masoud Sotoodeh
 * Licence LGPL
 */
#ifndef VERSION
	#error "No VERSION macro defined."
#endif
#ifndef METAVERSION
	#error "No METAVERSION macro defined."
#endif
#ifndef PATCHLEVEL
	#error "No PATCHLEVEL macro defined."
#endif

float globver =  METAVERSION;

/*#define AC_DEBUG*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>

#ifdef __linux__
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
/* Linux 2.4 or newer */
#include <sys/io.h>
#else
/* Linux 2.2.x (and older? hm.. didn't test older versions...) */
#include <asm/io.h>
#endif
#endif

#if defined(__NetBSD__) || defined(__OpenBSD__)
  #include <sys/types.h>
  #include <sys/syscall.h>
  #include <i386/sysarch.h>
  #include <i386/pio.h>
  #define BSD
#endif

#ifdef __FreeBSD__
  #include <sys/types.h>
  #include <sys/syscall.h>
  #include <machine/sysarch.h>
  #include <machine/cpufunc.h>
#endif

#define CONFIG_FILE	"/etc/astrocam.conf"
#define PIDFILE		"/var/log/acam_pid"
#define PID_CHMOD	(S_IRUSR | S_IWUSR)

#define MAX_MOT    3
#define MAX_PHASEN 4

#define PA_L 0
#define PA_R 4
#define PB_L 0
#define PB_R 32

#define CTRL_MOT_X 0
#define CTRL_MOT_Y 64
#define CTRL_MOT_Z 128

/* some errors */
#define ERROR_INIT  perror("AstroCamInit");
#define ERROR_EXIT  perror("AstroCamShutdown");

#ifdef __FreeBSD__
int   devfd;
#endif

long  pos[MAX_MOT];
int   mot_power[MAX_MOT],
      srvid;

/* my syslog function */
void logit(char *string);

u_char mot_ctrl[MAX_MOT] =
{
  CTRL_MOT_X,
  CTRL_MOT_Y,
  CTRL_MOT_Z
};

u_char phase[MAX_PHASEN] =
{
  PA_L + PB_L,
  PA_R + PB_L,
  PA_R + PB_R,
  PA_L + PB_R
};

typedef struct
{
  char var[100];
  char value[100];
} entry;

typedef struct
{
  float conf_ver;
  int maxdreh,
      mindreh;
  int maxvdreh,
      minvdreh;
  int curpos,
      curvpos;
  int negation;
  unsigned int iobase;
  unsigned int ipckey;
} astrocam_conf;
astrocam_conf globconf;

typedef struct
{
  long mtyp;
  char data[10];
} ipcrecv;

void outportb(unsigned char b)
{
  #ifdef linux
    outb(b, globconf.iobase);
    outb(0, globconf.iobase + 2);
    outb(1, globconf.iobase + 2);
  #endif

  #if defined(BSD) || defined(__FreeBSD__)
    outb(globconf.iobase, b);
    outb(globconf.iobase + 2, 0);
    outb(globconf.iobase + 2, 1);
  #endif
}

void AstroCam_Step(int mot, long n)
{
  int i;
  unsigned char ctrl = mot_ctrl[mot];

  if(n>0){
    for(i=0; i<n; i++){
      pos[mot]++;
      outportb(ctrl + phase[pos[mot] % MAX_PHASEN]);
      usleep(100);
    }
  }else{
    for(i=0; i>n; i--){
      pos[mot]--;
      outportb(ctrl + phase[pos[mot] % MAX_PHASEN]);
      usleep(100);
      }
   }
}

#ifdef BSD
void ioport()
{
  u_long iomap[32];
  struct i386_set_ioperm_args ioperm;

  /* code part found in usenet */
  ioperm.iomap = iomap;
  syscall(SYS_sysarch, I386_GET_IOPERM, (char *) &ioperm);
  iomap[globconf.iobase >> 5] &= ~(1 << (globconf.iobase & 0x1f));
  syscall(SYS_sysarch, I386_SET_IOPERM, (char *) &ioperm);
}
#endif

int AstroCamInit()
{
  int i;

  #ifdef BSD
    if(getuid()==0){
       ioport(globconf.iobase);
       ioport(globconf.iobase + 1);
       ioport(globconf.iobase + 2);
    }else{
       fprintf(stderr, "AstroCamInit(): You need root-access to open an "
			"I/O-device directly.\n");
       fflush(stderr);
	   exit(0);
      return -1;
    }
  #endif

  #ifdef linux
    if(ioperm(globconf.iobase, 3, 1)){
       ERROR_INIT
       fflush(stderr);
      return -1;
    }
  #endif

  #ifdef __FreeBSD__
    if((devfd=open("/dev/io", O_WRONLY))<0){
       ERROR_INIT
       fflush(stderr);
      return -1;
    }
  #endif

  for(i=0;i<MAX_MOT;i++){
     pos[i]=400000;
     mot_power[i]=-1;     
  }
  return 0;
}

int AstroCamShutdown()
{
  #ifdef linux
    if(ioperm(globconf.iobase, 3, 0)){
       ERROR_EXIT
      return -1;
    }
  #endif

  #ifdef __FreeBSD__
    if(close(devfd)!=0){
       ERROR_EXIT
      return -1;
    }
  #endif
  return 0;
}

/* create the daemon */
void dm(pid_t *mypid)
{
  setsid();
  umask(0);
  *mypid=getpid();
}

void welcome()
{
  printf("\n\x1B[32mAstroCam Daemon " VERSION PATCHLEVEL " \x1B[0m [ "
         "Written and Copyright 2001-2009 by Steffen Wendzel, "
         "https://www.wendzel.de ]\x1B[32m running...\x1B[0m\n");
}

void sighndl(int signo)
{
  logit("signal response - shutdown now !");

  unlink(PIDFILE);
  if(AstroCamShutdown()<0)
     perror("AstroCamShutdown(): ");
  if(signo!=1000)
    if(msgctl(srvid, IPC_RMID, NULL)==-1)
      logit("msgctl (rmid) problem!");
  exit(1);
}

void logit(char *string)
{
  openlog("astrocam", LOG_PID, LOG_DAEMON);
  syslog(LOG_DAEMON|LOG_ERR, "%s", string);
  closelog();
}

int chkrunstat()
{
  int file;

  if((file = open(PIDFILE, O_RDONLY))<0)
     return 0;
  else{
     fprintf(stderr, "It seems that the astrocam daemon is already "
                     "running. Please wait while restarting...\n");
     return -1;
  }
}

int restartac(void)
{
  int file;
  char buf[11] = { '\0' };
  struct stat s;
  int sec_sd = 0; /* security shutdown */
  pid_t pid;

  if((file=open(PIDFILE, O_RDONLY))<0)
     return -1;

  /* 2.6.5: needed for the next two steps */
  if(fstat(file, &s)==-1) {
     perror("fstat(" PIDFILE ")");
     exit(1);
  }
  /* 2.6.5: check if this file is writable by group/others and if this
   *        file is a symlink to prevent symlink hacks.
   */
  if(S_ISLNK(s.st_mode) || (S_IWOTH & s.st_mode) || (S_IWGRP &s.st_mode)) {
     fprintf(stderr, "File mode of " PIDFILE
			" has changed or file is a symlink!\n");
     sec_sd=1;
  }
  /* 2.6.5: check if owner isn't root */
  if(s.st_uid!=0) {
     fprintf(stderr, "Owner of " PIDFILE " is NOT uid zero (root)!\n");
     sec_sd=1;
  }
  if(sec_sd) {
     fprintf(stderr, "I will exit now. Please kill the running astrocam"
     		     " process by hand and delete " PIDFILE " for security"
		     " reasons.\n");
     exit(1);
  }

  if(read(file, buf, 10)<1)
     return -1;
  if((pid=atoi(buf))) {
     kill(pid, SIGTERM);
  }

  close(file);  
  unlink(PIDFILE);
  return 0;
}

astrocam_conf AC_ReadConf(void)
{
  astrocam_conf acam_conf;
  FILE *file;
  entry aentry;
  
  bzero(&acam_conf, sizeof(astrocam_conf));
  
  if((file = fopen(CONFIG_FILE, "r"))==NULL){
    perror(CONFIG_FILE);
    exit(1);
  }

  while(fscanf(file, "%98s\t%98s\n", aentry.var, aentry.value)!=EOF)
  {
    printf("%14s: %s\n", aentry.var, aentry.value);
    if(strcmp(aentry.var, "version")==0)
      acam_conf.conf_ver = atof(aentry.value);
    else if(strcmp(aentry.var, "ipckey")==0)
      acam_conf.ipckey = atoi(aentry.value);
    else if(strcmp(aentry.var, "maxdreh")==0)
      acam_conf.maxdreh = atoi(aentry.value);
    else if(strcmp(aentry.var, "mindreh")==0)
      acam_conf.mindreh = atoi(aentry.value);
    else if(strcmp(aentry.var, "maxvdreh")==0)
      acam_conf.maxvdreh = atoi(aentry.value);
    else if(strcmp(aentry.var, "minvdreh")==0)
      acam_conf.minvdreh = atoi(aentry.value);
    else if(strcmp(aentry.var, "iobase")==0)
      acam_conf.iobase = atoi(aentry.value);
    else if(strcmp(aentry.var, "negation")==0) {
      if(aentry.value[1]=='n') /* on */
         acam_conf.negation = 1;
      else /* off */
         acam_conf.negation = 0;
    } else if(strcmp(aentry.var, "design")!=0
           && strcmp(aentry.var, "contenturl")!=0
           && strcmp(aentry.var, "picfile")!=0
           && strcmp(aentry.var, "refreshtime")!=0
           && strcmp(aentry.var, "imagesize-x")!=0
           && strcmp(aentry.var, "imagesize-y")!=0) {
      fprintf(stderr, CONFIG_FILE ": unsupported variable found! "
      		"name is '%s'\n", aentry.var);
      sighndl(0);
      exit(0);
    }
  }
  fclose(file);
  return acam_conf;
}

int main(void)
{
  FILE *fdpid;
  pid_t mypid;
  char *buf;
  int j;
  ipcrecv recvdata;
  
  j=0x0;
  
  if(geteuid()!=0){
     logit("need root access!");
     fprintf(stderr, "need root-access to open the I/O-device directly!\n");
	 return -1;
  }
  
  if(chkrunstat()<0)
     if(restartac()<0){
        fprintf(stderr, "--- Alert: Can't restart astrocam daemon!\n");
        return -1;
     }
  /* 2.7.2: In the past AC_ReadConf() was called after AstroCamInit()
   * was done.
   */
  globconf=AC_ReadConf();
  if(globconf.conf_ver!=globver){
     fprintf(stderr, "incompatible version in configuration file "
                     CONFIG_FILE "!\n");
     sighndl(1000);
     exit(1);
  }
  printf("using IO address 0x%x...\n", globconf.iobase);
  
  if(AstroCamInit()!=0){
     perror("AstroCamInit() returned zero. exiting.");
     sighndl(1000);
     return -1;
  }
 
  signal(SIGINT|SIGQUIT|SIGTERM, sighndl);

  /* default init position is 0 */
  globconf.curpos=0;
  globconf.curvpos=0;
  
  if((srvid=msgget(globconf.ipckey, S_IRWXU|S_IWGRP|S_IWOTH|IPC_CREAT))==-1){
    perror("msgget");
    sighndl(1000);
    return -1;
  }

  if((mypid=fork())<0){
     perror("fork() ");
     sighndl(0);
     return -1;
  }else if(mypid!=0){
     exit(1);
  }else{
     dm(&mypid);
     printf("\nstoring astrocam process-id in " PIDFILE "! [%d]", mypid);
  }

  if((buf=(char *)malloc(11*sizeof(char)))==NULL){
     perror("malloc ");
     sighndl(0);
     return -1;
  }
  snprintf(buf, 10, "%i", mypid);
  if((fdpid=fopen(PIDFILE, "w+"))==NULL){
     perror(PIDFILE);
     sighndl(0);
     return -1;
  }
  
  /* v. 2.6.3: secure file perm: -rw------- */
  /* (don't stop if chmod() fails, AC will run anyway) */
  if(fchmod(fileno(fdpid), PID_CHMOD)!=0) {
     perror(PIDFILE);
     return -1;
  }
	
  /* NOW write the content */
  if(fputs(buf, fdpid)<0) {
     perror("fputs");
     return -1;
  }
  fclose(fdpid);

  welcome();
  fflush(stdout);
  
  while(msgrcv(srvid, &recvdata, 10, 0, MSG_NOERROR)!=-1) {
    /* PHP sends on recvdata.data[5] and Perl on [0]. We need a
     * offset that is equal in both cases.
     */
    if(recvdata.data[0]=='s') /* PHP-> Perl */
       recvdata.data[0]=recvdata.data[5];

/* implemented with version 2.6.0 */
    if(globconf.negation==1) {
       if(recvdata.data[0]=='L') recvdata.data[0]='R';
       if(recvdata.data[0]=='l') recvdata.data[0]='r';
       if(recvdata.data[0]=='R') recvdata.data[0]='L';
       if(recvdata.data[0]=='r') recvdata.data[0]='l';
       /* do not implement this feature for the hor. movement
       if(recvdata.data[0]=='d') recvdata.data[0]='u';
       if(recvdata.data[0]=='u') recvdata.data[0]='d';*/
    }
/* hpos stuff */
    if(recvdata.data[0]!='0')
    {
       j=0;
       if(recvdata.data[0]=='L'){
         if(globconf.mindreh<globconf.curpos-19)
           while(j<20){
	     AstroCam_Step(3, 1);
	     j++;
	     globconf.curpos--;
	   }
       }else if(recvdata.data[0]=='l'){
         if(globconf.mindreh<globconf.curpos-9)
           while(j<10){
	     AstroCam_Step(3, 1);
	     j++;
	     globconf.curpos--;
	   }
       }else if(recvdata.data[0]=='R'){
         if(globconf.maxdreh>globconf.curpos+19)
           while(j<20){
	     AstroCam_Step(0, -1);
	     j++;
	     globconf.curpos++;
	   }
       }else if(recvdata.data[0]=='r'){
         if(globconf.maxdreh>globconf.curpos+9)
	   while(j<10){
	     AstroCam_Step(0, -1);
	     j++;
	     globconf.curpos++;
	   }
/* vpos stuff */
       }else if(recvdata.data[0]=='u'){
         if(globconf.maxvdreh>globconf.curvpos+9)
           while(j<10){
	     AstroCam_Step(1, 1);
	     j++;
	     globconf.curvpos++;
	   }
       }else if(recvdata.data[0]=='d'){
         if(globconf.minvdreh<globconf.curvpos-9)
           while(j<10){
	     AstroCam_Step(1, -1);
	     j++;
	     globconf.curvpos--;
	   }
/* error mngmnt */
       } else { /* simple error management. implemented with v. 2.5.2 */
          logit("error: recv bad char from webinterface");
       }
#if defined AC_DEBUG
       printf("%s", recvdata.data);fflush(stdout);
       printf("\ncurpos: %i  (min: %i | max: %i)\n",
         globconf.curpos,
         globconf.mindreh,
         globconf.maxdreh);
#endif
    }
  }

  logit("msgrcv-error in mainloop!");
  sighndl(0);
  free(buf);
  exit(0);
}

