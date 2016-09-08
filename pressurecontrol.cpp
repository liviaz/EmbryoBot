#include "pressurecontrol.h"
#include <QtSerialPort/QtSerialPort>
#include <QDebug>
#include <QMessageBox>
#include <stdio.h>
#include <QThread>

#define VALVE_PIN 3
#define VENT_PIN 4
#define MOTOR_PIN 9
#define START_ZERO_PRESSURE 509


PressureControl::PressureControl(QObject *parent) : QObject(parent)
{

    pressureOffset = START_ZERO_PRESSURE;
    portNameList = NULL;
    sensorVoltage = -1;
    motorPosition = -1;
    pressure = 0;
    serialData.clear();
    port = NULL;


}


// destructor
PressureControl::~PressureControl(){
    if (port){
        if (port->isOpen()){
            port->close();
        }
        delete port;
    }
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
    motorPosition = value;

    if (value > 2000){
        motorPosition = 2000;
    } else if (value < 1000) {
        motorPosition = 1000;
    }


    QByteArray valueArray = QByteArray();
    valueArray.append((unsigned char) 0x09);
    valueArray.append((unsigned char) (motorPosition >> 8));
    valueArray.append((unsigned char) motorPosition);
    valueArray.append((unsigned char) 0xFF);
    valueArray.append((unsigned char) 0xFF);

    qDebug() << "array written: " << valueArray;

    port->write(valueArray);
    port->flush();
}



// initialize motor position and clear pressure
void PressureControl::initMotor(int startingValue){

    // vent
    this->setVent(true);
    QThread::msleep(200);

    // move motor to starting position
    this->moveMotor(startingValue);

    // wait a couple seconds
    QThread::msleep(2000);

    // close vent
    this->setVent(false);
    QThread::msleep(200);

    waitUntilMoveDone(500, 10, 1);

    emit updateMotorPosition(startingValue);
    emit motorInitialized();
}




// control loop to go to a certain pressure
// this is the main function used to achieve a desired pressure
void PressureControl::goToPressure(double desiredPressure, int flag)
{
    int numCycles = 0;
    int maxCycles = 20;
    double accuracy = .0025; // pressure accuracy
    int P = 300; // gain
    int moveIncrement = 0;

    // move motor, wait until it's done, move again, etc.
    while (qAbs(desiredPressure - pressure) > accuracy && numCycles < maxCycles){

        // determined how much to move motor
        moveIncrement = (desiredPressure - pressure) * P;

        if (moveIncrement > 0 && moveIncrement < 5){
            moveIncrement = 5;
        } else if (moveIncrement < 0 && moveIncrement > -5) {
            moveIncrement = -5;
        }

        // move motor
        motorPosition -= moveIncrement;
        this->moveMotor(motorPosition);

        numCycles++;

        // wait until it's done before moving again
        waitUntilMoveDone(200, 20, 0);
    }

    // give camera some time to adjust in other thread
    // QThread::msleep(1000);

    if (numCycles == maxCycles && flag == 0){
        emit balanceFinished(0, flag);
    }

    emit balanceFinished(1, flag);

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

    //qDebug() << "serial data: " << serialData;
    //serialData.clear();

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
void PressureControl::waitUntilMoveDone(int waitTimeMs, int maxCycles, bool duringInit){

    onDataAvailable();
    double lastPressure = pressure;
    bool stillMoving = true;
    int numCycles = 0;

    while (stillMoving && numCycles < maxCycles){

        // wait, then update pressure by checking for new serial data
        QThread::msleep(waitTimeMs);
        onDataAvailable();

        if (qAbs(pressure - lastPressure) < .002){
            stillMoving = false;
        } else {
            lastPressure = pressure;
            numCycles++;

            if (duringInit){
                setVent(true);
                QThread::msleep(500);
                setVent(false);
            }
        }

        if (numCycles == maxCycles){
            qDebug() << "waited too long for pressure to stabilize";
        }
    }

}












