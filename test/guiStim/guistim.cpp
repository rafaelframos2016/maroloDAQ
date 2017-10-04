#include "guistim.h"
#include "ui_guistim.h"
#include <QDebug>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#if 0
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/timeb.h>
#include <sys/time.h>
#endif

#include "receiverMsgs.h"

#define _ALLOC
#include "receiver.h"
#undef _ALLOC

#include "simpl.h"
#include "receiverProto.h"
#include "simplProto.h"
#include "loggerProto.h"
#include "simplmiscProto.h"

guiStim::guiStim(QWidget *parent) :
QWidget(parent),
ui(new Ui::guiStim)
{
    ui->setupUi(this);
    QPalette paleta;
    
    //notifier = (STDIN_FILENO, QSocketNotifier::Read, parent);
    
    
    paleta.setColor(QPalette::Base,Qt::black);
    paleta.setColor(QPalette::Text,Qt::white);
    ui->teLog->setPalette(paleta);
}

guiStim::~guiStim()
{
    delete ui;
}

void guiStim::on_pbExit_clicked()
{
    //name_detach();
    exit(0);
}

void guiStim::on_editSendCmd_returnPressed()
{
}

/*============================================
	initialize - entry point
============================================*/
void initialize(int argc, char **argv)
{
static char *fn="initialize";
int i;                          /* loop variable */
char myName[20];
char loggerName[20];
int myslot;

globalMask=0xff;
myName[0]=0;

/*===============================================
  process command args
===============================================*/
for(i=1; i<=argc; ++i)
        {
        char *p = argv[i];

        if(p == NULL) continue;

        if(*p == '-')
                {
                switch(*++p)
                        {
                        case 'n':
                                for(;*p != 0; p++);
				sprintf(myName,"%s",++p);
				myslot=name_attach(myName, NULL);
				if(myslot == -1)
					{
					printf("%s: unable to attach as <%s>\n",
						fn,
						myName);
					exit(0);
					}
				else
					printf("attached as <%s> myslot=%d\n",myName,myslot);
                                break;

			case 'l':
                                for(;*p != 0; p++);
				sprintf(loggerName,"%.19s",++p);
				break;

			case 'm':
				if(*++p == 0) p++;
				globalMask=atoh(&p[2]);  // skip 0x
				break;

                        default:
				printf("%s:unknown arg %s\n",fn, p);
                                break;
                        }/*end switch*/
                } /* end if *p */
        }/*end for i*/

// check for compulsory args
if(myName[0] == 0)
	{
	myUsage();
	exit(0);
	}

// try to connect to trace logger
logger_ID = is_logger_upx(loggerName);

fcLogx(__FILE__, fn,
	0xff,
	RECV_MARK,
	"myName=<%s> myslot=%d",
	myName,
	myslot
	);

fcLogx(__FILE__, fn,
	0xff,
	RECV_MARK,
	"trace logger mask = 0x%04X",
	globalMask);

} /* end initialize */

/*===================================================
	myUsage - entry point
===================================================*/
void myUsage()
{
printf("====================== usage =============================\n");
printf(" usage for receiver:\n");
printf("      receiver -n <myName> <optionals>\n");
printf("      where optionals are:\n");
printf("        -m 0x<mask> - trace logger mask override\n");
printf("        -l <loggerName> - connect to trace logger\n");
printf("==========================================================\n");
}// end myUsage
