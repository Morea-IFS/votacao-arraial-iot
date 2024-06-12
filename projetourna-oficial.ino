//Bibiotecas.
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Keypad.h>
#include <ESP8266HTTPClient.h>

//Definição do numero de linhas e colunas.
const byte LINHAS = 4;
const byte COLUNAS = 3;

//Dados referentes a conexão wifi e servidor.
const char* ssid = "MINASTELECOM-SIMONE";
const char* password = "helena2228";
const char* serverAddress = "morea-ifs.org";

//Matriz do teclado.
char teclas [LINHAS][COLUNAS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}  
};

//Endereço e config do display.
LiquidCrystal_I2C lcd(0x27,16,2);

//portas: S2; D0; D3; D4;
byte linhasPinos[LINHAS] = {9, 16, 0, 2};

//portas: D5; D6; D7;
byte colunasPinos[COLUNAS] = {14, 12, 13};

//objeto: teclado.
Keypad teclado = Keypad(makeKeymap(teclas), linhasPinos, colunasPinos, LINHAS, COLUNAS);

//String vazia onde será armazenada a matricula
char matricula[11] = "";

//String vazia onde será armazenado o numero do candidato
char candidato[3] = "";

//inicio do void
void setup(){

  //comunicação serial iniciada
  Serial.begin(9600);  
  //comunicação wifi iniciada
  WiFi.begin(ssid,password);
  //comunicação lcd i2c iniciada
  lcd.init();
  lcd.backlight();
  void pedidoMatricula();

}
void loop(){

  char tecla = teclado.getKey();
  if(tecla && strlen(matricula)<6){
    void matriculaDigit();
  }
  else if(tecla == '#' && strlen(candidato)<2 && strlen(matricula)>=6){
    void candidatoDigit();
  }
  else if(tecla == '#' && strlen(candidato)>=1){
    void enviarDados();
  }
}

void pedidoMatricula(){
  char tecla = teclado.getKey();
  lcd.setCursor(1,0);
  lcd.print("INFORME A SUA");
  lcd.setCursor(3,1);
  lcd.print("MATRICULA");
  if(tecla){
    void matriculaDigit();
  }
}

void matriculaDigit(){
  char tecla = teclado.getKey();
  if(tecla){
    if(strlen(matricula) < 5){
      if(tecla == '*'){      
        if(strlen(matricula) > 0){
              matricula[strlen(matricula)-1] = '\0';
        }  
      }else if (strlen(matricula) < 10){          
              matricula[strlen(matricula)] = tecla;      
              matricula[strlen(matricula)+1] = '\0';
              Serial.print("MATR-CONCAT:");
              Serial.println(matricula);
              Serial.print("");
      }     
      lcd.clear();     
      lcd.setCursor(3,0);
      lcd.print("MATRICULA:");
      lcd.setCursor(3,1);
      lcd.print(matricula);
      Serial.print("MATRICULA:");
      Serial.println(matricula);
      delay(100);
    }
  }

}
void condidatoDigit(){
  char tecla = teclado.getKey();
  if(tecla=='#'){
    if(strlen(matricula)>5){
      if(strlen(candidato)<2){
          if(tecla == '*'){      
            if(strlen(candidato) > 0){
              candidato[strlen(candidato)-1] = '\0';
          }  
        }else if (strlen(candidato) < 10){          
          if(tecla != '#'){
            candidato[strlen(candidato)] = tecla;      
            candidato[strlen(candidato)+1] = '\0';
            Serial.print("CAND-CONCAT:");
            Serial.println(candidato);
            Serial.print("");
          }
        }     
        lcd.clear();     
        lcd.setCursor(3,0);
        lcd.print("CANDIDATO:");
        lcd.setCursor(3,1);
        lcd.print(candidato);
        Serial.print("CANDIDATO:");
        Serial.println(candidato);
        delay(100);
      }
    }
  }

}
void enviarDados(){
  Serial.print("ESTAMOS ENVIANDO!");
  int freeHeap = ESP.getFreeHeap();
  Serial.print("Memória RAM livre: ");
  Serial.print(freeHeap);
  Serial.println(" bytes");

  if (WiFi.status() == WL_CONNECTED) {       
    HTTPClient http;
    WiFiClient client;
    String url = "http://" + String(serverAddress) + "/enviarDados?matricula=" + matricula + "&candidato=" + candidato;                   
    http.begin(client, url);
    Serial.print("URLSERVER-HTTP: ");
    Serial.println(url);
    Serial.println("");
    int httpResponseCode = http.GET();      
    if (httpResponseCode > 0){
      Serial.print("Resposta do servidor: ");
      Serial.println(http.getString());
    }
    else{
      Serial.print("Falha na solicitação HTTP, código de erro: ");
      Serial.println(httpResponseCode);
    }      
      http.end();     
  }
}