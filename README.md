# Cespedes EnerGuard – Control de Refrigerador y Vigilante Energético

Sistema para mantener un refrigerador de 12V funcionando con **red eléctrica o batería**, con alarmas de **humo** /**antirrobo**, y control de **temperatura**.

---

## 🔌 ¿Qué hace?

- Si hay **red eléctrica** → el refrigerador usa el cargador 12V.
- Si **no hay red** → el refrigerador usa la batería de 12V.
- Si la temperatura supera los **30°C** → se enciende un ventilador para proteger los componentes.
- Si se detecta **humo** (batería, inversor, bicicleta eléctrica, etc.) → suena **alarma de bomberos** (prioridad máxima, no se desactiva manualmente).
- Si el sistema **PIR** está activado y detecta movimiento → suena **alarma antirrobo** (se desactiva con tarjeta NFC).
- El sistema da **2 minutos** para activar/desactivar la alarma antes de que salte.

---

## 📡 Conexiones (pines principales)

| Pin | Conecta a |
|-----|-----------|
| A0 | Divisor de tensión (detecta 12V del cargador) |
| A1 | LED azul (batería) |
| A2 | LED verde (red eléctrica) |
| A4 | Sensor de humo MQ-2 |
| A5 | Sensor de movimiento PIR |
| D4 | Relé de conmutación red/batería |
| D6 | Bocina piezoeléctrica (pitidos de confirmación) |
| D7 | Relé del ventilador |
| D8 | Sensor de temperatura DS18B20 |
| D9 + D10 | RFID RC522 (activación/desactivación de alarma) |

---

## 🔊 Archivos de audio (microSD del MP3)

| Archivo | Evento |
|---------|--------|
| `001.mp3` | Alarma de humo (prioridad máxima, no se desactiva) |
| `002.mp3` | Alarma antirrobo (se desactiva con tarjeta NFC) |

---

## 🚀 Cómo usarlo

1. Conecta todo según la tabla de pines.
2. Sube el código al Arduino Nano.
3. Abre el monitor serie, pasa tu tarjeta NFC y copia su UID.
4. Agrega ese UID al arreglo `authorizedUIDs[]` en el código.
5. Pasa la tarjeta para activar el sistema → pitido de confirmación.
6. Tienes **2 minutos** para salir de la habitación antes de que el PIR empiece a vigilar.
7. Para desactivar, pasa la tarjeta otra vez (2 pitidos de confirmación).
8. Si la alarma suena, usa la tarjeta para detenerla.

> 🔥 **Coloca el sensor MQ-2 cerca de la batería** para detectar humo a tiempo.

---

# Cespedes EnerGuard – Управление холодильником и энергетический страж

Система для поддержания работы автомобильного холодильника 12В от **сети или аккумулятора**, с сигнализациями **дыма** и **взлома**, а также контролем **температуры**.

---

## 🔌 Что делает система?

- Если есть **напряжение в сети** → холодильник питается от зарядного устройства 12В.
- Если **сеть отключена** → холодильник питается от аккумулятора 12В.
- При температуре выше **30°C** → включается вентилятор для защиты компонентов внутри корпуса.
- При обнаружении **дыма** (аккумулятор, инвертор, электровелосипед и т.п.) → включается **пожарная сирена** (высший приоритет, не отключается вручную).
- Если активирована система **PIR** и обнаружено движение → включается **охранная сигнализация** (отключается NFC-картой).
- Система даёт **2 минуты** на активацию/деактивацию сигнализации перед срабатыванием.

---

## 📡 Подключение (основные пины)

| Пин | Подключение |
|-----|-------------|
| A0 | Делитель напряжения (обнаружение 12В от зарядного устройства) |
| A1 | Синий светодиод (питание от аккумулятора) |
| A2 | Зелёный светодиод (питание от сети) |
| A4 | Датчик дыма MQ-2 |
| A5 | Датчик движения PIR |
| D4 | Реле переключения сеть/аккумулятор |
| D6 | Пьезоизлучатель (звуковые сигналы подтверждения) |
| D7 | Реле вентилятора |
| D8 | Датчик температуры DS18B20 |
| D9 + D10 | RFID RC522 (активация/деактивация охраны) |

---

## 🔊 Аудиофайлы (на microSD MP3-модуля)

| Файл | Событие |
|------|---------|
| `001.mp3` | Пожарная сирена (высший приоритет, не отключается) |
| `002.mp3` | Охранная сирена (отключается NFC-картой) |

---

## 🚀 Как использовать

1. Подключите всё согласно таблице пинов.
2. Загрузите код в Arduino Nano.
3. Откройте монитор порта, поднесите NFC-карту и скопируйте её UID.
4. Добавьте этот UID в массив `authorizedUIDs[]` в коде.
5. Поднесите карту для активации системы → звуковой сигнал подтверждения.
6. У вас есть **2 минуты**, чтобы покинуть помещение, после чего PIR начнёт охрану.

7. Для деактивации снова поднесите карту (два звуковых сигнала подтверждения).
8. Если сигнализация сработала, используйте карту для её отключения.

> 🔥 **Разместите датчик MQ-2 рядом с аккумулятором** для своевременного обнаружения дыма.

---
<img width="750" height="1000" alt="01" src="https://github.com/user-attachments/assets/bcaf542c-9a81-42ce-8033-c7749dc64d48" />
<img width="750" height="1000" alt="02" src="https://github.com/user-attachments/assets/69af65bf-9d01-40fe-b7f7-3b7fe3e7ea57" />
<img width="607" height="1080" alt="03" src="https://github.com/user-attachments/assets/4d95c7e6-f6df-44fc-9b63-f425f8ea3be5" />
<img width="940" height="1673" alt="04" src="https://github.com/user-attachments/assets/f03b6b7d-fb0f-484d-802a-0efbc2426a45" />
<img width="1004" height="1566" alt="05" src="https://github.com/user-attachments/assets/fb4031b3-7cf4-42ec-86e8-991c496ccda8" />
<img width="1673" height="940" alt="06" src="https://github.com/user-attachments/assets/6a48b9da-78fb-4ce8-a8c3-8f6c50e13961" />
<img width="1672" height="941" alt="07" src="https://github.com/user-attachments/assets/03a8e9e4-7aac-49c7-8cf1-e5e806d57f6a" />
<img width="1080" height="607" alt="08" src="https://github.com/user-attachments/assets/94d08519-633a-458a-96e6-d277c7ef330c" />

markdown
## 👤 Autor

**Leno Perdomo Céspedes**  
[GitHub](https://github.com/lennon025) · [Email](mailto:lennon.perdomocespedes@gmail.com)

*Siempre pensando en mi Cuba, desde Moscú, Rusia.*
