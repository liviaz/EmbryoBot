#include "frontpanel.h"
#include "ui_frontpanel.h"
#include "pressurecontrol.h"

#include <QThread>
#include <QFileDialog>
#include <QDebug>


FrontPanel::FrontPanel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrontPanel)
{

    // initialize variables
    portName = "";
    portOpen = 0;
    pressure = 0;
    motorPosition = 1000 + MOTOR_INIT_PCT * 10;

    balancePressureValue = -0.035;
    measurePressureValue = 0.1;
    valveOpen = false;
    ventOpen = false;


    // setup UI
    ui->setupUi(this);
    ui->balancePressureDouble->setValue(balancePressureValue);
    ui->measurePressureDouble->setValue(measurePressureValue);
    ui->valveButton->setText("OPEN VALVE");
    ui->ventButton->setText("VENT");
    ui->zeroPressure->setEnabled(false);
    ui->valveButton->setEnabled(false);
    ui->ventButton->setEnabled(false);
    ui->horizontalSlider->setMinimum(1000);
    ui->horizontalSlider->setMaximum(2000);
    ui->actualPressure->setText("+0.000 psi");
    ui->initializeButton->setEnabled(false);
    ui->balanceButton->setEnabled(false);
    ui->measureButton->setEnabled(false);
    ui->horizontalSlider->setValue((int) MOTOR_INIT_PCT * 10 + 1000);
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    ui->arduinoConnectBtn->setEnabled(false);

    // start PressureControl
    pc = new PressureControl();


    // set up threads
    QThread* thread_pc = new QThread;
    pc->moveToThread(thread_pc);


    // set up thread connections for pressure
    connect(thread_pc, SIGNAL(started()), pc, SLOT(getAvailablePorts()));
    connect(pc, SIGNAL(relayPortList(QList<QString>*)), this, SLOT(listAvailablePorts(QList<QString>*)));
    connect(pc, SIGNAL(signalPortOpen()), this, SLOT(arduinoOpenSlot()));

    connect(pc, SIGNAL(signalPortClosed()), this, SLOT(arduinoClosedSlot()));
    connect(pc, SIGNAL(destroyed(QObject*)), thread_pc, SLOT(quit()));
    connect(thread_pc, SIGNAL(finished()), thread_pc, SLOT(deleteLater()));


    // set up thread connections for camera



    // set up data connections
    connect(this, SIGNAL(setMotorPosition(int)), pc, SLOT(moveMotor(int)));
    connect(pc, SIGNAL(updateMotorPosition(int)), this, SLOT(updateMotorPosition(int)));

    connect(this, SIGNAL(initMotor(int)), pc, SLOT(initMotor(int)));
    connect(pc, SIGNAL(motorInitialized()), this, SLOT(motorInitialized()));

    connect(this, SIGNAL(goToPressure(double,int)), pc, SLOT(goToPressure(double,int)));
    connect(pc, SIGNAL(balanceFinished(int,int)), this, SLOT(balanceFinished(int,int)));

    connect(ui->zeroPressure, SIGNAL(clicked(bool)), pc, SLOT(zeroPressure()));
    connect(pc, SIGNAL(relayPressure(double)), this, SLOT(pressureUpdatedSlot(double)));

    connect(this, SIGNAL(setValve(bool)), pc, SLOT(setValve(bool)));
    connect(this, SIGNAL(setVent(bool)), pc, SLOT(setVent(bool)));


    // start theads
    thread_pc->start();

}

FrontPanel::~FrontPanel()
{
    if (pc) {
        delete pc;
    }

    delete ui;
}



/* ************************
 *
 * Initial connections
 *
 * ***********************/


void FrontPanel::on_startButton_clicked()
{
    if (!portOpen){
        qDebug() << "ok ...";
        portName = ui->comPortSelect->currentText();
        pc->openPort(portName);
    }

}



void FrontPanel::on_stopButton_clicked()
{
    if (portOpen){
        pc->closePort();
    }

}

// pull up file selector box
void FrontPanel::on_fileNameButton_clicked()
{
    fileName = QFileDialog::getSaveFileName(this,
                        tr("Select Filename to Save"),
                        "C:/Users/Admin/Desktop/Data");
    ui->fileNameBox->setText(fileName);
}



void FrontPanel::on_fileNameBox_editingFinished()
{
    fileName = ui->fileNameBox->text();
}



void FrontPanel::on_arduinoConnectBtn_clicked()
{


}

void FrontPanel::arduinoOpenSlot()
{
    portOpen = 1;

    ui->stopButton->setEnabled(true);
    ui->initializeButton->setEnabled(true);
    ui->balanceButton->setEnabled(true);
    ui->measureButton->setEnabled(true);
    ui->zeroPressure->setEnabled(true);
    ui->valveButton->setEnabled(true);
    ui->ventButton->setEnabled(true);

    qDebug() << "connection successful!";
}


void FrontPanel::arduinoClosedSlot()
{
    portOpen = 0;
    ui->arduinoConnectBtn->setEnabled(true);
    qDebug() << "connection ended";
}



// fill ui box with available ports
void FrontPanel::listAvailablePorts(QList<QString> *portNamesList)
{

    ui->comPortSelect->clear();

    for (QString currPortName : *portNamesList){
        ui->comPortSelect->addItem(currPortName);
        qDebug() << currPortName;
    }


    // allow connection
    if (ui->comPortSelect->count() > 1){
        ui->comPortSelect->setCurrentIndex(1);
        ui->startButton->setEnabled(true);
    }

}


/* ************************
 *
 * Private Slots - from UI
 *
 * ***********************/



void FrontPanel::on_valveButton_clicked()
{
    if (valveOpen){
        valveOpen = false;
        emit setValve(false);
        ui->valveButton->setText("OPEN VALVE");
    } else {
        valveOpen = true;
        emit setValve(true);
        ui->valveButton->setText("CLOSE VALVE");
    }
}



void FrontPanel::on_ventButton_clicked()
{
    if (ventOpen){
        ventOpen = false;
        emit setVent(false);
        ui->ventButton->setText("OPEN VENT");
    } else {
        ventOpen = true;
        emit setVent(true);
        ui->ventButton->setText("CLOSE VENT");
    }
}


// take value from horizontal slider and relay to motor
void FrontPanel::on_horizontalSlider_sliderReleased()
{
    // motor position goes from 0-50
    // needs to be converted to 1000-2000
    motorPosition = ui->horizontalSlider->value();
    emit setMotorPosition(motorPosition);
}



void FrontPanel::on_initializeButton_clicked()
{
    ui->SystemStatusTextEdit->setText("Initializing ... please wait!");
    ui->initializeButton->setEnabled(false);
    ui->balanceButton->setEnabled(false);
    ui->measureButton->setEnabled(false);
    ui->zeroPressure->setEnabled(false);
    ui->valveButton->setEnabled(false);
    ui->ventButton->setEnabled(false);

    motorPosition = MOTOR_INIT_PCT * 10 + 1000;
    emit initMotor(motorPosition);


}


void FrontPanel::motorInitialized()
{
    ui->SystemStatusTextEdit->setText("");
    ui->initializeButton->setEnabled(true);
    ui->balanceButton->setEnabled(true);
    ui->measureButton->setEnabled(true);
    ui->zeroPressure->setEnabled(true);
    ui->valveButton->setEnabled(true);
    ui->ventButton->setEnabled(true);
}




void FrontPanel::on_balanceButton_clicked()
{

}



void FrontPanel::on_measureButton_clicked()
{

}



// update desired balance pressure when box value changes
void FrontPanel::on_balancePressureDouble_valueChanged(double arg1)
{
    balancePressureValue = arg1;
}


// update desired measurement pressure when box value changes
void FrontPanel::on_measurePressureDouble_valueChanged(double arg1)
{
    measurePressureValue = arg1;
}




void FrontPanel::on_outIncrement_clicked()
{

}


void FrontPanel::on_inIncrement_clicked()
{

}







/* ************************
 *
 * Public Slots
 *
 * ***********************/



void FrontPanel::pressureUpdatedSlot(double pressureIn)
{
    pressure = pressureIn;

    // print out pressure
    QString data_string;
    data_string.sprintf("%+01.4f", pressure);
    data_string.append(" psi");
    ui->actualPressure->setText(data_string);
}



void FrontPanel::balanceFinished(int successful, int flag)
{

}




// update the motor position
void FrontPanel::updateMotorPosition(int value){
    motorPosition = value;
    ui->horizontalSlider->setValue(value);
}





/* ************************
 *
 * Private functions
 *
 * ***********************/

// check if file exists
bool FrontPanel::fileExists(QString path) {
    QFileInfo checkFile(path);

    // TODO: check if directory exists; if not, create it!!!


    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    } else {
        return false;
    }
}









