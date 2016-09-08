#ifndef FRONTPANEL_H
#define FRONTPANEL_H

#include <QMainWindow>
#include <QMessageBox>


#include "ui_frontpanel.h"
#include "pressurecontrol.h"


namespace Ui {
class FrontPanel;
}

class PressureControl;

class FrontPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit FrontPanel(QWidget *parent = 0);
    ~FrontPanel();

private:
    Ui::FrontPanel *ui;


    // general control things
    QString portName;
    int portOpen;
    QString fileName;
    bool fileExists(QString path);

    // pressure things
    PressureControl *pc;
    double pressure;
    int motorPosition; // 1000 to 2000
    double measurePressureValue;
    double balancePressureValue;
    bool valveOpen;
    bool ventOpen;


    // camera things




    // ROI things


signals:
    void setValve(bool value);
    void setVent(bool value);
    void goToPressure(double desiredPressure, int flag);
    void setMotorPosition(int value); // value is percentage of max stroke
    void initMotor(int startingValue);


public slots:
    void arduinoOpenSlot();
    void arduinoClosedSlot();
    void listAvailablePorts(QList<QString> *portNamesList);

    void pressureUpdatedSlot(double pressureIn);
    void balanceFinished(int successful, int flag);
    void updateMotorPosition(int value);
    void motorInitialized();



private slots:
    void on_stopButton_clicked();
    void on_startButton_clicked();

    void on_valveButton_clicked();
    void on_ventButton_clicked();
    void on_horizontalSlider_sliderReleased();

    void on_initializeButton_clicked();
    void on_balanceButton_clicked();
    void on_measureButton_clicked();

    void on_balancePressureDouble_valueChanged(double arg1);
    void on_measurePressureDouble_valueChanged(double arg1);

    void on_fileNameButton_clicked();
    void on_fileNameBox_editingFinished();

    void on_outIncrement_clicked();
    void on_inIncrement_clicked();
    void on_arduinoConnectBtn_clicked();
};

#endif // FRONTPANEL_H
