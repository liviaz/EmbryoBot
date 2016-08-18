#include "frontpanel.h"
#include "ui_frontpanel.h"
#include "pressurecontrol.h"

#include <QThread>



FrontPanel::FrontPanel(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FrontPanel)
{

    // initialize variables
    portOpen = 0;
    pressure = 0;
    arduinoStartable = 0;
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

    // start PressureControl
    pc = new PressureControl();


    // set up threads
    QThread* thread_pc = new QThread;
    pc->moveToThread(thread_pc);


    // set up thread connections for pressure



    // set up thread connections for camera



    // set up data connections


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













