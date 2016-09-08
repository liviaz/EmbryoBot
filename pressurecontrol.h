#ifndef PRESSURECONTROL_H
#define PRESSURECONTROL_H

#include <QObject>
#include <QByteArray>
#include <QtSerialPort>
#include <QString>

#define MOTOR_INIT_PCT 70
#define MAX_STROKE 50


class PressureControl : public QObject
{
    Q_OBJECT


public:
    explicit PressureControl(QObject *parent = 0);
    ~PressureControl();
    void openPort(QString portName);
    void closePort();



private:
    QSerialPort *port;
    QList<QString> *portNameList;
    unsigned int sensorVoltage;
    unsigned char firstChar;
    unsigned char secondChar;
    double pressure;
    double pressureOffset;
    int motorPosition;
    QByteArray serialData;
    void waitUntilMoveDone(int waitTimeMs, int maxCycles, bool duringInit);


signals:
    void signalPortClosed();
    void signalPortOpen();
    void relayPressure(double pressureOut);
    void balanceFinished(int successful, int flag);
    void updateMotorPosition(int value);
    void relayPortList(QList<QString> *);
    void motorInitialized();


public slots:
    void zeroPressure();
    void moveMotor(int value);
    void goToPressure(double desiredPressure, int flag);
    void setValve(bool value);
    void setVent(bool value);
    void getAvailablePorts();
    void initMotor(int startingValue);



private slots:
    void onDataAvailable();


};

#endif // PRESSURECONTROL_H
