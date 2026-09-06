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
<img width="750" height="1000" alt="01" src="https://github.com/user-attachments/assets/b5bb0fdb-f39c-43b0-9afe-ce4504b2827c" />
<img width="750" height="1000" alt="02" src="https://github.com/user-attachments/assets/d524baee-9d30-4190-9e1c-c5f0468dceeb" />
<img width="607" height="1080" alt="03" src="https://github.com/user-attachments/assets/62424f88-fc2f-4050-9878-898b633ebb8a" />
<img width="607" height="1080" alt="04" src="https://github.com/user-attachments/assets/980885cf-6e76-4a38-ad45-944889443fe7" />
<img width="607" height="1080" alt="05" src="https://github.com/user-attachments/assets/ac5184ea-b23f-401c-84fc-b042348ea084" />
<img width="1080" height="607" alt="06" src="https://github.com/user-attachments/assets/a7689437-495d-4f65-96f6-2cecd9ec9e78" />
<img width="1080" height="607" alt="07" src="https://github.com/user-attachments/assets/e1c865b9-6153-4f5f-aab5-c380136eb309" />
<img width="607" height="1080" alt="08" src="https://github.com/user-attachments/assets/1af3946a-7852-4a81-abaa-eb5557575b51" />

markdown
## 👤 Autor

**Leno Perdomo Céspedes**  
[GitHub](https://github.com/lennon025) · [Email](mailto:lennon.perdomocespedes@gmail.com)

*Siempre pensando en mi Cuba, desde Moscú, Rusia.*
