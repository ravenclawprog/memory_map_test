#ifndef MEMORYMAPFILEIPC_H
#define MEMORYMAPFILEIPC_H
#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QString>
#include <QDebug>
#include <inttypes.h>

namespace types {
// стратегия "байт чтения"
// =========================
template <uint8_t Readed = 1,   // значение "Прочитано"
          uint8_t Unreaded = 0> // значение "Не прочитано"
struct NoBitReaded{
    int getSize(){return 0;}
    void setReaded(uchar*){}
    void setUnreaded(uchar*){}
    bool isReaded(uchar*){ return false;}
    operator bool(){
        return false;
    }
};
template <uint8_t Readed = 1,
          uint8_t Unreaded = 0>
struct BitReaded{
    int getSize(){return 1;}                    // размер в байтах байта чтения чтения
    void setReaded(uchar* pos_){                // устанавливает значение "Прочитано"
        (*pos_) = static_cast<uchar>(Readed);
    }
    void setUnreaded(uchar* pos_){              // устанавливает значение "Не прочитано"
        (*pos_) = static_cast<uchar>(Unreaded);
    }
    bool isReaded(uchar* pos_){                 // возвращает значение "Истина", если данные в значение "Прочитано" и "Ложь" в противном случае
        return (*pos_) == static_cast<uchar>(Readed);
    }
    operator bool(){                            // определяет, поддерживает ли класс-стратегия "байт чтения"
        return true;
    }
};
// стратегия "размер данных"
// =======================
template <class T = quint64>
struct SizeOfData{
private:
    T data_size_;
public:
    SizeOfData(){
        data_size_ = 0;
    }
    int getSize(){return sizeof (T);}
    T getDataSize(uchar* pos_){
        memcpy(&data_size_,pos_,sizeof (T));
        return data_size_;
    }
    void setDataSize(T new_size, uchar* pos_){
        data_size_ = new_size;
        memcpy(pos_,&data_size_,sizeof (T));
    }
    operator bool(){                            // определяет, поддерживает ли класс-стратегия "размер данных"
        return true;
    }
};
template <class T = quint64>
struct NoSizeOfData{
    NoSizeOfData(){}
    int getSize(){return 0;}
    T getDataSize(uchar*){
        return 0;
    }
    void setDataSize(T, uchar*){}
    operator bool(){                            // определяет, поддерживает ли класс-стратегия "размер данных"
        return false;
    }
};
// Основной класс - буфер в виде отображения файла в памяти
// =======================
template <quint64 N = 1024,                                             // размер буфера
          uint8_t a = 1,                                                // значение "Прочитано"
          uint8_t b = 0,                                                // значение "Не прочитано"
          template <uint8_t, uint8_t>  class Reader = BitReaded,        // стратегия "байт чтения"
          class DataSizeType = quint64,                                 // тип данных для хранения размера данных
          template <class> class DataSize = SizeOfData>                 // стратегия "байт чтения
class MemoryMapFileBuffer{
    QFile   f_;                                                         // файл
    QString file_name_;                                                 // имя файла
    uchar   *memory_;                                                   // буфер памяти
    quint64 n_;                                                         // размер буфера в байтах
    bool    opened_;                                                    // флаг - открыт ли файл
    void resize_(quint64 new_size);                                     // изменяет размер открытого файла
    void openfile_(QIODevice::OpenModeFlag fm_);                        // открывает файл и управляет значением переменной opened
    void closefile_();                                                  // закрывает файл и управляет значением переменной opened
    void clear_();                                                      // зануляет значение файла и устанавливает бит-байт в значение "Не прочитано"
    void hard_clear_();                                                 // изменяет размер файла, делая его нулевого размера
    Reader<a,b> bit_read_;                                              // стратегия "байта чтения"
    DataSize<DataSizeType> data_size_;                                  // стратегия "размер данных"
public:

     MemoryMapFileBuffer(QString fileName);
    ~MemoryMapFileBuffer();

    bool sendData(char *data, DataSizeType maxlen);
    bool reciveData(char *data, DataSizeType maxlen);
    bool bind();
    operator bool();
    DataSizeType dataSize();
};
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::MemoryMapFileBuffer(QString fileName):
   f_(fileName),
   file_name_(fileName),
   memory_(NULL),
   n_(0),
   opened_(false)
{
   if(QFile::exists(file_name_)){
       openfile_(QIODevice::ReadWrite);
       if(opened_){
           n_ = f_.size();
           memory_ = f_.map(0,n_);
       }
   } else {
       openfile_(QIODevice::ReadWrite);
       if(opened_){
           resize_(N);
       }
   }
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
void MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::openfile_(QIODevice::OpenModeFlag fm_)
{
    if(!opened_){
        f_.open(fm_);
        opened_ = true;
    }
    if(f_.error() != 0){
        f_.close();
        opened_ = false;
    }
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
void MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::closefile_()
{
    if(memory_){
        f_.unmap(memory_);
    }
    if(opened_){
        f_.close();
    }
    opened_ = false;
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
void MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::clear_()
{
    for(quint64 i = 0; i < n_;i++){
        memory_[i] = 0;
    }
    bit_read_.setUnreaded(&(memory_[0]));
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
void MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::hard_clear_()
{
    resize_(0);
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::~MemoryMapFileBuffer()
{
    closefile_();
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
void MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::resize_(quint64 new_size)
{
    closefile_();

    openfile_(QIODevice::ReadWrite);
    // если возникла ошибка во время открытия файла
    if(!opened_){
        memory_ = NULL;
        n_ = 0;
    } else {
    // отображение памяти
        n_ = new_size + bit_read_.getSize() + data_size_.getSize();
        f_.resize(n_);
        memory_ = f_.map(0,n_);
        bit_read_.setUnreaded(&(memory_[0]));
    }

}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
bool MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::sendData(char *data, DataSizeType maxlen)
{
    if(memory_){
        bit_read_.setUnreaded(&(memory_[0]));
        data_size_.setDataSize(maxlen,(memory_ + bit_read_.getSize()));
        for(quint64 i = 0; i < maxlen; i++){
            memory_[i + bit_read_.getSize() + data_size_.getSize()] = data[i];
        }
        return true;
    } else {
        return false;
    }
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
bool MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::reciveData(char *data, DataSizeType maxlen)
{
    if(memory_){
        QByteArray ba((char*)(memory_),bit_read_.getSize() + data_size_.getSize() + maxlen);
        QDataStream d(&ba,QIODevice::ReadWrite);
        quint8  readed = 0;
        DataSizeType data_size = 0;
        d.device()->seek(0);
        if(bit_read_){
            d >> readed;
        }
        if(data_size_){
            d >> data_size;
        } else {
            data_size = maxlen;
        }
        if(maxlen > data_size){
            maxlen = data_size;
        }
        for(DataSizeType i = 0; i < maxlen; i++){
            quint8 data_;
            d >> data_;
            data[i] = data_;
        }
        d.device()->seek(0);
        if(!bit_read_.isReaded(&(memory_[0]))){
            bit_read_.setReaded(&(memory_[0]));
        }
        return true;
    } else {
        return false;
    }
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
bool MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::bind()
{
    closefile_();
    if(f_.exists()){
        openfile_(QIODevice::ReadWrite);
        n_ = f_.size();
        memory_ = f_.map(0,n_);
        return true;
    } else {
        return false;
    }
}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
DataSizeType MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::dataSize()
{
    DataSizeType result = 0;
    if(memory_){
        if(data_size_){
            result = data_size_.getDataSize(memory_ + bit_read_.getSize());
        }else {
            result = f_.size() - bit_read_.getSize();
        }
    }
    return  result;

}
template <quint64 N,
          uint8_t a,
          uint8_t b,
          template <uint8_t, uint8_t>  class Reader,
          class DataSizeType,
          template <class> class DataSize>
MemoryMapFileBuffer<N,a,b,Reader,DataSizeType,DataSize>::operator bool()
{
    if(memory_){
        return bit_read_.isReaded(&(memory_[0]));
    }else {
        return false;
    }
}

}
#endif // MEMORYMAPFILEIPC_H
