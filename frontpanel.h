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
    int portOpen;
    QString fileName;
    bool fileExists(QString path);

    // pressure things
    PressureControl *pc;
    double pressure;
    double motorPosition;
    double measurePressureValue;
    double balancePressureValue;
    int arduinoStartable;
    int valveOpened;


    // camera things




    // ROI things


signals:
    void setValve(int value);
    void setVent(int value);
    void goToPressure(double desiredPressure, int flag);
    void setMotorPosition(int value); // value is percentage of max stroke


public slots:
    void arduinoOpenSlot();
    void arduinoClosedSlot();
    void arduinoReadySlot();

    void pressureUpdatedSlot();
    void balanceFinished(int successful, int flag);
    void updateMotorPosition(double value);



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
};

#endif // FRONTPANEL_H
