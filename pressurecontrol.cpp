#include "pressurecontrol.h"
#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <QMessageBox>


#define VALVE_PIN 3
#define VENT_PIN 4
#define MOTOR_PIN 9
#define START_ZERO_PRESSURE 509


PressureControl::PressureControl(QObject *parent) : QObject(parent)
{

    pressureOffset = START_ZERO_PRESSURE;
    portNameList = NULL;
    sensorVoltage = -1;
    pressure = 0;
    serialData.clear();
    port = NULL;


}


// destructor
PressureControl::~PressureControl(){
    if (port && port->isOpen()){
        port->close();
        delete port;
    }

    emit signalPortClosed();
}



// signal that the port was opened successfully
void PressureControl::openPort(QString portName){

    // try to open port
    port = new QSerialPort();
    port->setPortName(portName);
    port->setBaudRate(9600);
    qDebug() << "opening port " << portName;

    if (port->open(QIODevice::ReadWrite)) {
        connect(port, SIGNAL(readyRead()), this, SLOT(onDataAvailable()));
        emit signalPortOpen();
        qDebug() << "success!!";

    } else {
        qDebug() << "error: " << port->errorString();
    }

}



// signal that the port was closed successfully
void PressureControl::closePort(){
    if (port && port->isOpen()){
        port->close();
        delete port;
    }
    emit signalPortClosed();
}


// get list of available ports once thread is started
void PressureControl::getAvailablePorts()
{

    // get list of available ports
    const auto infos = QSerialPortInfo::availablePorts();

    // initialize list
    if (portNameList == NULL){
        portNameList = new QList<QString>();
    } else {
        portNameList->clear();
    }


    for (const QSerialPortInfo &info : infos) {
        portNameList->append(info.portName());
    }

    relayPortList(portNameList);
}



// send voltage to pressure valve
void PressureControl::setValve(bool value){

    unsigned char valueToWrite;

    if (value){
        valueToWrite = 0x01;
    } else {
        valueToWrite = 0x0;
    }

    QByteArray valueArray = QByteArray();
    valueArray.append((unsigned char) VALVE_PIN);
    valueArray.append((unsigned char) valueToWrite);
    valueArray.append((unsigned char) valueToWrite);
    valueArray.append((unsigned char) 0xFF);
    valueArray.append((unsigned char) 0xFF);

    qDebug() << "array written: " << valueArray;

    port->write(valueArray);
    port->flush();
}


// send voltage to vent valve
void PressureControl::setVent(bool value){

    unsigned char valueToWrite;

    if (value){
        valueToWrite = 0x01;
    } else {
        valueToWrite = 0x0;
    }

    QByteArray valueArray = QByteArray();
    valueArray.append((unsigned char) VENT_PIN);
    valueArray.append((unsigned char) valueToWrite);
    valueArray.append((unsigned char) valueToWrite);
    valueArray.append((unsigned char) 0xFF);
    valueArray.append((unsigned char) 0xFF);

    qDebug() << "array written: " << valueArray;

    port->write(valueArray);
    port->flush();
}



/*
 * zeroPressure:
 * set zero pressure reference to current pressure
 */
void PressureControl::zeroPressure(){

    //std::cout << "pressureOffset: " << pressureOffset << std::endl;
    if (sensorVoltage > 0){
        pressureOffset = sensorVoltage;
    }
}




// move to position
void PressureControl::moveMotor(int value)
{
    int valueToWrite = value;

    if (value > 1023){
        valueToWrite = 1023;
    } else if (value < 0) {
        valueToWrite = 0;
    }

    QByteArray valueArray = QByteArray();
    valueArray.append((unsigned char) MOTOR_PIN);
    valueArray.append((unsigned char) valueToWrite);
    valueArray.append((unsigned char) (valueToWrite >> 8));
    valueArray.append((unsigned char) 0xFF);
    valueArray.append((unsigned char) 0xFF);

    port->write(valueArray);
    port->flush();
}




// control loop to go to a certain pressure
// this is the main function used to achieve a desired pressure
void PressureControl::goToPressure(double desiredPressure, int flag)
{

}



/*
 * onDataAvailable: read data from the serial port
 * If it does not end in appropriate character, put it
 * in serialData variable and keep waiting for more data
 */
void PressureControl::onDataAvailable()
{
    firstChar = 0;
    secondChar = 0;
    serialData.append(port->readAll());

    // check if we've received data
    if (serialData.size() > 0){

        // if last 2 chars are 0xFF
        int lastEndIndex = serialData.lastIndexOf((unsigned char) 0xFF);
        if (lastEndIndex > 2 && ((unsigned char) serialData[lastEndIndex-1] == 0xFF)){

            firstChar = ((unsigned char) serialData[lastEndIndex-3]);
            secondChar = ((unsigned char) serialData[lastEndIndex-2]);

            sensorVoltage =  firstChar*256 + secondChar;
            pressure = ((double) sensorVoltage - pressureOffset) / 36.95 * .03614;
            serialData.clear();

            emit relayPressure(pressure);
        }
    }
}


// wait until pressure has stabilized before moving motor again
void PressureControl::waitUntilMoveDone(double desiredPressure){


}












