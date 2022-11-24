# C++ programmer

## Test task

### Description

Develop a C++ TCP proxy server for Postgresql RDBMS with the ability to log all SQL queries passing through it. Documentation on the network protocol of this RDBMS is available on the official website.

To run the test task, you need to use Berkeley sockets (select/poll/epoll).

There shouldn't be any other dependencies.

The proxy must be able to handle a large number of connections without creating a thread for each connection. It is necessary to parse network packets passing through the proxy, which contain SQL queries, extract these queries from the packets and write them to a file as text (one query per line, the structure is unimportant). In order for unencrypted SQL queries to be visible in the proxy, it is necessary to disable SSL (on the client and/or server). There should be minimal error handling, and comments are welcome where errors are possible. The application should not crash on several dozen simultaneous connections that execute queries to the DBMS without interruption for 5 minutes (you can use sysbench for testing). The operating system is Linux, the compiler is GCC, it is also necessary to create a file to build the project using cmake or make.

The purpose of the test task is to test the professional skills of candidates for a vacant position. The code written by you will not be used in the company's products or transferred to third parties. The result of the test task can be posted on github.com under any license in order to avoid using your code.

---

## Тестовое задание

### Описание

Разработать на C++ TCP прокси-сервер для СУБД Postgresql с возможностью логирования всех SQL запросов, проходящих через него. Документация по сетевому протоколу этой СУБД доступна на официальном сайте.

Для выполнения тестового задания нужно использовать Berkley sockets (select/poll/epoll).

Прочих зависимостей быть не должно.

Прокси должен уметь обрабатывать большое количество соединений без создания потока (thread) на каждое соединение. Необходимо распарсить сетевые пакеты, проходящие через прокси, в которых содержатся SQL запросы, извлечь эти запросы из пакетов и записать их в файл в виде текста (по одному запросу в строке, структура неважна). Для того, чтобы в прокси были видны SQL запросы в незашифрованном виде, необходимо отключить SSL (на клиенте и/или сервере). Должна присутствовать минимальная обработка ошибок, так же желательны комментарии в тех местах, где возможны ошибки. Приложение не должно падать на нескольких десятках одновременных соединений, выполняющих запросы к СУБД без перерыва в течение 5 минут (можно использовать sysbench для тестирования). Операционная система Linux, компилятор – GCC, так же необходимо создать файл для сборки проекта с помощью cmake или make.

Цель выполнения тестового задания – проверка профессиональных навыков кандидатов на вакантную позицию. Написанный Вами код не будет использоваться в продуктах компании или передан третьим лицам. Результат выполнения тестового задания можно выложить на github.com под любой лицензией во избежание использования Вашего кода.

---

### Run code with use docker compose

``docker compose up``
