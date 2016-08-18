#include "pressurecontrol.h"
#include <QtSerialPort/QtSerialPort>



#define VALVE_PIN 3
#define VENT_PIN 4
#define MOTOR_PIN 9
#define START_ZERO_PRESSURE 509


PressureControl::PressureControl(QObject *parent) : QObject(parent)
{

    pressureOffset = START_ZERO_PRESSURE;
    sensorVoltage = -1;
    serialData.clear();
    port = NULL;


}


// destructor
PressureControl::~PressureControl(){
    if (port){
        delete port;
    }
}




// signal that the port was closed successfully
void PressureControl::closePort(){

}



// signal that the port was opened successfully
void PressureControl::openPort(){



}


// send voltage to pressure valve
void PressureControl::setValve(int value){

}


// send voltage to vent valve
void PressureControl::setVent(int value){

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

}


// wait until pressure has stabilized before moving motor again
void PressureControl::waitUntilMoveDone(double desiredPressure){


}












