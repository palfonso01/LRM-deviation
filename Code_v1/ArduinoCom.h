#ifndef ARDUINOCOM_H
#define ARDUINOCOM_H

#include <QtGui>
#include <iostream>
#include <fstream>
#include <time.h>
#include <abstractserial.h>
#include <serialdeviceenumerator.h>
#include <QList>
#include <QtDebug>
#include <ui_design1.h>

namespace Ui {
    class MainWindow;
}

using namespace std;

#define DELAY 10
#define RAD2DEG 180.0/M_PI
#define Dim 80

typedef struct
{
    int degree;
    int min;
    float sec;
    char orientation;
    char chain[20];
}GPS;


class ArduinoCom: public QWidget
{
    Q_OBJECT
public:
    ArduinoCom(QWidget *parent=0);
    ~ArduinoCom();
signals:
    void Reading(int e);
    void SendRPY(double r, double p, double y);
public slots:
    void Connection(void);
    void Stop(void);
    void ReadGPS(void);
    void SetPort(int index);
    void ButtonSave(void);
    void SaveR(void);
    void ArduinoEnumerate(const QStringList &lis);
private:
    void Conections(void);
    AbstractSerial *Arduino;
    QTimer *timer;
    time_t Tact;
    char *name;
    char *FileName;
    int flag;
    char *data;
    double roll, pitch, yaw;
    double LAT, LON, ANG;
    int X, Y, Z;
    GPS Lat, Lon;
    char *sample, *sampleang;
    QString PORT;
    SerialDeviceEnumerator *ports;
    QStringList l;
    Ui::MainWindow *ui;
    void ConvertRPY(int size);
    void ConvertChain();
    char *NameFile(void);
};

#endif // ARDUINOCOM_H
