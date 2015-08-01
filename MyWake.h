#ifndef MYWAKE_H
#define MYWAKE_H

#include <QtCore>
#include <QtCore/QTimer>
#include <QIODevice>

#include <string.h>
#include <stdlib.h>

template <class T>
class MyWake
{
public:
    MyWake(T *port);
    void setTxAddr(int address);        //установка адреса получателя

    int getAvgTimeout();
    int getMaxTimeout();
    int getTimeout(int index);

    QString getErrorString(int err);
    void waitLongTimeoutAndClearBuffer(int timeout);

protected:
    QTimer _timer;
    QTimer _timerTemp;
    QVector<int> _vectTime;
    T *_port;

    int _txAddr;           //передаваемый адрес устройства
    int _txCmd;            //передаваемая команда
    QString _txCmdName;    //имя передаваемой команды

    char _txData[256];     //массив данных для передачи
    char _rxData[256];     //массив данных для приема

    //передача пакета
    void txData(int cmdcode,
                QString cmdname,
                int datalength);

    //получение пакета
    int rxData(int timeout, int &err);

    //формирование и передача пакета в порт
    int txFrame(unsigned char addr,
                unsigned char cmd,
                unsigned char countTxData,
                const char *data);

    //получение и парсинг пакета из порта
    bool rxFrame(int timeout,
                 unsigned char &addr,
                 unsigned char &cmd,
                 unsigned char &countRxData,
                 char *data);

    //получение байта из порта
    bool rxByteCOM(char &b);
};



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//---------------------------Implementation---------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------


//---------------------------- Constants: -----------------------------------

//Стандартные коды команд протокола WAKE:
#define C_NOP  0    //no operation
#define C_ERR  1    //get error code
#define C_ECHO 2    //get echo
#define C_INFO 3    //get information

//Wake protocol errors:
#define ERR_NO   0    //no error
#define ERR_TX   1    //IO error
#define ERR_BU   2    //device busy error
#define ERR_RE   3    //device not ready
#define ERR_PA   4    //parameters value error
#define ERR_NR   5    //device not responding (response not received, timeout error, & etc.)
#define ERR_IR   6    //incorrect responce command (rxCmd != txCmd)
#define ERR_UK   7    //unknown error

#define MAX_ERROR_CODE ERR_UK           //максимальный номер ошибки
#define DEFAULT_TIMEOUT 300             //timeout для TX и RX по умолчанию, мс
#define DEFAULT_TIMEOUT_CLEAR_BUFF 300  //timeout для очистки буффера от старой информации, мс
#define DEFAULT_ADDRESS 1               //адрес получателя по умолчанию (0..127)
#define CRC_INIT 0xDE                   //CRC Initial value

const char
  FEND  = 0xC0,        // Frame END
  FESC  = 0xDB,        // Frame ESCape
  TFEND = 0xDC,        // Transposed Frame END
  TFESC = 0xDD;        // Transposed Frame ESCape

const QString Errors[] = {
    "",
    "ошибка отправки пакета.",
    "устройство занято.",
    "устройство не готово.",
    "неправильные параметры.",
    "устройство не отвечает.",
    "неверный ответ на команду."
};




///    Calculate CRC
///    \param b New byte to calculate
///    \param crc Current CRC
static void do_crc(unsigned char b, unsigned char *crc)
{
  for (int i = 0; i < 8; i++)
  {
   if (((b ^ *crc) & 1) != 0)
     *crc = ((*crc ^ 0x18) >> 1) | 0x80;
       else *crc = (*crc >> 1) & ~0x80;
   b = b >> 1;
   }
}

///    Do byte stuffing in buffer and update pointer, if needed
///    \param b Byte to tx
///    \param dptr Data pointer
///    \param buff Buffer
static void byte_stuff(char b, int &bptr, char *buff)
{
  if ((b == FEND) || (b == FESC))
  {
    buff[bptr++] = FESC;
    buff[bptr++] = (b == FEND)? TFEND : TFESC;
  }
  else buff[bptr++] = b;
}

template <class T>
MyWake<T>::MyWake(T *port)
{
    _port=port;
    _txAddr=DEFAULT_ADDRESS;            //передаваемый адрес устройства
    _txCmd=0;                           //передаваемая команда
    _txCmdName="";                      //имя передаваемой команды

//    _timer.setInterval(10000);
    _timer.setTimerType(Qt::PreciseTimer);

    for(int i=0;i<256;i++) _txData[i]=0;//массив данных для передачи и приема
    for(int i=0;i<256;i++) _rxData[i]=0;//массив данных для передачи и приема
}


///    Transmit frame in wake format
///    \param addr
///    \param cmd
///    \param countTxData
///    \param data
///    \retval  0: all fine
///    \retval -1: ftdidev_tx_buff failed
template <class T>
int MyWake<T>::txFrame(unsigned char addr, unsigned char cmd,
                    unsigned char countTxData, const char *data)
{
    char buff[518];                         // буфер для передачи
    int index = 0;                          // указатель буфера
    unsigned char crc = CRC_INIT;           // контрольная сумма

    // fixme: check input params
    buff[index++] = FEND;                   // передача FEND
    do_crc(FEND, &crc);
    if(addr)
    {
        byte_stuff((unsigned char)addr | 0x80, index, buff); // передача адреса
        do_crc(addr & 0x7F, &crc);
    }
    byte_stuff(cmd & 0x7F, index, buff);    // передача команды
    do_crc(cmd & 0x7F, &crc);
    byte_stuff(countTxData, index, buff);   // передача длины пакета
    do_crc(countTxData, &crc);
    for(int i = 0; i < countTxData; i++)
    {
        byte_stuff(data[i], index, buff);     // передача данных
        do_crc(data[i], &crc);
    }
    byte_stuff(crc, index, buff);           // передача CRC

//    qint64 n = _port->bytesAvailable();
//    char *dataR = new char[n];
//    qint64 readBytes = _port->read(dataR, n);
//    qDebug() << n << "\n"
//             << "Очищаем буфер приемника от старой информации"<< "\n"
//             << readBytes << "\n"
//             << _port->bytesAvailable();
//    delete[] dataR;
    _port->write(buff, index);              // write data in port

    return 0;
}

//-------------Ждем данные и очищаем буффер:---------------------------------
template <class T>
void MyWake<T>::waitLongTimeoutAndClearBuffer(int timeout)
{
    _timer.start(timeout);
    if(_timer.isActive() && _timer.remainingTime() > 0
            && _port->isOpen() && _port->isReadable())
    {
        while(_timer.remainingTime())
        {
            qApp->processEvents();    // Доставить накопившиеся события
            _port->readAll();
        }
    }
    _timer.stop();
}

//---------------------------------------------------------------------------
// 1 - если данные прочитаны
template <class T>
bool MyWake<T>::rxByteCOM(char &b)
{
    qApp->processEvents();    // Доставить накопившиеся события
    if(_timer.isActive() && _timer.remainingTime() > 0
            && _port->isOpen() && _port->isReadable() && _port->bytesAvailable())
    {
        _port->read(&b, 1);
//        qDebug() << QByteArray(1, b).toHex();
        return true;
    }
    else
        return false;
}

//--------------------------- Receive frame: --------------------------------

template <class T>
bool MyWake<T>::rxFrame(int timeout, unsigned char &addr, unsigned char &cmd,
                     unsigned char &countRxData, char *data)
{
    int i;
    char b = 0;
    unsigned char crc = CRC_INIT;             //init CRC

//    _port->waitForBytesWritten(100);
    //_port->waitForReadyRead(50);

    _timer.start(timeout);                    //set timeout
    int t2RT=_timer.remainingTime();
    // при отправке по bluetooth данные отправляются сразу (проверено опытным путем)
//    _timerTemp.start(2000);
//    int t1RT=_timerTemp.remainingTime();
//    qDebug() << "Отправляем данные в порт \n"
//             << "Осталось отправить байт" << _port->bytesToWrite() << "\n"
//             << "Осталось времени" << t1RT << "\n"
//             << "Запускаем цикл 1" << (_timerTemp.remainingTime() && _port->bytesToWrite());
//    qApp->processEvents();    // Доставить накопившиеся события
//    while(_timer.remainingTime() && _port->bytesToWrite())
//    {
//        _port->flush();
//        qApp->processEvents();    // Доставить накопившиеся события
//    }
//    t1RT-=_timerTemp.remainingTime();
//    qDebug() << "Прошло времени" << t1RT
//             << "Осталось отправить байт (должно быть 0)" << _port->bytesToWrite() << "\n";
//    qDebug() << "Конец цикла 1" << "\n"
//             << "Прошло времени" << t1RT - _timerTemp.remainingTime() << "\n"
//             << "Осталось времени" << _timerTemp.remainingTime() << "\n"
//             << "Осталось отправить байт (должно быть 0)" << _port->bytesToWrite() << "\n";
//    _timerTemp.stop();


//    qApp->processEvents();    // Доставить накопившиеся события
//    qDebug() << "Ждем данные из порта \n"
//             << "Пришло байт" << _port->bytesAvailable() << "\n"
//             << "Осталось времени" << t2RT << "\n"
//             << "Запускаем цикл 2";
    while(_timer.remainingTime() && !rxByteCOM(b))
    {
    }
//    qDebug() << "Конец цикла 2" << "\n"
//             << "Прошло времени" << t2RT - _timer.remainingTime() << "\n"
//             << "Осталось времени" << _timer.remainingTime() << "\n"
//             << "Пришло байт" << _port->bytesAvailable();

    if(_timer.remainingTime()<=0)
    {
        qDebug() << "Выходим с ошибкой. Таймаут при ожидании данных с порта";
        _vectTime.append(t2RT);
        return 0;
    }
    if(b != FEND)
    {
//        qDebug() << "Ждем стартовый байт";
        while(_timer.remainingTime() && b != FEND)
            rxByteCOM(b);
        if(_timer.remainingTime()<=0)
        {
            qDebug() << "Выходим с ошибкой. Таймаут при ожидании FEND с порта";
            _vectTime.append(t2RT);
            return 0;
        }
//        qDebug() << "Времени прошло" <<  t2RT -  _timer.remainingTime();
    }
    do_crc(b, &crc);                          //update CRC

//    qDebug() << "Начинаем разбор принятых данных";
    countRxData = 0; addr = 0;
    for(i = -3; i<=countRxData; i++)
    {
        while(_timer.remainingTime() && !rxByteCOM(b))
        {
        }
        if(_timer.remainingTime()<=0)
        {
            qDebug() << "Выходим с ошибкой. Таймаут при ожидании данных с порта. In While";
            break;                        //timeout error
        }
        if(b == FESC)
        {
            while(_timer.remainingTime() && !rxByteCOM(b))
            {
            }
            if(_timer.remainingTime()<=0)
            {
                qDebug() << "Выходим с ошибкой. Таймаут при ожидании данных с порта. After FESC";
                break;                    //timeout error
            }
            if(b == TFEND)
                b = FEND;                 //TFEND <- FEND
            else
                if(b == TFESC)
                    b = FESC;             //TFESC <- FESC
                else
                    break;
        }
        if(i == -3)
            if((b & 0x80) == 0) { cmd = b; i++; }         //CMD (b.7=0)
            else { b = b & 0x7F; addr = b; }              //ADD (b.7=1)
        else
            if(i == -2)
                if((b & 0x80) != 0) break;                //CMD error (b.7=1)
                else cmd = b;                             //CMD
            else
                if(i == -1)
                    countRxData = b;                      //countRxData
                else
                    if(i != countRxData) data[i] = b;     //data
        do_crc(b, &crc);                                  //update CRC
    }
//    qDebug() << "Конец разбора принятых данных";
//    qDebug() << "Времени прошло" <<  t2RT - _timer.remainingTime();
    _vectTime.append(t2RT - _timer.remainingTime());

    _timer.stop();                                       //stop timer

//    qDebug() << "i" << i
//             << "countRxData"  << countRxData
//             << "crc" << crc
//             << "return" << ((i==countRxData+1) && !crc);
    return((i==countRxData+1) && !crc);                   //RX or CRC error
}

//-------------------- Установка адреса получателя: ---------------------------

template <class T>
void MyWake<T>::setTxAddr(int address)
{
    if((address < 0) || (address > 127))
        _txAddr = 0;
    else
        _txAddr = address;
}

//------------------------- Передача пакета: --------------------------------

template <class T>
void MyWake<T>::txData(int cmdcode, QString cmdname, int datalength)
{
    if(_port->isOpen()) //если порт открыт
    {
        _txCmd = cmdcode;     //запоминаем код передаваемой команды
        _txCmdName = cmdname; //запоминаем имя передаваемой команды
        txFrame((unsigned char)_txAddr, //передача фрейма
                (unsigned char)_txCmd,
                (unsigned char)datalength,
                _txData); //FTxData - передаваемые данные
    }
}

//--------------------------- Прием пакета: ---------------------------------

template <class T>
int MyWake<T>::rxData(int timeout, int &err)
{
    unsigned char rxAdd = 0, rxCmd = 0, rxN = 0;
    err=ERR_NO;
    bool rx;

    if(_port->isOpen()) //если порт открыт
    {
        rx = rxFrame(timeout, //прием фрейма
                     rxAdd,
                     rxCmd,
                     rxN,
                     _rxData); //FRxData - принятые данные

        if(!rx)
        {
            err=ERR_NR;
            return -1;
        }

        if(rxCmd == C_ERR)
            err = _rxData[0];
        else
            if(rxCmd != _txCmd)
                err=ERR_IR;

        if(err > MAX_ERROR_CODE)
            err = ERR_TX;

        if(err)
            return -1;
    }
    else
    {
        err=ERR_TX;
        return -1;
    }
    return((int)rxN); //возвращаем количество принятых байт
}

//--------------- Получение информации об ошибке: --------------------------
template <class T>
QString MyWake<T>::getErrorString(int err)
{
    if (err>0 && err<=MAX_ERROR_CODE)
        return QString("Ошибка команды %1: %2")
            .arg(_txCmdName)
            .arg(Errors[err]);
    else
        return QString("Ошибка команды %1: %2")
            .arg(_txCmdName)
            .arg("не опознанная ошибка");
}


template <class T>
int MyWake<T>::getAvgTimeout()
{
    int sum=0;
    for(int i=0; i<_vectTime.size(); i++)
    {
        sum+=_vectTime.at(i);
    }
    return sum/_vectTime.size();
}

template <class T>
int MyWake<T>::getMaxTimeout()
{
    int max=0;
    for(int i=0; i<_vectTime.size(); i++)
    {
        if(max<_vectTime.at(i))
            max=_vectTime.at(i);
    }
    return max;
}

template <class T>
int MyWake<T>::getTimeout(int index)
{
    return _vectTime.at(index);
}

#endif // MYWAKE_H
