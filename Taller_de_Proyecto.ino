// librerías
#include <Servo.h>
#include <DS1302RTC.h>
#include <Time.h>

//Set pins
int servoPin = 47; //Pin del servo
int triggerPin = 2; //Pin trigger del sensor
int echoPin = 3; //Pin echo del sensor
int potencPin = A7; //Pin del potenciometro

//Set pins reloj:  CE, IO,CLK (RST, DAT, CLK)
DS1302RTC RTC(40, 38, 36);

//Variables sensor
int tiempo;
int distancia1;
int distancia2;

//Variables del reloj
int hora;
int minu;

//Variables de alimentación
int horaActivacion;
int distActivacion = 20; //A qué distancia se activa el sensor.

bool yaComio = false; //Si ya comió en el día.

//Init servo
Servo myservo;


void setup(void)
{
  //Init serial
  Serial.begin(9600);

  //Setups generales
  pinMode(triggerPin,OUTPUT);
  pinMode(echoPin,INPUT);
  myservo.attach(servoPin);
  myservo.write(100);
  
  //Un delay para el reloj
  delay(2000);

  //No sé qué hace esto, me da miedo sacarlo, es para el reloj.
  setSyncProvider(RTC.get); 
}

void loop(void)
{
    //Potenciometro para la hora de alimentación.
    horaActivacion = round(analogRead(potencPin)*24/1024);
    Serial.print("hora: "+String(horaActivacion)+"\n");
    
    //Cosas del reloj.
    static time_t tLast;
    time_t t;
    tmElements_t tm;
   
    t = now();
    if (t != tLast) {
      tLast = t;
    }
    hora = hour(t);
    minu = minute(t);

    //Alimentación:
    if(hora == horaActivacion && not(yaComio))//¿Es hora de comer? ¿Ha comido hoy?
    {
      distancia1 = medirDistancia(); 
        if(abs(distancia1 - distancia2)>3) //Mostrar distancia si esta varía.
        {
          Serial.print("distancia: "+String(distancia1)+"\n");
          distancia2=distancia1;
        }
      
        if (distancia1 < distActivacion) //El perro está mas cerca que la distancia de activación.
        {
          delay(1000);
          distancia1 = medirDistancia();
          if (distancia1<distActivacion) //El perro sigue cerca.
          {
            comer(); //Dar comida.
            yaComio = true; //No debe comer más hoy.
          }
        }
    }

    if(hora == 0 && minu == 0)//Cambio de dia, el perro no ha comido.
    {
      yaComio = false;
    }
    
    delay(50);
}

void comer() //Mueve el servo para dar comida.
{
  for (int i = 100;i>=0;i--){
      myservo.write(i);
      delay(20);
  }
  delay(1000);
  for (int i = 0;i<=100;i++){
    myservo.write(i);
    delay(20);
  }

}

int medirDistancia() //Mide la distancia del sensor de proximidad.
{
  digitalWrite(triggerPin, LOW); 
  digitalWrite(triggerPin, HIGH); 
  tiempo = pulseIn(echoPin, HIGH);
  return (tiempo/2) / 29.1;
}

