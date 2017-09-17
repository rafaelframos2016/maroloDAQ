#include "maroloDAQ.h"
#include "ui_maroloDAQ.h"

#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
//#include <QActionGroup>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <string>

maroloDAQ::maroloDAQ(QWidget *parent) :
QMainWindow(parent),
ui(new Ui::maroloDAQ)
{
    ui->setupUi(this);
    //QActionGroup *PortasGroup = new QActionGroup(this);
    //PortasGroup->setExclusive(true);

    // Procurando por portar seriais abertas
    scanPortas();
    
    // habilitando|desabilitando menus|actions
    ui->btnAppClose->setEnabled(true);
    ui->actionSair->setEnabled(true);
    ui->actionConectar->setEnabled(true);
    ui->actionDesconectar->setEnabled(false);
    ui->actionRecarregar->setEnabled(true);
    
}

maroloDAQ::~maroloDAQ()
{
    delete ui;
}

void maroloDAQ::on_btnAppClose_clicked()
{
    exit(0);
}

void maroloDAQ::WriteData(const QByteArray data)
{
    procSerial->Write(data);
}

QString maroloDAQ::ReadData()
{
    QString data = procSerial->Read();
    //qDebug() << "ReadData - RX UART: " << data << endl;
    return data;
}

void maroloDAQ::scanPortas() {
    
    QString GetInfoHw;
    QStringList InfoHW;

    // Serials Port Group
    PortasGroup = new QActionGroup(this);
    QStringList myAction_split;
    
    /* Create Object the Class QSerialPort*/
    devserial = new QSerialPort(this);
    /* Create Object the Class comserial to manipulate read/write of the my way */
    procSerial = new comserial(devserial);
    /* Load Device PortSerial available */
    QStringList DispSeriais = procSerial->CarregarDispositivos();
    // fechando tudo
    //bool statusCloseSerial;
    
    // algumas variaveis temporarias    
    int count = DispSeriais.count();
    //const char * meuAction;
    //const char * meuAction_tmp;
    QString minhaSerial;
    
    if(DispSeriais.length() > 0) {
        
        for(int i=0;i<count;i++) {
            
            minhaSerial = DispSeriais[i];
            
            procSerial->Conectar(minhaSerial,9600);
            
            // aguardando a porta "aquecer" ;_(((
            sleep(2);
            
            // Se conectou com sucesso no disposito serial
            
            // Enviando comando para obter informações do Device
            WriteData("12\n");
            // * Recebendo as informações *
            GetInfoHw = ReadData();
            GetInfoHw = GetInfoHw.simplified();
            
            // * Confirmando se recebeu os dados *
            if( GetInfoHw.size() > 0 ) {
                
                // encontrei maroloDAQ
                minhaSerial = minhaSerial+" [maroloDAQ]";
                // escrevendo no terminal
                ui->teLog->append("### maroloDAQ ABERTO com Sucesso!");
                
                // fechando a serial com o maroloDAQ
                //statusCloseSerial = procSerial->Desconectar();
                
                // encerrando a busca por maroloDAQ
            }  
            
            // criando action para o submenu
            //minhaSerial = "/dev/"+minhaSerial;
            setPortasSeriais(minhaSerial);
            
            //qDebug() << "AQUI minhaSerial = " << minhaSerial << endl;                    
        }
    }
    else {
        ui->teLog->append("### Nenhuma porta serial foi detectada!");
    }

    // ref.: https://stackoverflow.com/questions/9399840/how-to-iterate-through-a-menus-actions-in-qt
    foreach (QAction* action, ui->menuPortas->actions())
    {
        PortasGroup->addAction(action);
        
        myAction_split = action->text().split("]");
        //qDebug() << "$$$$ AQUI action = " << action->text();
        if ( myAction_split.length() > 1 ) {
            if (  myAction_split[1] == "maroloDAQ]") {
                action->setCheckable(true);
                action->setChecked(true);
            }
        }
        else {
            action->setCheckable(true);
            action->setChecked(false);
        }
        //connect(PortasGroup,SIGNAL(triggered(QAction*)),this,SLOT(on_actionDevices_triggered(QAction*)));
    }
}

void maroloDAQ::maroloDevClose()
{
    bool statusCloseSerial;

    // Serials Port Group
    PortasGroup = new QActionGroup(this);
    // removing all ations in PortasGroup
    QStringList myAction_split;
    
    foreach (QAction* action, ui->menuPortas->actions()) {
        PortasGroup->removeAction(action);
        ui->menuPortas->removeAction(action);
    }
    
    statusCloseSerial = procSerial->Desconectar();
    
    // Descontando a porta serial com sucesso
    // Desabilito os botões Versao, Desconectar, Hardware, Ligar [F10]
    // Habilito Sair e Conectar
    
    if (statusCloseSerial) {
        // habilitando|desabilitando menus|actions
        //ui->btnDevClose->setEnabled(false);
        //ui->btnDevOpen->setEnabled(true);
        ui->btnAppClose->setEnabled(true);
        ui->actionSair->setEnabled(true);
        ui->actionConectar->setEnabled(true);
        ui->actionDesconectar->setEnabled(false);
        ui->actionRecarregar->setEnabled(true);
        
        ui->btnCalibrarSensor->setEnabled(false);
        ui->btnIniciar->setEnabled(false);
        ui->btnParar->setEnabled(false);
        
        ui->editDevCompiler->clear();
        ui->editDevModel->clear();
        
        ui->teLog->append("### Porta serial fechada com sucesso!");
    }
    else {
        ui->teLog->append("### Falha ao fechar conexão serial.");
    }
}

void maroloDAQ::on_btnCalibrarSensor_clicked()
{
    
}

void maroloDAQ::on_btnBWTerminal_clicked()
{
    QPalette paleta;
    /*
     * Verifica se PaletaBW é True ou False
     * Se True: Fundo Preto, Fonte Branco
     * Se False: Fundo Branco, Fonte Preto
     */
    
    if(PaletaLogBW) {
        paleta.setColor(QPalette::Base,Qt::black);
        paleta.setColor(QPalette::Text,Qt::white);
        ui->teLog->setPalette(paleta);
        PaletaLogBW=false;
    }
    else {
        paleta.setColor(QPalette::Base,Qt::white);
        paleta.setColor(QPalette::Text,Qt::black);
        ui->teLog->setPalette(paleta);
        PaletaLogBW=true;
    }
}


void maroloDAQ::on_btnDevOpen_clicked()
{
    QString GetInfoHw;
    QStringList InfoHW;
    QString devport;
    QStringList devport_list;
    
    foreach (QAction* action, ui->menuPortas->actions()) {
        if (action->isChecked()) {
            devport = action->text();
            devport = devport.simplified();
            devport_list = devport.split(" [");
            devport = devport_list[0];
        }    
    }
    
    qDebug() << "AQUI btnDevOpen: devport = " << devport;
    procSerial->Conectar(devport,9600);
    
    /*
     * aguardando a porta "aquecer" ;_(((
     *
     */
    sleep(2);
    
    /*
     * Se conectou com sucesso no disposito serial
     * Desabilito o botão Conectar e Sair
     * Habilito Desconectar, Versao, Hardware e Ligar [F10]
     */
    
    // * Enviando comando para obter informações do Device *
    WriteData("12\n");
    // * Recebendo as informações *
    GetInfoHw = ReadData();
    GetInfoHw = GetInfoHw.simplified();
    
    // * Confirmando se recebeu os dados *
    if( GetInfoHw.size() > 0 ) {
        
        
        // * Ex: 4.3.2|UNO
        //  * O que chegou pela serial foi adicionado na variavel GetInfoHW
        //  * então acima removemos qualquer tabulação e abaixo um split
        //  * baseado no caractere |, então sera quebrado em 2 posicoes
        //  * 0 - 4.3.2
        //  * 1 - UNO
        //  *
        InfoHW = GetInfoHw.split("|");

        // habilitando|desabilitando menus|actions
        ui->btnAppClose->setEnabled(false);
        ui->actionSair->setEnabled(false);
        ui->actionConectar->setEnabled(false);
        ui->actionDesconectar->setEnabled(true);
        ui->actionRecarregar->setEnabled(false);
        
        // Inserindo nos devidos Edits
        ui->editDevCompiler->setText(InfoHW[0]);
        ui->editDevModel->setText(InfoHW[1]);
        
        ui->teLog->append("### maroloDAQ Aberto com Sucesso!");
    }
    else {
        // habilitando|desabilitando menus|actions
        /*
        ui->btnAppClose->setEnabled(false);
        ui->actionSair->setEnabled(true);
        ui->actionConectar->setEnabled(false);
        ui->actionDesconectar->setEnabled(false);
        ui->actionRecarregar->setEnabled(true);
        */
        ui->teLog->append("### Erro ao obter informações do maroloDAQ, tente novamente.");
    }
}

void maroloDAQ::on_btnDevClose_clicked()
{
    maroloDevClose();
}

void maroloDAQ::on_btnParar_clicked()
{
    
}

void maroloDAQ::on_btnIniciar_clicked()
{
    
}

void maroloDAQ::on_actionSalvar_como_triggered()
{
    
}

void maroloDAQ::on_actionSalvar_triggered()
{
    
}

void maroloDAQ::on_actionSair_triggered()
{
    exit(0);
}

void maroloDAQ::on_actionConectar_triggered()
{
    QString GetInfoHw;
    QStringList InfoHW;
    
    // * Enviando comando para obter informações do Device *
    WriteData("12\n");
    // * Recebendo as informações *
    GetInfoHw = ReadData();
    GetInfoHw = GetInfoHw.simplified();
    qDebug() << "AQUI GetInfoHW = " << GetInfoHw;
    
    // * Confirmando se recebeu os dados *
    if( GetInfoHw.size() > 0 ) {
        
        // * Ex: 4.3.2|UNO
        //  * O que chegou pela serial foi adicionado na variavel GetInfoHW
        //  * então acima removemos qualquer tabulação e abaixo um split
        //  * baseado no caractere |, então sera quebrado em 2 posicoes
        //  * 0 - 4.3.2
        //  * 1 - UNO
        //  *
        InfoHW = GetInfoHw.split("|");
        
        // habilitando|desabilitando menus|actions
        ui->btnAppClose->setEnabled(true);
        ui->actionSair->setEnabled(false);
        ui->actionConectar->setEnabled(false);
        ui->actionDesconectar->setEnabled(true);
        ui->actionRecarregar->setEnabled(false);
        
        // Inserindo nos devidos Edits
        ui->editDevCompiler->setText(InfoHW[0]);
        ui->editDevModel->setText(InfoHW[1]);
        
        ui->teLog->append("### maroloDAQ Aberto com Sucesso!");
    }
    else {
        ui->teLog->append("### FALHA ao Conectar com maroloDAQ!");
    }
}

void maroloDAQ::on_actionDesconectar_triggered()
{
    maroloDevClose();

    /*
    bool statusCloseSerial;
    
    statusCloseSerial = procSerial->Desconectar();
    
    
    // Descontando a porta serial com sucesso
    // Desabilito os botões Versao, Desconectar, Hardware, Ligar [F10]
    // Habilito Sair e Conectar
    
    if (statusCloseSerial) {
        // habilitando|desabilitando menus|actions
        //ui->btnDevClose->setEnabled(false);
        //ui->btnDevOpen->setEnabled(true);
        ui->btnAppClose->setEnabled(true);
        ui->actionSair->setEnabled(true);
        ui->actionConectar->setEnabled(true);
        ui->actionDesconectar->setEnabled(false);
        ui->actionRecarregar->setEnabled(true);
        
        ui->btnCalibrarSensor->setEnabled(false);
        ui->btnIniciar->setEnabled(false);
        ui->btnParar->setEnabled(false);
        
        ui->editDevCompiler->clear();
        ui->editDevModel->clear();
        
        ui->teLog->append("### Porta serial fechada com sucesso!");
    }
    else {
        ui->teLog->append("### Falha ao fechar conexão serial.");
    }
    */
}

void maroloDAQ::on_actionRecarregar_triggered()
{
    /*
    // Serials Port Group
    PortasGroup = new QActionGroup(this);
    // removing all ations in PortasGroup
    QStringList myAction_split;
    
    foreach (QAction* action, ui->menuPortas->actions()) {
        PortasGroup->removeAction(action);
        ui->menuPortas->removeAction(action);
    }
    */
    scanPortas();
}

void maroloDAQ::setPortasSeriais(QString myAction) {
    //////////////////////////////////////////////////////
    //ref.: http://zetcode.com/gui/qt5/menusandtoolbars/
    QString myAction_spare;
    QString myAction_temp;
    QStringList myAction_split;
    myAction = myAction.simplified();
    //myAction_temp = myAction;
    myAction_split = myAction.split(" [");
    myAction_temp = myAction_split[0];
    myAction = "/dev/"+myAction;

    /*
    if (myAction_split.length() > 1) {
        myAction_temp = myAction_split[0];
        //qDebug() << "# AQUI myAction_split[0] = " << myAction_split[0];
        //qDebug() << "# AQUI myAction_split[1] = " << myAction_split[1];
    }
    */
    
    int ihavename = 0;
    foreach (QAction *action, ui->menuPortas->actions()) {
        if ( !(action->isSeparator()) || !(action->menu()) ) {
            //myAction_spare = action->text();
            /*
            myAction_spare = myAction_spare.simplified();
            myAction_sparelt = myAction_spare.split(" [");
            myAction_spare = myAction_sparelt[0];
                
            if (myAction_sparelt.length() > 1) {
                myAction_spare = myAction_sparelt[0];
                //qDebug() << "# AQUI myAction_split[0] = " << myAction_split[0];
                //qDebug() << "# AQUI myAction_split[1] = " << myAction_split[1];
            }
            */
            //qDebug() << "# AQUI myAction = " << myAction;
            //qDebug() << "# AQUI action->text() = " << action->text();
            
            //if ( QString::compare(myAction_temp, myAction_spare, Qt::CaseInsensitive) ) {
            if ( myAction == action->text() ) {
                ihavename = ihavename + 1;
                qDebug() << "# AQUI myAction = " << myAction;
                qDebug() << "# AQUI action->text() = " << action->text();
                //qDebug("# action->text() isn't menu: %s", qUtf8Printable(action->text()));
                //qDebug() << "#####################################################";
            }
        }
    }
    
    if ( ihavename == 0 ) {
        //myAction="/dev/"+myAction;
        //qDebug() << "##################################################################";
        qDebug() << "###### CRIANDO SUBMENU -- myAction = " << myAction;
        //qDebug() << "##################################################################";
        
        //qDebug() << ">>>>> AQUI myAction_temp = " << myAction_temp;
        
        if ( myAction_temp == "ttyACM0" ) {
            //qDebug() << ">>>>> AQUI myAction_split[1] = " << myAction_split[1];
            actionACM0 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionACM0);
            if (myAction_split.length() > 1 ) {
                if ( myAction_split[1] == "maroloDAQ]") {
                    actionACM0->setCheckable(true);
                    actionACM0->setChecked(true);
                }
            }
        }
        if ( myAction_temp == "ttyACM1" ) {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionACM0 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionACM1);
            if (myAction_split.length() > 1 ) {
                if ( myAction_split[1] == "maroloDAQ]") {
                    actionACM0->setCheckable(true);
                    actionACM0->setChecked(true);
                }
            }
        }
        if ( myAction_temp == "ttyACM2" ) {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionACM0 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionACM2);
            if (myAction_split.length() > 1 ) {
                if ( myAction_split[1] == "maroloDAQ]") {
                    actionACM0->setCheckable(true);
                    actionACM0->setChecked(true);
                }
            }
        }
        if ( myAction_temp == "ttyACM3" ) {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionACM0 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionACM3);
            if (myAction_split.length() > 1 ) {
                if ( myAction_split[1] == "maroloDAQ]") {
                    actionACM0->setCheckable(true);
                    actionACM0->setChecked(true);
                }
            }
        }
        if (  myAction_temp == "ttyS0") {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionS0 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionS0);
            actionS0->setCheckable(true);
        }
        if (  myAction_temp == "ttyS1") {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionS1 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionS1);
            actionS1->setCheckable(true);
        }
        if (  myAction_temp == "ttyS2") {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionS2 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionS2);
            actionS2->setCheckable(true);
        }
        if (  myAction_temp == "ttyS3") {
            //qDebug() << ">>>>> AQUI myAction_split[0] = " << myAction_split[0];
            actionS3 = new QAction(myAction, this);
            ui->menuPortas->addAction(actionS3);
            actionS3->setCheckable(true);
        }
    }
    /*
    // ref.: https://stackoverflow.com/questions/9399840/how-to-iterate-through-a-menus-actions-in-qt
    PortasGroup = new QActionGroup(this);
    foreach (QAction* action, ui->menuPortas->actions())
    {
        PortasGroup->addAction(action);
        actions->setCheckable(true);
    }
    connect(PortasGroup,SIGNAL(triggered(QAction*)),this,SLOT(on_actionDevices_triggered(QAction*)));
    */
}

void maroloDAQ::enumerateMenu(QMenu *menu) {
    //////////////////////////////////////////////////////
    //ref.: https://stackoverflow.com/questions/9399840/how-to-iterate-through-a-menus-actions-in-qt

    foreach (QAction *action, menu->actions()) {
        
        if (action->isSeparator()) {
            //qDebug() << "this action is a separator.";// << "" << endl;
        } else if (action->menu()) {
            qDebug() << "action1: %s:" << qUtf8Printable(action->text());// << endl;
            qDebug() << "this action is associated with a submenu, iterating it recursively...";// << "" << endl;
            enumerateMenu(action->menu());
            qDebug() << "finished iterating the submenu";// << "" << endl;
        } else {
            qDebug() << "action2: %s" << qUtf8Printable(action->text());// << endl;
        }
    }
}
