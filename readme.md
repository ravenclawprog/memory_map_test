# 0. О программе

  memory_map_test - тестовая программа для настольных ПЭВМ, которая показывает пример межпроцессного взаимодействия двух приложений через отображаемый в памяти файл.

# 1. Структура репозитория

    \ - корневой каталог;  

# 2. Системные требования
   
	* ОС Windows 7 или выше;  
	* ОС Linux (версия ядра 2.4.2 или выше);  
	* Qt v.4.8.1 или выше;  
	* gcc v.3.3.6 или выше (или аналогичный компилятор); 


# 3. Сборка

## 3.1 Сборка при помощи утилиты Qt Creator:  

    1) Открыть файл "memory_map_test.pro" в программе Qt Creator.  
    2) Выбрать комплект сборки, настроенный в вашем Qt.  
    3) Нажать сочетание клавиш Ctrl+B (или выбрать в верхнем меню раздел "Сборка"->"Собрать проект "memory_map_test").  
    4) Проект будет собран в папке, которая была настроена в п.2.  
   

## 3.2 Сборка из командной строки:  

    1) Перейти в корневой каталог проекта ("memory_map_test").  
    2) Выполнить команду "qmake memory_map_test.pro".  
    3) Выполнить команду "make".  
    4) Проект будет собран в папке с проектом. Чтобы собрать проект в другом каталоге необходимо откорректировать файл memory_map_test.pro, добавив в него поле "DESTDIR= < каталог для сборки > ".  
   


# 4. Настройка программы

    1) Размер вновь создаваемого файла (указывается первым параметром в типе MemoryMapFileBuffer). Указывается в байтах.  
    Возможные значения: от 1 до  18 446 744 073 709 551 615 (последнее соответсвует почти 16 эксабайт). Если данный параметр не задан, то в классе по умолчанию ему присваивается значение 1024.  
    В программе memory_map_test данный параметр соответсвует 32.  
    Если отправить в отображаемый файл в памяти посылку, размер котороый будет превышать 32 байта, то передадутся только первые 32 байта данной посылки, а оставшиеся - пропадут.  
    2) Стратегия "Байт чтения". При обмене бывает полезно процессам сообщать о том, что данные в буфере были перезаписаны или считаны.  
    Для этого в программе четвертый параметр в классе MemoryMapFileBuffer задает класс стратегии "байта чтения", а два предыдущих параметра - значения "прочитано" и "не прочитано".  
    Имеется две стратегии - NoBitReaded и BitReaded. Сратегия NoBitReaded не оказывает никакого влияния на буфер. Стратегия BitReaded резервирует первый байт в буфере под значение "прочитано"/"не прочитано".  
    Данные значения можно выставлять самостоятельно. Также при вызове методов sendData() значение байта устанавливается в "не прочитано", а при вызове метода reciveData() значение первого байта устанавливается в "прочитано".  
    Значения "Прочитано" и "Не прочитано" устанавливаются в качестве параметров в классе MemoryMapFileBuffer вторым и третьим соответственно.  
    3) Стратегия "Размер данных". Также при обмене данными бывает полезно указать количество пересылаемых байт в буфере.  
    Для этого в программе шестой параметр в классе MemoryMapFileBuffer задает класс стратегии "Размер данных". В качетве пятого параметра указывается тип данных для хранения размера файла. Данное поле будет располагаться после байта чтения, если такой предусмотрен стратегией "Байт чтения".  
    При записи в данное поле в файл записывает размер пересылаемых данных. При считывании данное поле указывает на то, сколько байт необходимо считать из буфера.  
    Имеется две стратегии - NoSizeOfData и SizeOfData. Первая стратегия не создает поле в отображаемом в файле буфере. Вторая стратегия создает поле в отображаемом в памяти файле размером, который имеет размер тип данных, указанный пятым параметром.  
    Данное поле заполняется автоматически при вызове метода sendData() и считывается автоматически методом reciveData().  
   
# 5. Инструкция по управлению

   Программа позволяет подключиться к буферу или создать его. Отправка данных в буфер осуществляется при помощи соответствующей команды в консоли. Данные, считанные из буфера, отображаются в консоли.  
   Переда началом отправки и приема данных необходимо создать буффер или подключиться к существующему при помощи команды create.  
   Затем можно отправлять и принимать данные при помощи команд send и recive.  
   В конце приема и передачи необходимо вызвать команду exit.
   Программа имеет следующие команды:  
   - create;  
   - send;  
   - recive;  
   - readed?;  
   - datasize?;
   - exit.  
   
## 5.1 Команда "create"  

   Перед началом работы необходимо создать буфер или подключиться к существующему. Для этого необходимо написать команду create и путь до файла без пробелов.  
   Если такой файл существует, то произойдет подключение и размер буфера будет соответствовать размеру файла.  
   Если же такого файла не существует, то такой файл будет создан с размером буфера, указанного первым параметром в классе MemoryMapFileBuffer(32 по умолчанию в данной программе).  
   
## 5.2 Команда "send"  

   После подключения к файлу, можно отправить команду send и через пробел данные для отправки.  
   Символом окончания последовательности данных является пробел.  
   Стоит отметить, что данные в буфер записываются с самого начала, перезаписывая только те данные, которые соответствуют данному размеру.Т.е. если в буфере содержатся 20 байт данных и после отправки в буфер 15 байт данных, то хранившиеся ранее в буфере 20 байт данных будут перезаписаны не полностью, а лишь 15 байт, сохраняя при этом оставшиеся 5 байт неизменными, но недоступными для чтения.  
   Также, если используется стратегия NoSizeOfData, то чтение будет происходить от начала области данных до конца файла.  
   

## 5.3 Команда "recive"  

   После подключения к файлу, можно использовать команду recive. Команда не требует никаких параметров.  
   Если используется стратегия SizeOfData, то будут получены данные, укладывающиеся в данный размер.  
   Если используется стратегия NoSizeOfData, то будут получены как сами данные, так и остатки, которые были в буфере.  
   

## 5.4 Команда "readed?" 

   Команда возвращает "Data is readed", если значение в "байте чтения" соответсвует значения "Прочитано" и возвращает значение "Data is not readed", если значение не равно "Прочитано".  
   Если используется стратегия NoBitReaded, то результат команды всегда возвращает "Data is not readed".  
   

## 5.5 Команда "datasize?"  

   Команда возвращает размер полученных данных в байтах.  
   Если используется стратегия NoSizeOfData, то программа напечате размер файла в байтах.  
   
## 5.5 Команда "exit"  

   Если получена команда "exit", то программа завершит свою работу.  
   Данные из буфера при этом не очищаются.  
