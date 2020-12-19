#include <WiFi.h>
#include <ESP32Ping.h>
const int trigPin = 2;
const int echoPin = 5;

const char* ssid     = "eld_kzj_3_3";
const char* password = "as.df.gh12";
// defines variables
long duration;
int distance;
char linebuf[80];
int charcount=0;
hw_timer_t *watchdogTimer = NULL;
WiFiServer server(80);



     // контакт для второго светодиода


void setup() {
watchdogTimer = timerBegin(0, 80, true); //timer 0 divisor 80
  timerAlarmWrite(watchdogTimer, 30000000, false); // set time in uS must be fed within this time or reboot
  timerAttachInterrupt(watchdogTimer, & interruptReboot, true);
  timerAlarmEnable(watchdogTimer);  // enable interrupt
pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
pinMode(echoPin, INPUT); // Sets the echoPin as an Input
Serial.begin(9600);

  while(!Serial) {
    ; // ждем подключения последовательного порта 
      // (нужно только для штатного USB-порта) 
  }
  
  // начинаем с подключения к WiFi-сети: 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");  //  "Подключаемся к "
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  // пытаемся подключиться к WiFi-сети: 
  while(WiFi.status() != WL_CONNECTED) {
    // подключаемся к сети WPA/WPA2; поменяйте эту строчку, 
    // если используете открытую или WEP-сеть:
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  //  "Подключение 
                                     //  по WiFi выполнено"
  Serial.println("IP address: ");    //  "IP-адрес: "
  Serial.println(WiFi.localIP());
  
  server.begin();


//Serial.begin(9600); // Starts the serial communication
}
void loop() {
timerWrite(watchdogTimer, 0);
IPAddress ip (192, 168, 11, 1); // The remote ip to ping
bool ret = Ping.ping(ip);
if(!ret)
 {
interruptReboot();
}


// Clears the trigPin
/*digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
// Prints the distance on the Serial Monitor
if(distance>0){
Serial.print("Distance: ");
Serial.println(distance);
}*/



 //анализируем канал связи, высматривая входящих клиентов:
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client");  //  "Новый клиент"
    memset(linebuf,0,sizeof(linebuf));
    charcount=0;
    // HTTP-запрос заканчивается пустой строкой:
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // считываем HTTP-запрос, символ за символом: 
        linebuf[charcount]=c;
        if (charcount<sizeof(linebuf)-1) charcount++;
        // если добрались до конца строки (т.е. получили 
        // символ новой строки) и строка пуста, 
        // это значит, что HTTP-запрос закончился;
        // следовательно, можно отправлять ответ:
        if (c == '\n' && currentLineIsBlank) {
          // отправляем стандартный заголовок HTTP-ответа: 
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");  
                     //  "Тип контента: text/html"
          client.println("Connection: close"); 
                     //  "Соединение: отключено";
                     //  после отправки ответа связь будет отключена 
          client.println();
          client.println("<!DOCTYPE HTML><html><head>");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><meta http-equiv=\"Cache-Control\" content=\"no-cache\"><meta http-equiv=\"Cache-Control\" content=\"private\"></head>");
int i=0, j=0;
int distanceMassive[10];
while((i<10)&&(j<15)){
j++;
delay(500);
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
// Sets the trigPin on HIGH state for 10 micro seconds
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
// Reads the echoPin, returns the sound wave travel time in microseconds
duration = pulseIn(echoPin, HIGH);
// Calculating the distance
distance= duration*0.034/2;
if((distance>0)&&(distance<1000)){
distanceMassive[i]=distance;
i++;
}
}
distance=0;
for(i=0;i<10;i++){
client.print("<h1>");
          client.print(distanceMassive[i]);
		  
          client.print("</h1>");
distance+=distanceMassive[i];
}
distance=distance/10;
          
          client.println("</html>");
          break;
        }
        
      }
    }
    // даем веб-браузеру время, чтобы получить данные: 
    delay(1);

    // закрываем соединение:
    client.stop();
    Serial.println("client disconnected");  //  "клиент отключен"
  }
}

void interruptReboot()
{
  Serial.print("Rebooting .. \n\n");
  ets_printf("reboot\n");
  esp_restart();
}