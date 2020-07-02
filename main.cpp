#include <QCoreApplication>
#include <QDebug>
#include "memorymapfileipc.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "sender mode on." << endl;
    QString s = "_";
    //typedef types::MemoryMapFileBuffer<> memory_map_buffer;

    typedef types::MemoryMapFileBuffer<32,                      // размер буфера
            1,                                                  // значение "Прочитано"
            0,                                                  // значение "Не прочитано"
            types::NoBitReaded,                                 // стратегия "байт чтения"
            quint32,                                            // тип данных для хранения размера данных
            types::NoSizeOfData> memory_map_buffer;             // стратегия "размер данных"

    memory_map_buffer *m = NULL;
    QTextStream stin(stdin);
    while(s != ""){
        stin >> s;
        if(s == "create"){
            stin >> s;
            if(m != NULL){
                delete m;
            }
            m = new memory_map_buffer(s);
        } else if (s == "exit"){
            if(m != NULL){
                delete m;
            }
            break;
        } else if (s == "send"){
            QString data_to_send;
            stin >> data_to_send;
            if(m != NULL){
                QByteArray dat(data_to_send.toStdString().data(),data_to_send.size());
                (*m).sendData(dat.data(),dat.size());
            } else {
                qDebug() << "No buffer are created.";
            }
        } else if (s == "readed?"){
            bool readed = false;
            if(m != NULL){
                readed = (bool)(*m);
            }
            if(readed){
                qDebug() << "Data is readed.";
            } else {
                qDebug() << "Data is not readed.";
            }
        } else if(s == "datasize?"){
            if(m != NULL){
                qDebug() << (*m).dataSize();
            }else {
                qDebug() << "No buffer are created.";
            }
        } else if(s == "recive"){
            if(m!=NULL){
                QByteArray ba;
                ba.resize((*m).dataSize());
                (*m).reciveData(ba.data(),ba.size());
                QString s(ba);
                qDebug() << s;
            } else {
                qDebug() << "No buffer are created.";
            }

        }
    }
    qDebug() << "Program is done." << endl;
    return 0;
}
