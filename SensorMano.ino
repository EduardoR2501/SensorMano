#include <Arduino.h>
#include <Wire.h>                 
#include <MPU6050.h>             
#include <BleCompositeHID.h>
#include <MouseDevice.h>

MPU6050 mpu;

MouseDevice* raton;
BleCompositeHID dispositivoBLE("Mouse feria EMI", "FERIA_EMI", 100);

const int pinClicIzquierdo = 5;
const int pinClicDerecho = 4;

bool estadoBotonIzquierdo = false;
bool estadoBotonDerecho = false;
bool estadoBotonIzquierdoAnterior = false;
bool estadoBotonDerechoAnterior = false;

const int threshold = 1000;  

void setup() {
  Serial.begin(115200);
  
  Wire.begin(21, 22); 
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 conectado correctamente.");
  } else {
    Serial.println("Error al conectar la MPU6050.");
    while (1); 
  }

  pinMode(pinClicIzquierdo, INPUT_PULLUP);  
  pinMode(pinClicDerecho, INPUT_PULLUP); 

  raton = new MouseDevice();
  dispositivoBLE.addDevice(raton);
  
  Serial.println("Inicializando BLE...");
  delay(5000); 
  
  dispositivoBLE.begin();
  Serial.println("BLE inicializado. Conéctate con el dispositivo.");
}

void loop() {
  if (dispositivoBLE.isConnected()) {
    int16_t ax, ay, az;
    mpu.getAcceleration(&ax, &ay, &az);
    
    int x = 0, y = 0;
    if (abs(ax) > threshold) {
      y = map(ax, -17000, 17000, 10, -10); 
    }
    if (abs(ay) > threshold) {
      x = map(ay, -17000, 17000, 10, -10); 
    }

    raton->mouseMove(x, y);
    raton->sendMouseReport(); 

    Serial.print("Estado actual del botón izquierdo (D5): ");
    Serial.println(digitalRead(pinClicIzquierdo));

    if (digitalRead(pinClicIzquierdo) == LOW) {
      estadoBotonIzquierdo = true;
      Serial.println("Botón izquierdo PRESIONADO");
    } else {
      estadoBotonIzquierdo = false;
      Serial.println("Botón izquierdo LIBERADO");
    }

    if (estadoBotonIzquierdo != estadoBotonIzquierdoAnterior) {
      if (estadoBotonIzquierdo) {
        raton->mousePress(MOUSE_LOGICAL_LEFT_BUTTON); 
        Serial.println("Clic izquierdo ENVIADO");
      } else {
        raton->mouseRelease(MOUSE_LOGICAL_LEFT_BUTTON); 
        Serial.println("Clic izquierdo LIBERADO");
      }
      estadoBotonIzquierdoAnterior = estadoBotonIzquierdo;
    }

    Serial.print("Estado actual del botón derecho (D4): ");
    Serial.println(digitalRead(pinClicDerecho));

    if (digitalRead(pinClicDerecho) == LOW) {
      estadoBotonDerecho = true;
      Serial.println("Botón derecho PRESIONADO");
    } else {
      estadoBotonDerecho = false;
      Serial.println("Botón derecho LIBERADO");
    }

    if (estadoBotonDerecho != estadoBotonDerechoAnterior) {
      if (estadoBotonDerecho) {
        raton->mousePress(MOUSE_LOGICAL_RIGHT_BUTTON); 
        Serial.println("Clic derecho ENVIADO");
        delay(200); 
      } else {
        raton->mouseRelease(MOUSE_LOGICAL_RIGHT_BUTTON); 
        Serial.println("Clic derecho LIBERADO");
      }
      estadoBotonDerechoAnterior = estadoBotonDerecho;
    }

    delay(25); 
  }
}
