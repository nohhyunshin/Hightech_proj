#include <SPI.h>
#include <Ethernet.h>
#include <ArduinoModbus.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress arduinoIP(192, 168, 0, 50);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress plcIP(192, 168, 0, 66);

EthernetClient ethClient;
ModbusTCPClient modbusClient(ethClient);

const int modbusPort = 502;
const int solpin = 2;
const int dir0 = 5;
const int dir1 = 6;
const int pwm = 7;
const int PhotoSensor = 18;

const int MOTOR_FORWARD = 0;
const int MOTOR_REVERSE = 1;
const int REG_PHOTO = 2;
const int REC_SOL = 3;

volatile bool pesChanged = false;  // 센서 변화 감지 플래그
volatile int pesValue = 0;

// 포토센서 
void pesChanged_ISR() {
  pesValue = digitalRead(PhotoSensor) == LOW ? 1 : 0;
  pesChanged = true;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(53, OUTPUT);      // 메가 SPI SS핀
  pinMode(17, OUTPUT);      // CS 핀
  pinMode(dir0, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(pwm, OUTPUT);
  pinMode(solpin, OUTPUT);
  pinMode(PhotoSensor, INPUT);
  Serial.begin(9600);

  attachInterrupt(digitalPinToInterrupt(PhotoSensor), pesChanged_ISR, CHANGE);

  Ethernet.init(17);
  Ethernet.begin(mac, arduinoIP, gateway, gateway, subnet);
  delay(1500);

  Serial.print("아두이노 IP: ");
  Serial.println(Ethernet.localIP());

  connectToPLC();
}

void connectToPLC() {
  Serial.println("PLC Modbus TCP 연결 시도...");

  if (!modbusClient.begin(plcIP, modbusPort)) {
    Serial.println("PLC 연결 실패!");
  } else {
    Serial.println("PLC 연결 성공!");
  }
}

void motorForward() {
  digitalWrite(dir0, HIGH);
  digitalWrite(dir1, LOW);
  digitalWrite(pwm, HIGH);
  Serial.println("모터 정방향");
}

void motorReverse() {
  digitalWrite(dir0, LOW);
  digitalWrite(dir1, HIGH);
  digitalWrite(pwm, HIGH);
  Serial.println("모터 역방향");
}

void motorStop() {
  digitalWrite(dir0, LOW);
  digitalWrite(dir1, LOW);
  digitalWrite(pwm, LOW);
  Serial.println("모터 정지");
}

void sendPhotoSensor() {
  modbusClient.beginTransmission(HOLDING_REGISTERS, REG_PHOTO, 1);
  modbusClient.write((int)pesValue);
  if (!modbusClient.endTransmission()) {
    Serial.println("포토센서 전송 실패");
  } else {
    Serial.print("포토센서 PLC 전송: ");
    Serial.println(pesValue);
  }
}

void controlSolenoid(int solCmd) {
  if (solCmd == 1) {
    digitalWrite(solpin, HIGH);
  } else if (solCmd == 0) {
    digitalWrite(solpin, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!modbusClient.connected()) {
    Serial.println("연결 끊김. 재연결 시도...");
    connectToPLC();
    delay(1000);
    return;
  }

  // PLC HR 레지스터 읽기
  if (!modbusClient.requestFrom(HOLDING_REGISTERS, MOTOR_FORWARD, 4)) {
    Serial.println("레지스터 읽기 실패");
    return;
  }

  int motorFor = modbusClient.read();
  int motorRev = modbusClient.read();
  modbusClient.read();
  int solCmd = modbusClient.read();

  // 명령값에 따라 모터 제어
  if (motorFor == 1) {
    motorForward();
  } else if (motorRev == 1) {
      motorReverse();
  } else {
      motorStop();
  }

  // 포토센서 읽기
  // int PES = digitalRead(PhotoSensor);
  // Serial.println(PES == 1 ? "포토센서: OFF" : "포토센서: ON");
  
  // int pesValue = (PES == 0) ? 1 : 0; 
  // modbusClient.beginTransmission(HOLDING_REGISTERS, REG_PHOTO, 1);
  // modbusClient.write(pesValue);
  // if (!modbusClient.endTransmission()) {
  //   Serial.println("포토센서 전송 실패");
  // } else {
  //   Serial.print("포토센서 PLC 전송: ");
  //   Serial.println(pesValue);
  // }

  if (solCmd == 0 || solCmd == 1) {
    controlSolenoid(solCmd);
    Serial.print("solCmd 수신값: ");
    Serial.println(solCmd);
  }

  if (pesChanged) {
    pesChanged = false;
    Serial.println(pesValue == 0 ? "포토센서: ON" : "포토센서: OFF");
    sendPhotoSensor();
  }

  delay(20);
}