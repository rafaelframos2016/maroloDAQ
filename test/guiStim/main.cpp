#include "guistim.h"
#include <QApplication>

int main(int argc, char *argv[])
//int main(int argc, char **argv, char **envp)
{

    QApplication a(argc, argv);
    guiStim w;
    w.show();
    
    return a.exec();
}
