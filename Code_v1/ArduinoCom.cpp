#include "ArduinoCom.h"

ArduinoCom::ArduinoCom(QWidget *parent):QWidget(parent)
{
    ui=new Ui::MainWindow;
    ui->setupUi(this);
    Tact=time(NULL);
    flag=0;
    roll=0;
    pitch=0;
    yaw=0;
    X=0;
    Y=0;
    Z=0;
    data=new char[Dim];
    timer=new QTimer;
    if (!this->ports)
        this->ports = SerialDeviceEnumerator::instance();
    l= this->ports->devicesAvailable();
    ArduinoEnumerate(l);
    Conections();
}
void ArduinoCom::Conections()
{
    ui->Button_Stop->setDisabled(true);
    ui->Button_Save->setDisabled(true);

    QObject::connect(ui->Button_Close, SIGNAL(clicked()), this, SLOT(close()));
    QObject::connect(ui->Button_Start, SIGNAL(clicked()), this, SLOT(Connection()));
    QObject::connect(ui->Button_Stop, SIGNAL(clicked()), this, SLOT(Stop()));

    QObject::connect(ui->Button_Save, SIGNAL(clicked()), this, SLOT(ButtonSave()));

    QObject::connect(this, SIGNAL(SendRPY(double,double,double)), ui->ogreWidget, SLOT(SetRPY(double,double,double)));
    QObject::connect(ui->ogreWidget, SIGNAL(SendDeg(int)), ui->dial_Peralte, SLOT(setValue(int)));
    QObject::connect(ui->ogreWidget, SIGNAL(SendFDeg(QString)), ui->lineEdit_valueP, SLOT(setText(QString)));
}

ArduinoCom::~ArduinoCom()
{
    delete Arduino;
    delete timer;
    delete []data;
    delete ui;
}

void ArduinoCom::ArduinoEnumerate(const QStringList &lis)
{
    // Fill ports box.
    ui->combo->setEnabled(true);
    ui->combo->clear();
    ui->combo->addItems(lis);
}

void ArduinoCom:: SetPort(int index)
{
    PORT=l.at(index);
}

void ArduinoCom::Connection()
{
    ui->combo->setEnabled(false);
    ui->Button_Stop->setEnabled(true);
    ui->Button_Save->setEnabled(true);
    Arduino= new AbstractSerial();
    Arduino->setDeviceName(ui->combo->currentText());
    Arduino->setBaudRate(AbstractSerial::BaudRate115200);
    Arduino->setDataBits(AbstractSerial::DataBits8);
    Arduino->setParity(AbstractSerial::ParityNone);
    Arduino->setStopBits(AbstractSerial::StopBits1);
    Arduino->setFlowControl(AbstractSerial::FlowControlOff);
    Arduino->open(QIODevice::ReadWrite);
    timer->start(1000);
        connect(timer,SIGNAL(timeout()),this,SLOT(ReadGPS()));
}

void ArduinoCom::Stop()
{
    ui->combo->setEnabled(true);
    ui->Button_Save->setEnabled(false);
    ui->Button_Stop->setEnabled(false);
    cout<<"Desconexion..."<<endl;
    Arduino->close();
}

void ArduinoCom::ReadGPS()
{
    if(!Arduino->isOpen())
    {
        QMessageBox::information(this, "Clausura de puerto", "Puerto COM3 apagado");
        Arduino->close();
        timer->disconnect(this,SLOT(ReadGPS()));
    }
    else
    {
            name=asctime(localtime(&Tact));
            Arduino->write("1",1);
            QByteArray result;
            int bytesAvailable = Arduino->bytesAvailable();
            result.resize(bytesAvailable);
            Arduino->read(result.data(), result.size());
            Arduino->flush();
            sample=result.data();
                ConvertChain();
                emit SendRPY(roll, pitch, yaw);
    }

}

/*void ArduinoCom::ConvertChain()
{
    char angc[50], CadT[50];
    char auxang[10];
    int i=0, j=0;
    double ang=0;

    i=6;
    while(sample[i]!='\0')
    {
        auxang[j]=sample[i];
        j++;
        i++;
    }
    ang=atof(auxang);
    sprintf(angc, "%lf\°\n",ang);

    cout<<"ANG: "<<angc<<endl;

    CadT[0]='\0';
    strcat(CadT,angc);

    for(int k=0;k<50;k++)
        data[k]=CadT[k];

    cout<<"CAD: "<<data<<endl;

    roll=ang;
    pitch=ang;
    yaw=ang;
}*/


void ArduinoCom::ConvertChain()
{
    char latc[50], lonc[50], angc[50], angx[50], CadT[80];
    char aux[10], aux2[10], auxang[10], auxangx[10];
    int i=0, j=0;
    double lat=0, lon=0 , ang=0, angux=0;

    if(sample[11]=='W' || sample[11]=='S')
    {
        latc[0]='\-';
    }
    else
    {
        latc[0]=' ';
    }
    if(sample[25]=='W' || sample[25]=='S')
    {
        lonc[0]='\-';
    }
    else
    {
        lonc[0]=' ';
    }
    ////// Latitud ////////////////////////////
    while(i<2)
    {
        latc[i+1]=sample[i];
        i++;
    }
    latc[i+1]='\°';
    while(i<5)
    {
        latc[i+2]=sample[i];
        i++;
    }
    latc[i+1]='\'';
    i--;
    while(i<10)
    {
        aux[i-4]=sample[i];
        i++;
    }
    lat=atof(aux);
    lat*=60.0;
    sprintf(aux, "%lf\",",lat);
    strcat(latc,aux);

    ///// Longitud ///////////////////////////
    j=0;
    i=i+3;
    while(i<16)
    {
        lonc[j+1]=sample[i];
        j++;
        i++;
    }
    lonc[j+1]='\°';
    while(i<19)
    {
        lonc[j+2]=sample[i];
        j++;
        i++;
    }
    lonc[j+1]='\'';
    lonc[j+2]='\0';
    i--;
    j=0;
    while(i<24)
    {
        aux2[j]=sample[i];
        i++;
        j++;
    }
    lon=atof(aux2);
    lon*=60.0;
    sprintf(aux2, "%lf\",",lon);
    strcat(lonc,aux2);

    ////// Angulo //////////
    i=i+5;
    j=0;
    while(i<34)
    {
        auxang[j]=sample[i];
        j++;
        i++;
    }
    ang=atof(auxang);
    sprintf(angc, "%lf\°,",ang);

    j=0;
    i++;
    while(i<39)
    {
        auxangx[j]=sample[i];
        j++;
        i++;
    }
    angux=atof(auxangx);
    sprintf(angx, "%lf\°\n",angux);

    cout<<"LAT: "<<latc<<endl;
    cout<<"LON: "<<lonc<<endl;
    cout<<"ANGX: "<<angc<<endl;
    cout<<"ANGZ: "<<angx<<endl;

    CadT[0]='\0';
    strcat(CadT,latc);
    strcat(CadT,lonc);
    strcat(CadT,angc);
    strcat(CadT,angx);
    strcat(CadT,"\0");

    for(int k=0;k<80;k++)
        data[k]=CadT[k];

    cout<<"CAD: "<<data<<endl;

    roll=ang;
    pitch=ang;
    yaw=ang;
}

void ArduinoCom::ButtonSave(void)
{
    FileName=NameFile();
    SaveR();
    flag=1;
    if(flag==1 && Arduino->isOpen())
        connect(timer,SIGNAL(timeout()),this,SLOT(SaveR()));
    else
        timer->disconnect(this, SLOT(SaveR()));
}

void ArduinoCom::SaveR(void)
{
    ofstream fileOut(FileName, ofstream::binary|ofstream::app);
    if(flag==0)
    {
        fileOut<<"LATITUD, LONGITUD, ANGULO"<<endl;
        fileOut.close();
    }
    else
    {
        fileOut<<data;
        fileOut.close();
    }
}

char *ArduinoCom::NameFile(void)
{
    int i,j,n,k;
    n=strlen(name);
    char *temp;
    temp=new char[25];
   for(j=0;j<(n-1);j++)
   {
      if(name[j]==':')
           name[j]='_';
      if(name[j]==32)
      {
          for(k=0;k<(n-j)-1;k++)
              name[j+k]=name[j+k+1];
      }
   }
   for(i=0;i<25;i++)
   {
       if(i==24)
           temp[i]='\0';
       else
        temp[i]=name[i];
       if(i==20)
       {
           temp[i++]='.';
           temp[i++]='c';
           temp[i++]='s';
           temp[i++]='v';
           temp[i++]='\0';
       }
   }
    return temp;
}
