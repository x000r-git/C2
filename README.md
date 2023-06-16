# C2

Данный проект представляет собой шаблон для разработки полноценной C2-архитектуры.

Инфраструктура управления и контроля (C2), представляет собой набор инструментов и методов, которые злоумышленники используют для обмена данными со скомпрометированными устройствами после первоначального вторжения.

Поскольку разработка C2 - сложный и долгий процесс, требующий работы не одного профессионального разработчика на протяжении большого времени, было принято решение создать шаблон C2, который бы предоставил:
- Практически готовые бинарные файлы `server, client, database, admin`
- Возможность задавать функционал взаимодействия (разработчик сам выбирает, какие команды может выполнять клиент)
- Продвинутую технику закрепления на системе

Одним из вариантов реализации данной идеи и является данный проект.

## Server

Серверная часть выполняет следующие функции:
- Принимает подключения и сообщения от клиентов
- Может взаимодействовать с базой данных для хранения значений
- Обрабатывает пользовательский ввод

## Client

Клиентская часть состоит из двух частей: самого клиента и инжектора
- Инжектор запускается первым, прописываясь в автозапуск и, используя Process Hollowing, создает `svhost.exe`
- Клиент отправлять запросы на сервер для получения команд

## Database

База данных позволяет синхронизировать данные между администратором и сервером:
- Имеет три таблицы
- Позволяет вставлять, обновлять и выбирать данные

## Admin

Программа администратора:
- Позволяет задавать команды для удаленных клиентов
- Позволяет смотреть статистику по базе данных (состояние таблиц)


## Предпочтительный протокол общения
```
client          server          database        admin
HELLO_mes   ->  process    ->    INSERT_CL  -> SELECT
                process    <-    INSERT_Com  <- INSERT_Com
HELLO_mes   -> process
process     <- Command
OUTPUT      -> process      ->   INSERT_output -> SELECT
```