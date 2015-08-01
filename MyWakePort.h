#ifndef MYWAKEPORT_H
#define MYWAKEPORT_H

#include "MyWake.h"
#include "../Defines.h"

template <class T>
class MyWakePort : public MyWake<T>
{
public:
    /** \brief Конструктор
     */
    MyWakePort(T *port):
        MyWake<T>(port)
    {
    }

    /** \brief Получение информации об устройстве
     *  \param[out] err     Возникшая ошибка при передаче
     */
    QString getInfo(int &err);

    /** \brief Отправка echo пакета
     *  \param[in]  data    Указатель на передаваемые данные
     *  \param[in]  size    Размер данных
     *  \param[out] err     Возникшая ошибка при передаче
     *  \return Полученные данные в ответ на пакет
     */
    QByteArray sendEcho(const char *data, const int size, int &err);

    /** \brief Тестирует линию (отправляет echo пакет с заданным таймаутом)
     *  \param[in]  data    Указатель на передаваемые данные
     *  \param[in]  size    Размер данных
     *  \param[in]  timeout Максимальное время на получение ответа
     *  \param[out] err     Возникшая ошибка при передаче
     *  \return Время между отправкой и получением ответа, мс
     */
    int testLine(const char *data, const int size, const int timeout, int &err);

    /** \brief Обработчик отправки сообщения (перед записью данных в сокета)
     *  \param[in]  el      Передаваемый элемент
     *  \param[in]  mes     Передаваемое сообщение от элемента
     */
    void sendMessage(const Element el, const quint8 mes);

    /** \brief Получение сообщения о состоянии заданного элемента КМУ
     *         Прежде должен быть отправлен запрос состояния
     *  \param[in]   el      Элемент, что состояние необходимо узнать
     *  \param[out]  mes     Его новое состояние
     */
    void getResponceStatus(const Element el, quint8 &mes);

protected:
    QMap<Element, quint8> _mapMessage;
};



//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
//---------------------------Implementation---------------------------------
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------



//------------ Получение сообщения о состоянии заданного элемента КМУ: -----------
template <class T>
void MyWakePort<T>::getResponceStatus(const Element el, quint8 &mes)
{
    mes = _mapMessage.value(el, mes);
}


//--------------- Отправка пакета запроса информации: --------------------------
template <class T>
QString MyWakePort<T>::getInfo(int &err)
{
    MyWake<T>::txData(C_INFO, "GetInfo", 0);
//    int n = -1; err=1;
    int n = MyWake<T>::rxData(DEFAULT_TIMEOUT,err);
    if(n>=0)
        return QString::fromLocal8Bit(MyWake<T>::_rxData, n);
    else
        return MyWake<T>::getErrorString(err);
}

//---------------------- Отправка эхо-пакета: --------------------------------
template <class T>
QByteArray MyWakePort<T>::sendEcho(const char *data, const int size, int &err)
{
    memcpy((char *)MyWake<T>::_txData, data, size);
    MyWake<T>::txData(C_ECHO, "Echo", size);
    int n = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
    if(n>=0)
        return QByteArray(MyWake<T>::_rxData, n);
    else
        return MyWake<T>::getErrorString(err).toLocal8Bit();
}

//--------------- Проверка соединения (линии связи): ---------------------------
template <class T>
int MyWakePort<T>::testLine(const char *data, const int size, const int timeout, int &err)
{
    memcpy((char *)MyWake<T>::_txData, data, size);
    MyWake<T>::txData(C_ECHO, "TestLine", size);
    int n = MyWake<T>::rxData(timeout, err);
    if(n>=0)
        return MyWake<T>::getTimeout(0);
    else
        return err;
}

//------------------ Обработчик отправки сообщения: ---------------------------
template <class T>
void MyWakePort<T>::sendMessage(const Element el, const quint8 mes)
{
    if(!el)
        return;
    MyWake<T>::_txData[0]=(char)mes;
    QString cmdname=Converting::convertElementToString(el);
    MyWake<T>::txData((int)el, cmdname, 1);
    int err=0;
    int n=0;
#if SEND_REPLY
    n = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
    if(n>=0)
    {
        if(!err)
        {
            if(el==powerButton||el==lightButton
                    ||el==powerStatus||el==lightStatus
                    ||el==highTemperature||el==hookWarning
                    ||el==modeCrOrP||el==modeCrOrPStatus)
            {
                 _mapMessage[ (Element)MyWake<T>::_txCmd ] = (quint8)MyWake<T>::_rxData[0];
            }
        }
    }
    else
    {
        qDebug() << "1!: " << MyWake<T>::getErrorString(err);
        if(err==ERR_NR)
        {
            int n2 = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
            if(n2>=0)
            {
                if(!err)
                {
                    if(el==powerButton||el==lightButton
                            ||el==powerStatus||el==lightStatus
                            ||el==highTemperature||el==hookWarning
                            ||el==modeCrOrP||el==modeCrOrPStatus)
                    {
                         _mapMessage[ (Element)MyWake<T>::_txCmd ]=(quint8)MyWake<T>::_rxData[0];
                    }
                }
            }
            else
            {
                qDebug() << "2!!: " << MyWake<T>::getErrorString(err);
            }
        }
        else if(err==ERR_IR)
        {
//            MyWake<T>::waitLongTimeoutAndClearBuffer(DEFAULT_TIMEOUT_CLEAR_BUFF);
            int n2 = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
            if(n2>=0)
            {
                if(!err)
                {
                    if(el==powerButton||el==lightButton
                            ||el==powerStatus||el==lightStatus
                            ||el==highTemperature||el==hookWarning
                            ||el==modeCrOrP||el==modeCrOrPStatus)
                    {
                         _mapMessage[ (Element)MyWake<T>::_txCmd ]=(quint8)MyWake<T>::_rxData[0];
                    }
                }
            }
            else
            {
                qDebug() << "2!!: " << MyWake<T>::getErrorString(err);
            }
        }
    }
#else
    if(el==powerButton||el==lightButton
            ||el==powerStatus||el==lightStatus
            ||el==highTemperature||el==hookWarning
            ||el==modeCrOrP||el==modeCrOrPStatus)
    {
//        qDebug() << Converting::convertElementToString(el) << mes;
        n = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
        if(n>=0)
        {
            if(!err)
            {
                if(el==powerButton||el==lightButton
                        ||el==powerStatus||el==lightStatus
                        ||el==highTemperature||el==hookWarning
                        ||el==modeCrOrP||el==modeCrOrPStatus)
                {
                     _mapMessage[ (Element)MyWake<T>::_txCmd ] = (quint8)MyWake<T>::_rxData[0];
                }
            }
        }
        else
        {
#if SHOW_DEBUG
            qDebug() << "1!: " << MyWake<T>::getErrorString(err);
#endif
            if(err==ERR_NR)
            {
                int n2 = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
                if(n2>=0)
                {
                    if(!err)
                    {
                        if(el==powerButton||el==lightButton
                                ||el==powerStatus||el==lightStatus
                                ||el==highTemperature||el==hookWarning
                                ||el==modeCrOrP||el==modeCrOrPStatus)
                        {
                             _mapMessage[ (Element)MyWake<T>::_txCmd ]=(quint8)MyWake<T>::_rxData[0];
                        }
                    }
                }
                else
                {
#if SHOW_DEBUG
                    qDebug() << "2!!: " << MyWake<T>::getErrorString(err);
#endif
                }
            }
            else if(err==ERR_IR)
            {
    //            MyWake<T>::waitLongTimeoutAndClearBuffer(DEFAULT_TIMEOUT_CLEAR_BUFF);
                int n2 = MyWake<T>::rxData(DEFAULT_TIMEOUT, err);
                if(n2>=0)
                {
                    if(!err)
                    {
                        if(el==powerButton||el==lightButton
                                ||el==powerStatus||el==lightStatus
                                ||el==highTemperature||el==hookWarning
                                ||el==modeCrOrP||el==modeCrOrPStatus)
                        {
                             _mapMessage[ (Element)MyWake<T>::_txCmd ]=(quint8)MyWake<T>::_rxData[0];
                        }
                    }
                }
                else
                {
#if SHOW_DEBUG
                    qDebug() << "2!!: " << MyWake<T>::getErrorString(err);
#endif
                }
            }
        }
    }
#endif
}

#endif // MYWAKEPORT_H
