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
    int sensorVoltage;
    double pressureOffset;
    QByteArray serialData;
    void waitUntilMoveDone(double desiredPressure);


signals:
    void signalPortClosed();
    void signalPortOpen();
    void relayPressure();
    void balanceFinished(int successful, int flag);
    void updateMotorPosition(double value);
    void relayPortList(QList<QString> *);


public slots:
    void zeroPressure();
    void moveMotor(int value);
    void goToPressure(double desiredPressure, int flag);
    void setValve(int value);
    void setVent(int value);
    void getAvailablePorts();


private slots:
    void onDataAvailable();


};

#endif // PRESSURECONTROL_H
