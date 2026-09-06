#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

// --- Definiciones de pines ---
#define ONE_WIRE_BUS 8
#define Bocina 6
#define rele 7
#define DF_END_PIN 3
#define PIR_PIN A5
#define MQ2_PIN A4
#define RELE_D4 4
#define RELE_D5 5
#define TENSION_CARGADOR A0
#define LED_AZUL A1
#define LED_VERDE A2

// --- Configuración del sensor de temperatura ---
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// --- Configuración del RFID RC522 ---
#define RST_PIN 9
#define SS_PIN 10
MFRC522 mfrc522(SS_PIN, RST_PIN);

// --- Configuración del reproductor MP3 ---
SoftwareSerial mySerial(99, 2); // RX, TX
uint8_t buffer[10] = { 0x7E, 0xFF, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF };

// --- Lista de UIDs de tarjetas autorizadas ---
byte authorizedUIDs[5][4] = {
  {0x63, 0x12, 0xCB, 0x13}, // Tarjeta 1
  {0x03, 0x3E, 0xC5, 0x13}, // Tarjeta 2
  {0x13, 0xBA, 0xBA, 0x13}, // Tarjeta 3
  {0x2F, 0x64, 0x89, 0x1E}, // Tarjeta 4
  {0x23, 0xF5, 0xC2, 0x13}  // Tarjeta 5
};

// --- Variables de estado ---
bool sistemaActivo = false;
bool alarmaPIRActiva = false;
bool alarmaHumoActiva = false;
unsigned long ultimaDeteccionPIR = 0;
unsigned long ultimaDeteccionHumo = 0;
const unsigned long intervaloAntirrebote = 10000;

// --- Variables para control de tensión ---
float voltajeCargador = 0;
const float UMBRAL_TENSION = 10.0;
bool usandoBateria = false;

// --- Variables para mostrar estado de sensores ---
unsigned long ultimoMensajeSensores = 0;
const unsigned long intervaloMensajeSensores = 2000;

// --- Variables para la lógica mejorada de PIR ---
int contadorDeteccionesPIR = 0;
unsigned long primeraDeteccionPIR = 0;
unsigned long segundaDeteccionPIR = 0;
bool alarmaPIRProgramada = false;
unsigned long tiempoActivacionAlarmaPIR = 0;
bool bloqueoDetecciones = false;
const unsigned long intervaloEntreDetecciones = 60000;
const unsigned long tiempoEsperaAlarma = 120000;
const unsigned long tiempoBloqueo = 180000;

// --- Variables para control de audio de humo ---
bool audioHumoEnReproduccion = false;
unsigned long tiempoInicioAudioHumo = 0;
const unsigned long duracionAudioHumo = 10000; // 10 segundos - AJUSTAR según duración real del 001.mp3

// --- Variables para control de ventilador con histéresis ---
bool ventiladorEncendido = false;
const float TEMPERATURA_ENCENDIDO = 30.0;   // Se enciende a 25°C o más
const float TEMPERATURA_APAGADO = 25.0;     // Se apaga a 20°C o menos

// --- NUEVAS VARIABLES PARA CALENTAMIENTO MQ-2 ---
bool mq2Calentado = false;
unsigned long tiempoInicioSistema = 0;
const unsigned long TIEMPO_CALENTAMIENTO_MQ2 = 120000; // 2 minutos en milisegundos
unsigned long ultimoMensajeCalentamiento = 0;
const unsigned long INTERVALO_MENSAJE_CALENTAMIENTO = 5000; // Mostrar mensaje cada 5 segundos

// --- NUEVAS VARIABLES PARA PERIODO DE ESPERA INICIAL PIR ---
bool periodoEsperaPIRActivo = false;
unsigned long tiempoInicioEsperaPIR = 0;
const unsigned long TIEMPO_ESPERA_INICIAL_PIR = 120000; // 2 minutos de espera inicial
unsigned long ultimoMensajeEsperaPIR = 0;
const unsigned long INTERVALO_MENSAJE_ESPERA_PIR = 5000; // Mostrar mensaje cada 5 segundos

void setup() {
  Serial.begin(9600);
  
  // Inicializar componentes
  sensors.begin();
  pinMode(Bocina, OUTPUT);
  pinMode(rele, OUTPUT);
  pinMode(RELE_D4, OUTPUT);
  pinMode(RELE_D5, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(MQ2_PIN, INPUT);
  pinMode(TENSION_CARGADOR, INPUT);
  pinMode(LED_AZUL, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  
  // Iniciar apagados
  digitalWrite(rele, LOW);
  digitalWrite(Bocina, LOW);
  digitalWrite(RELE_D4, LOW);
  digitalWrite(RELE_D5, LOW);
  digitalWrite(LED_AZUL, LOW);
  digitalWrite(LED_VERDE, LOW);

  // Inicializar RFID
  SPI.begin();
  mfrc522.PCD_Init();
  
  // Inicializar MP3
  mySerial.begin(9600);
  DF_vol(5);

  // Inicialización corregida de LEDs
  controlTensionCargadorInicial();
  
  // NUEVO: Guardar el tiempo de inicio del sistema
  tiempoInicioSistema = millis();
  mq2Calentado = false;
  periodoEsperaPIRActivo = false;
  
  delay(2000);

  Serial.println("Sistema iniciado - Use RFID para activar/desactivar");
  Serial.println("===================================================");
  Serial.println(">> SENSOR MQ-2 EN CALENTAMIENTO (2 minutos) <<");
  Serial.print(">> Tiempo restante: ");
  Serial.print((TIEMPO_CALENTAMIENTO_MQ2 / 1000) - ((millis() - tiempoInicioSistema) / 1000));
  Serial.println(" segundos");
  
  mostrarEstadoSensores();
}

void loop() {
  // --- Control automático de tensión del cargador ---
  controlTensionCargador();

  // --- Sección de lectura de temperatura ---
  sensors.requestTemperatures();
  float Temp = sensors.getTempCByIndex(0);
  
  // --- NUEVO: Control del período de calentamiento del MQ-2 ---
  controlCalentamientoMQ2();
  
  // --- NUEVO: Control del período de espera inicial del PIR ---
  controlEsperaInicialPIR();
  
  // --- Mostrar información de sensores periódicamente ---
  if (millis() - ultimoMensajeSensores > intervaloMensajeSensores) {
    ultimoMensajeSensores = millis();
    mostrarEstadoSensores();
  }

  // --- Control del ventilador con histéresis ---
  controlVentilador(Temp);

  // --- Control del sistema con RFID ---
  controlRFID();

  // --- DETECCIÓN DE HUMO - SOLO SI EL MQ-2 ESTÁ CALENTADO ---
  if (mq2Calentado) {
    deteccionHumo();
  }

  // --- Detección PIR si el sistema está activo y terminó el período de espera ---
  if (sistemaActivo && !periodoEsperaPIRActivo && !alarmaHumoActiva) { // No activar PIR si hay humo activo o está en espera inicial
    deteccionPIR();
    
    // Verificar si es hora de activar la alarma PIR programada
    if (alarmaPIRProgramada && millis() >= tiempoActivacionAlarmaPIR) {
      activarAlarmaPIR();
    }
    
    // Verificar si debemos desbloquear las detecciones después del período de bloqueo
    if (bloqueoDetecciones && (millis() - segundaDeteccionPIR) > tiempoBloqueo) {
      bloqueoDetecciones = false;
      resetearContadoresPIR();
    }
  } else {
    // Si el sistema está inactivo o en espera inicial, resetear alarmas PIR
    if (alarmaPIRActiva) {
      detenerAlarmaPIR();
    }
    resetearContadoresPIR();
  }

  delay(500);
}

// --- NUEVA FUNCIÓN: Control del período de espera inicial del PIR ---
void controlEsperaInicialPIR() {
  if (periodoEsperaPIRActivo) {
    unsigned long tiempoTranscurrido = millis() - tiempoInicioEsperaPIR;
    unsigned long tiempoRestante = TIEMPO_ESPERA_INICIAL_PIR - tiempoTranscurrido;
    
    // Mostrar mensaje de espera cada 5 segundos
    if (millis() - ultimoMensajeEsperaPIR > INTERVALO_MENSAJE_ESPERA_PIR) {
      ultimoMensajeEsperaPIR = millis();
      Serial.print(">> ⏳ Espera inicial PIR: ");
      Serial.print(tiempoRestante / 1000);
      Serial.println(" segundos restantes");
    }
    
    // Verificar si ha terminado el período de espera
    if (tiempoTranscurrido >= TIEMPO_ESPERA_INICIAL_PIR) {
      periodoEsperaPIRActivo = false;
      Serial.println(">> ✅ PERIODO DE ESPERA TERMINADO - Detección PIR ACTIVADA");
      Serial.println(">> Sistema PIR listo para operación normal");
    }
  }
}

// --- NUEVA FUNCIÓN: Control del período de calentamiento del MQ-2 ---
void controlCalentamientoMQ2() {
  if (!mq2Calentado) {
    unsigned long tiempoTranscurrido = millis() - tiempoInicioSistema;
    unsigned long tiempoRestante = TIEMPO_CALENTAMIENTO_MQ2 - tiempoTranscurrido;
    
    // Mostrar mensaje de calentamiento cada 5 segundos
    if (millis() - ultimoMensajeCalentamiento > INTERVALO_MENSAJE_CALENTAMIENTO) {
      ultimoMensajeCalentamiento = millis();
      Serial.print(">> MQ-2 Calentándose... Tiempo restante: ");
      Serial.print(tiempoRestante / 1000);
      Serial.println(" segundos");
    }
    
    // Verificar si ha terminado el período de calentamiento
    if (tiempoTranscurrido >= TIEMPO_CALENTAMIENTO_MQ2) {
      mq2Calentado = true;
      Serial.println(">> ✅ SENSOR MQ-2 CALENTADO - Detección de humo ACTIVADA");
      Serial.println(">> Sistema listo para operación normal");
    }
  }
}

// --- Control del ventilador con histéresis ---
void controlVentilador(float temperatura) {
  if (!ventiladorEncendido && temperatura >= TEMPERATURA_ENCENDIDO) {
    // Encender ventilador cuando la temperatura alcanza o supera 25°C
    digitalWrite(rele, HIGH);
    ventiladorEncendido = true;
    Serial.print(" >> Ventilador ENCENDIDO - Temp: ");
    Serial.println(temperatura, 1);
  }
  else if (ventiladorEncendido && temperatura <= TEMPERATURA_APAGADO) {
    // Apagar ventilador cuando la temperatura baja a 20°C o menos
    digitalWrite(rele, LOW);
    ventiladorEncendido = false;
    Serial.print(" >> Ventilador APAGADO - Temp: ");
    Serial.println(temperatura, 1);
  }
}

// --- Control inicial de tensión al iniciar ---
void controlTensionCargadorInicial() {
  int suma = 0;
  for (int i = 0; i < 10; i++) {
    suma += analogRead(TENSION_CARGADOR);
    delay(10);
  }
  int valorAnalogico = suma / 10;
  
  voltajeCargador = (valorAnalogico * 5.0 / 1023.0) * 3.30;
  
  // Actualizar LEDs inmediatamente según la lectura inicial
  if (voltajeCargador >= UMBRAL_TENSION) {
    digitalWrite(RELE_D4, LOW);
    usandoBateria = false;
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_VERDE, LOW);
  } else {
    digitalWrite(RELE_D4, HIGH);
    usandoBateria = true;
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERDE, HIGH);
  }
}

// --- Función para mostrar estado completo de sensores MEJORADA ---
void mostrarEstadoSensores() {
  int lecturaPIR = digitalRead(PIR_PIN);
  int lecturaMQ2 = analogRead(MQ2_PIN);
  int estadoLEDAzul = digitalRead(LED_AZUL);
  
  // Línea 1: Información básica
  Serial.print("Temp:");
  Serial.print(sensors.getTempCByIndex(0), 1);
  Serial.print("C Volt:");
  Serial.print(voltajeCargador, 1);
  Serial.print("V ");
  Serial.print(usandoBateria ? "BAT" : "CAR");
  Serial.print(" LED:");
  Serial.print(estadoLEDAzul ? "AZUL" : "VERDE");
  Serial.print(" Vent:");
  Serial.print(ventiladorEncendido ? "ON" : "OFF");
  
  // Línea 2: Sensores y estado
  Serial.println();
  Serial.print("PIR:");
  Serial.print(lecturaPIR);
  Serial.print(" MQ2:");
  Serial.print(lecturaMQ2);
  
  // Estado MQ2 abreviado
  if (!mq2Calentado) {
    Serial.print("(CALENT)");
  } else if (lecturaMQ2 < 200) {
    Serial.print("(OK)");
  } else if (lecturaMQ2 < 400) {
    Serial.print("(BAJO)");
  } else if (lecturaMQ2 < 600) {
    Serial.print("(MEDIO)");
  } else {
    Serial.print("(HUMO!)");
  }
  
  Serial.print(" Alarma:");
  Serial.print(sistemaActivo ? "ON" : "OFF");
  
  // Información de PIR si está activo
  if (sistemaActivo) {
    if (periodoEsperaPIRActivo) {
      unsigned long tiempoRestante = (tiempoInicioEsperaPIR + TIEMPO_ESPERA_INICIAL_PIR - millis()) / 1000;
      Serial.print(" ESPERA:");
      Serial.print(tiempoRestante);
      Serial.print("s");
    } else {
      Serial.print(" PIRs:");
      Serial.print(contadorDeteccionesPIR);
      
      if (bloqueoDetecciones) {
        Serial.print(" BLOQ");
      }
      
      if (alarmaPIRProgramada) {
        Serial.print(" Alrm:");
        Serial.print((tiempoActivacionAlarmaPIR - millis()) / 1000);
        Serial.print("s");
      }
      if (alarmaPIRActiva) Serial.print(" ALARMA!");
    }
  }
  
  if (alarmaHumoActiva) Serial.print(" HUMO-ACTIVO!");
  
  Serial.println();
}

// --- Control automático de tensión del cargador ---
void controlTensionCargador() {
  int suma = 0;
  for (int i = 0; i < 10; i++) {
    suma += analogRead(TENSION_CARGADOR);
    delay(10);
  }
  int valorAnalogico = suma / 10;
  
  voltajeCargador = (valorAnalogico * 5.0 / 1023.0) * 3.30;
  
  static unsigned long ultimoCambioTension = 0;
  const unsigned long intervaloMinimoCambio = 3000;
  
  // Actualizar LEDs según voltaje inmediatamente
  if (voltajeCargador >= UMBRAL_TENSION) {
    if (!usandoBateria && digitalRead(LED_AZUL) == LOW) {
      digitalWrite(LED_AZUL, HIGH);
      digitalWrite(LED_VERDE, LOW);
    }
  } else {
    if (usandoBateria && digitalRead(LED_VERDE) == LOW) {
      digitalWrite(LED_AZUL, LOW);
      digitalWrite(LED_VERDE, HIGH);
    }
  }
  
  if (millis() - ultimoCambioTension > intervaloMinimoCambio) {
    if (voltajeCargador < UMBRAL_TENSION && !usandoBateria) {
      digitalWrite(RELE_D4, HIGH);
      usandoBateria = true;
      actualizarLEDs();
      ultimoCambioTension = millis();
    } 
    else if (voltajeCargador > (UMBRAL_TENSION + 1.0) && usandoBateria) {
      digitalWrite(RELE_D4, LOW);
      usandoBateria = false;
      actualizarLEDs();
      ultimoCambioTension = millis();
    }
  }
}

// --- Actualizar estado de los LEDs ---
void actualizarLEDs() {
  if (usandoBateria) {
    digitalWrite(LED_AZUL, LOW);
    digitalWrite(LED_VERDE, HIGH);
  } else {
    digitalWrite(LED_AZUL, HIGH);
    digitalWrite(LED_VERDE, LOW);
  }
}

// --- Función de control RFID MEJORADA ---
void controlRFID() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }

  bool isAuthorized = false;
  for (int i = 0; i < 5; i++) {
    if (compareUID(mfrc522.uid.uidByte, authorizedUIDs[i], mfrc522.uid.size)) {
      isAuthorized = true;
      break;
    }
  }

  if (isAuthorized) {
    if (sistemaActivo) {
      // DESACTIVAR SISTEMA - Dos pitidos
      sistemaActivo = false;
      periodoEsperaPIRActivo = false; // Resetear el período de espera
      Serial.println(" >> SISTEMA OFF");
      beep(200);
      delay(250);
      beep(200);
      //digitalWrite(RELE_D5, LOW);
      
      // Detener ALARMA PIR si está activa (INCLUSO si hay alarma de humo)
      if (alarmaPIRActiva || alarmaPIRProgramada) {
        detenerAlarmaPIR();
        Serial.println(" >> Alarma PIR detenida por RFID");
      }
    } else {
      // ACTIVAR SISTEMA - Un pitido
      sistemaActivo = true;
      // INICIAR PERIODO DE ESPERA DE 2 MINUTOS PARA PIR
      periodoEsperaPIRActivo = true;
      tiempoInicioEsperaPIR = millis();
      ultimoMensajeEsperaPIR = millis();
      
      Serial.println(" >> SISTEMA ON");
      Serial.println(" >> ⏳ PERIODO DE ESPERA PIR ACTIVADO (2 minutos)");
      beep(200);
      //digitalWrite(RELE_D5, HIGH);
      resetearContadoresPIR();
    }
  } else {
    Serial.println(" >> NO AUTORIZADO");
    beep(1000);
  }

  mfrc522.PICC_HaltA();
}

// --- Detección de movimiento PIR ---
void deteccionPIR() {
  // Si estamos en período de bloqueo, ignorar detecciones
  if (bloqueoDetecciones) {
    return;
  }
  
  int lecturaPIR = digitalRead(PIR_PIN);
  
  if (lecturaPIR == HIGH) {
    unsigned long tiempoActual = millis();
    
    // Si es la primera detección o han pasado más de 1 minuto desde la última
    if (contadorDeteccionesPIR == 0 || (tiempoActual - primeraDeteccionPIR) > intervaloEntreDetecciones) {
      contadorDeteccionesPIR = 1;
      primeraDeteccionPIR = tiempoActual;
      Serial.println(" >> 1ra deteccion PIR - Esperando 2da");
    }
    // Segunda detección dentro del intervalo de 1 minuto
    else if (contadorDeteccionesPIR == 1 && (tiempoActual - primeraDeteccionPIR) <= intervaloEntreDetecciones) {
      contadorDeteccionesPIR = 2;
      segundaDeteccionPIR = tiempoActual;
      
      // Programar alarma para 2 minutos después
      tiempoActivacionAlarmaPIR = tiempoActual + tiempoEsperaAlarma;
      alarmaPIRProgramada = true;
      bloqueoDetecciones = true;
      
      Serial.println(" >> 2da deteccion PIR - Alarma en 2min");
    }
    
    ultimaDeteccionPIR = tiempoActual;
  }
  
  // Si pasó más de 1 minuto desde la primera detección sin segunda, resetear
  if (contadorDeteccionesPIR == 1 && (millis() - primeraDeteccionPIR) > intervaloEntreDetecciones) {
    resetearContadoresPIR();
  }
}

// --- Activar alarma PIR programada ---
void activarAlarmaPIR() {
  // Solo activar si no hay alarma de humo activa (prioridad del humo)
  if (!alarmaHumoActiva) {
    Serial.println(" >> ALARMA PIR ACTIVADA!");
    alarmaPIRActiva = true;
    alarmaPIRProgramada = false;
    
    // Activar alarma de audio - USA 002.mp3 para PIR
    DF_play(2);
  } else {
    Serial.println(" >> Alarma PIR cancelada - Humo tiene prioridad");
    resetearContadoresPIR();
  }
}

// --- Detener alarma PIR - CORREGIDA ---
void detenerAlarmaPIR() {
  // **CORRECCIÓN: Siempre detener el audio de PIR cuando se desactiva el sistema**
  // El humo tiene prioridad para activarse, pero cuando desactivamos el sistema
  // debemos detener TODAS las alarmas PIR independientemente del estado del humo
  
  if (alarmaPIRActiva) {
    DF_stop(); // Detener el audio inmediatamente
    Serial.println(" >> Audio PIR detenido");
  }
  
  alarmaPIRActiva = false;
  alarmaPIRProgramada = false;
  resetearContadoresPIR();
  
  Serial.println(" >> Alarma PIR completamente desactivada");
}

// --- Resetear contadores PIR ---
void resetearContadoresPIR() {
  contadorDeteccionesPIR = 0;
  primeraDeteccionPIR = 0;
  segundaDeteccionPIR = 0;
  alarmaPIRProgramada = false;
  tiempoActivacionAlarmaPIR = 0;
  bloqueoDetecciones = false;
}

// --- Detección de humo con MQ-2 - SOLO SI ESTÁ CALENTADO ---
void deteccionHumo() {
  int lecturaMQ2 = analogRead(MQ2_PIN);
  
  // Umbral para humo - solo monitoreado si el sensor está calentado
  if (lecturaMQ2 > 300) {
    if (!alarmaHumoActiva) {
      // Primera detección de humo
      Serial.print(" >> DETECCIÓN DE HUMO! Valor:");
      Serial.println(lecturaMQ2);
      
      alarmaHumoActiva = true;
      ultimaDeteccionHumo = millis();
      
      // PRIORIDAD MÁXIMA: Detener cualquier alarma PIR y activar humo
      if (alarmaPIRActiva) {
        DF_stop(); // Detener alarma PIR
        alarmaPIRActiva = false;
        Serial.println(" >> Alarma PIR detenida - HUMO tiene prioridad");
      }
      
      // Activar alarma de audio - USA 001.mp3 para humo
      DF_play(1);
      audioHumoEnReproduccion = true;
      tiempoInicioAudioHumo = millis();
      Serial.println(" >> Alarma de humo activada - Audio 001.mp3");
    }
    else if (alarmaHumoActiva && !audioHumoEnReproduccion) {
      // Humo persistente - reproducir audio nuevamente si no hay audio en curso
      DF_play(1);
      audioHumoEnReproduccion = true;
      tiempoInicioAudioHumo = millis();
      Serial.println(" >> Humo persistente - Reproduciendo audio nuevamente");
    }
  }
  
  // Verificar si el audio ha terminado (usando tiempo estimado)
  if (audioHumoEnReproduccion && (millis() - tiempoInicioAudioHumo) > duracionAudioHumo) {
    audioHumoEnReproduccion = false;
    Serial.println(" >> Audio de humo terminado");
    
    // Si el humo bajó del umbral, desactivar alarma
    if (lecturaMQ2 <= 110) {
      alarmaHumoActiva = false;
      Serial.println(" >> Humo normalizado - Alarma desactivada");
    }
  }
  
  // Si el humo baja del umbral y no hay audio en reproducción, desactivar inmediatamente
  if (lecturaMQ2 <= 110 && alarmaHumoActiva && !audioHumoEnReproduccion) {
    alarmaHumoActiva = false;
    Serial.println(" >> Humo normalizado - Alarma desactivada");
  }
}

// --- Funciones del reproductor MP3 ---
void setBuff(uint8_t _command, uint16_t _option) {
  uint16_t crc = 0x00;
  buffer[0x03] = _command;
  buffer[0x05] = (uint8_t)(_option >> 0x08);
  buffer[0x06] = (uint8_t)_option;
  for (uint8_t i = 0x01; i < 0x07; i++) {
    crc += buffer[i];
  }
  crc = (0xFFFF - crc + 0x01);
  buffer[0x07] = (uint8_t)(crc >> 0x08);
  buffer[0x08] = (uint8_t)crc;
}

void DF_play(uint16_t _option) {
  setBuff(0x03, _option);
  mySerial.write(buffer, sizeof(buffer));
}

void DF_vol(uint16_t _option) {
  setBuff(0x06, _option);
  mySerial.write(buffer, sizeof(buffer));
}

void DF_stop() {
  setBuff(0x16, 0);
  mySerial.write(buffer, sizeof(buffer));
}

// --- Función para comparar UIDs ---
bool compareUID(byte* uid1, byte* uid2, byte length) {
  for (byte i = 0; i < length; i++) {
    if (uid1[i] != uid2[i]) {
      return false;
    }
  }
  return true;
}

// --- Función para controlar la bocina ---
void beep(unsigned int duration) {
  digitalWrite(Bocina, HIGH);
  delay(duration);
  digitalWrite(Bocina, LOW);
}