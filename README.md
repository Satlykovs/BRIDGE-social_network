![bridge logo](https://github.com/user-attachments/assets/35c840df-183b-4eb0-b26b-8e4cca8ff8da)
# BRIDGE

Социальная сеть с чистым, минималистичным интерфейсом клиентского приложения и серверной частью, написанной на микросервисной архитектуре.

---


## 🛠️ Tech Stack

| Категория          | Технологии                                                                                     |
|--------------------|------------------------------------------------------------------------------------------------|
| **Backend**        | [![C++](https://img.shields.io/badge/C++-blue?logo=cplusplus)](https://isocpp.org/) + [![userver](https://img.shields.io/badge/userver-Framework-orange)](https://userver.tech/) |
| **Frontend**       | [![C++](https://img.shields.io/badge/C++-blue?logo=cplusplus)](https://isocpp.org/) + [![Qt](https://img.shields.io/badge/Qt-6.2+-green?logo=qt)](https://www.qt.io/) |
| **Database**       | [![PostgreSQL](https://img.shields.io/badge/PostgreSQL-15+-blue?logo=postgresql)](https://www.postgresql.org/) |
| **Storage**        | [![S3](https://img.shields.io/badge/Yandex_S3-Cloud-yellow?logo=amazon-s3)](https://cloud.yandex.ru/services/storage) |
| **Containerization** | [![Docker](https://img.shields.io/badge/Docker-20.10+-blue?logo=docker)](https://www.docker.com/) |

## 🔧 Installation

```bash
  git clone https://github.com/Satlykovs/BRIDGE-social_network.git
  cd BRIDGE-social_network
```

## ⚙️ Server Configuration

#### Общая настройка сервисов
Для большинства сервисов требуется:
1. Создать файл конфигурации в папке `configs` сервиса:
   ```yaml
   server-port: 8080
   secdist-path: ./secdist.json
   access-key: (your access key for s3 storage)
   secret-key: (your secret key for s3 storage)
   j-secret-key: (your jwt-secret)
   worker-threads: 8
   worker-fs-threads: 4
   logger-level: warning
   ```
	``Note: j-secret-key во всех сервисах должен быть одинаковым ``

2. А также в корне сервиса создать файл ``secdist.json``. Пример:
	```json
	{
		"postgresql_settings": {
		  "databases": {
			"post-db": [
			{
				"shard_number": 0,
				"hosts": [
				"host=post-db dbname=postdatabase user=user password=password port=5432"
						]
					}
				]
			}
		}
	}
	```
	``Note: Точные параметры для каждого сервиса смотрите в соответствующих переменных окружения баз данных в docker-compose.yml``
	
Исключение составляют сервис ``AuthService``: для него не нужны ``access-key`` и ``secret-key`` (не используется s3 хранилище); а также сервисы ``ChatService`` и ``FriendshipService``: в них не нужно создавать файл ``secdist.json``, но в файле ``config_vars.yaml`` стоит добавить поле:  ``db-path: "postgresql://user:password@chat-db:5432/chatdatabase"`` (Опять же, точные параметры подключения смотрите в ``docker-compose.yml``)



## 🚀 Running Server
Чтобы запустить серверную часть потребуются установленные ``Docker`` и ``docker-compose``. 
```bash
cd Server
docker-compose up --build
```

``
Каждый сервис запустится в своем контейнере, никаких дополнительных зависимостей устанавливать не нужно.
``

## 🖥️ Running Client
  Чтобы запустить клиентскую часть, необходимо открыть проект ``Client`` в ``Qt Creator``, запустить сборку проекта.


## 🧪 Postman-коллекция
После настройки и запуска сервера для тестирования некоторых частей API доступна готовая Postman-коллекция:
1. Установите **Node.js**
2. Установите **newman**: ```npm install -g newman``` и **html - репортеры**: ```npm install -g newman-reporter-htmlextra```
2. Импортируйте файл tests из папки postman-tests в Postman или запустите коллекцию с помощью:
```bash
 newman run "tests.json" `
   --iteration-count i `
   --reporters 'cli,htmlextra,json' `
   --reporter-htmlextra-export "newman-report.html" `
   --reporter-htmlextra-title "API Test Report" `
   --reporter-htmlextra-browserTitle "API Tests" `
   --reporter-htmlextra-titleSize "5" `
   --reporter-json-export "newman-report.json" `
   --color on `
   --disable-unicode
```
``i - параметр, число итераций тестирования коллекции``


## 👥 Authors

- [Satlykov Sanjar](https://github.com/Satlykovs)
- [Podrezov Anton](https://github.com/ParovozikThomas)
- [Hovsepyan Arnak](https://github.com/Arnak101)

