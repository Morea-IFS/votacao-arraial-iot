//Bibiotecas.
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <ESP8266HTTPClient.h>

//Definição do numero de linhas e colunas.
const byte LINHAS = 4;
const byte COLUNAS = 3;

//Dados referentes a conexão wifi e servidor.
const char* ssid = "IFSLIVRE";
const char* password = "";
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
char matricula[12] = "";
bool matricula_ok = false;

//String vazia onde será armazenado o numero do candidato
char candidato[4] = "";
bool candidato_ok = false;


//inicio do void
void setup(){
  //comunicação serial iniciada
  Serial.begin(9600);  
  //comunicação wifi iniciada
  WiFi.begin(ssid,password);
  while(WiFi.status() != WL_CONNECTED){
    Serial.println("");
    Serial.println(".");
   
  }
  Serial.println(WiFi.localIP());
  //comunicação lcd i2c iniciada
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print("INFORME A SUA");
  lcd.setCursor(3,1);
  lcd.print("MATRICULA");
}
void loop(){

  char tecla = teclado.getKey();

  if(tecla){
    if(strlen(matricula)<1){    
      if(tecla == '*'){      
        if(strlen(matricula) > 0){
          matricula[strlen(matricula)-1] = '\0';
        }  
      }else if(tecla == '#'){
        matricula_ok = true;
        candidato_ok = false;
               
       }else if (strlen(matricula) < 10){          
          matricula[strlen(matricula)] = tecla;      
          matricula[strlen(matricula)+1] = '\0';
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
    if(strlen(matricula)<0){
      if(tecla == '*'){      
        if(strlen(candidato) > 0){
          candidato[strlen(candidato)-1] = '\0';
        }  
      }else if(tecla == '#' && strlen(candidato)>1){
        candidato_ok = true;
        matricula_ok = true;       
       }else if (strlen(candidato) < 10){          
          if(tecla != '#'){
            candidato[strlen(candidato)] = tecla;      
            candidato[strlen(candidato)+1] = '\0';
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
        
        
    if(candidato_ok == true && matricula_ok == true){
       lcd.clear();
       lcd.setCursor(3,0);
       lcd.print("ESTAMOS ENVIANDO");
       lcd.setCursor(3,1);
       lcd.print("AGUARDE");
        
       Serial.print("ESTAMOS ENVIANDO!");
       if (WiFi.status() == WL_CONNECTED) {       
         HTTPClient http;
         WiFiClient client;
         String url = "http://" + String(serverAddress) + "/enviarDados?matricula=" + matricula + "&candidato=" + candidato;                   
         http.begin(client, url);
         int httpResponseCode = http.GET();      
         if (httpResponseCode > 0) {
           Serial.print("Resposta do servidor: ");
           Serial.println(http.getString());
         }else {
           Serial.print("Falha na solicitação HTTP, código de erro: ");
           Serial.println(httpResponseCode);
         }      
         http.end();
           memset(matricula, 0, sizeof(matricula));
           memset(candidato, 0, sizeof(candidato));
           matricula_ok = false;
           candidato_ok = false;
    
           lcd.clear();
           lcd.setCursor(1,0);
           lcd.print("INFORME A SUA");
           lcd.setCursor(3,1);
           lcd.print("MATRICULA");
            
       }
    }
  }
  delay(100);
}
