#ifndef PRESSURECONTROL_H
#define PRESSURECONTROL_H

#include <QObject>
#include <QByteArray>
#include <QtSerialPort>

#define MOTOR_INIT_PCT 70
#define MAX_STROKE 50


class PressureControl : public QObject
{
    Q_OBJECT


public:
    explicit PressureControl(QObject *parent = 0);
    ~PressureControl();
    void closePort();
    void openPort();


private:
    QSerialPort *port;
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


public slots:
    void zeroPressure();
    void moveMotor(int value);
    void goToPressure(double desiredPressure, int flag);


private slots:
    void onDataAvailable();


};

#endif // PRESSURECONTROL_H
