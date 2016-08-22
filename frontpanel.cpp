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
    motorPosition = MOTOR_INIT_PCT * MAX_STROKE / 100; // units of mm

    balancePressureValue = -0.035;
    measurePressureValue = 0.1;
    valveOpened = 0;


    // setup UI
    ui->setupUi(this);
    ui->balancePressureDouble->setValue(balancePressureValue);
    ui->measurePressureDouble->setValue(measurePressureValue);
    ui->valveButton->setText("OPEN VALVE");
    ui->horizontalSlider->setMaximum(MAX_STROKE);
    ui->actualPressure->setText("+0.000 psi");
    ui->balanceButton->setEnabled(false);
    ui->measureButton->setEnabled(false);
    ui->horizontalSlider->setValue((int) motorPosition);
    ui->startButton->setEnabled(false);
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
    connect(this, SIGNAL(goToPressure(double,int)), pc, SLOT(goToPressure(double,int)));
    connect(pc, SIGNAL(balanceFinished(int,int)), this, SLOT(balanceFinished(int,int)));
    connect(pc, SIGNAL(updateMotorPosition(double)), this, SLOT(updateMotorPosition(double)));
    connect(ui->zeroPressure, SIGNAL(clicked(bool)), pc, SLOT(zeroPressure()));
    connect(pc, SIGNAL(relayPressure()), this, SLOT(pressureUpdatedSlot()));
    connect(this, SIGNAL(setValve(int)), pc, SLOT(setValve(int)));
    connect(this, SIGNAL(setVent(int)), pc, SLOT(setVent(int)));


    // start theads
    thread_pc->start();

}

FrontPanel::~FrontPanel()
{

    // close Arduino port
    if (portOpen){
        pc->closePort();
    }


    delete pc;
    delete ui;
}



/* ************************
 *
 * Initial connections
 *
 * ***********************/


void FrontPanel::on_startButton_clicked()
{
    if (portOpen){

        ui->stopButton->setEnabled(true);
    }


}



void FrontPanel::on_stopButton_clicked()
{

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

    if (!portOpen){
        qDebug() << "ok ...";
        portName = ui->comPortSelect->currentText();
        pc->openPort(portName);
    }
}

void FrontPanel::arduinoOpenSlot()
{
    portOpen = 1;
    ui->startButton->setEnabled(true);
    qDebug() << "connection successful!";
}


void FrontPanel::arduinoClosedSlot()
{

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
    ui->comPortSelect->setCurrentIndex(1);
    ui->arduinoConnectBtn->setEnabled(true);

}


/* ************************
 *
 * Private Slots - from UI
 *
 * ***********************/



void FrontPanel::on_valveButton_clicked()
{

}



void FrontPanel::on_ventButton_clicked()
{

}


// take value from horizontal slider and relay to motor
void FrontPanel::on_horizontalSlider_sliderReleased()
{
    // motor position goes from 0-50
    // needs to be converted to 1000-2000
    double strokePercentage = ((double) ui->horizontalSlider->value())
            / ((double) MAX_STROKE);

    int voltageToWrite = 1000 + strokePercentage * 1000;
    emit setMotorPosition(voltageToWrite);
    motorPosition = strokePercentage * MAX_STROKE;
}



void FrontPanel::on_initializeButton_clicked()
{

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



void FrontPanel::pressureUpdatedSlot()
{

}

void FrontPanel::balanceFinished(int successful, int flag)
{

}




// update the motor position
void FrontPanel::updateMotorPosition(double value){
    motorPosition = value;
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









