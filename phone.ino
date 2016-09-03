#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <String.h>
 
SoftwareSerial mySerial(7,8);

char uppercase[10][10] = { 
  { '.', '?', ',', '\'', '!', '0', 0 },
  { ' ', '1', 0 },
  { 'A', 'B', 'C', '2', 0 },
  { 'D', 'E', 'F', '3', 0 },
  { 'G', 'H', 'I', '4', 0 },
  { 'J', 'K', 'L', '5', 0 },
  { 'M', 'N', 'O', '6', 0 },
  { 'P', 'Q', 'R', 'S', '7', 0 },
  { 'T', 'U', 'V', '8', 0 },
  { 'W', 'X', 'Y', 'Z', '9', 0 },
};

char lowercase[10][10] = { 
  { '.', '?', ',', '\'', '!', '0', 0 },
  { ' ', '1', 0 },
  { 'a', 'b', 'c', '2', 0 },
  { 'd', 'e', 'f', '3', 0 },
  { 'g', 'h', 'i', '4', 0 },
  { 'j', 'k', 'l', '5', 0 },
  { 'm', 'n', 'o', '6', 0 },
  { 'p', 'q', 'r', 's', '7', 0 },
  { 't', 'u', 'v', '8', 0 },
  { 'w', 'x', 'y', 'z', '9', 0},
};

const byte ROWS = 6;const byte COLS = 3;
char keys[ROWS][COLS] = {
{'!','U','?'},
{'L','D','R'},
{'1','2','3'},
{'4','5','6'},
{'7','8','9'},
{'*','0','#'},
};
byte rowPins[ROWS] = {A0,A1,A2,A3,A4,A5};
byte colPins[COLS] = {8,9,10};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int keycount=0;
unsigned long lastKeyPressTime;
unsigned long lastKeyPressTime2;
char lastKey;
char (*letters)[10];
int lastKeyIndex;
char text;

boolean shiftNextKey=true;
String textMessage;
String textMessagefornow;

String incNum="";
String mode;
String editNum="";

int brightness=100;

String timedate;
 
byte signal1[8] = {B10000,B00000,B10000,B00000,B10000,B00000,B10000};byte signal2[8] = {B10000,B00000,B10000,B00000,B10000,B00000,B11000};byte signal3[8] = {B10000,B00000,B10000,B00000,B11100,B00000,B11000};byte signal4[8] = {B10000,B00000,B11110,B00000,B11100,B00000,B11000};byte signal5[8] = {B11111,B00000,B11110,B00000,B11100,B00000,B11000};

int getsig;

void setup(){
  digitalWrite(9, LOW);
      
  lcd.createChar(0, signal1);lcd.createChar(1, signal2);lcd.createChar(2, signal3);lcd.createChar(3, signal4);lcd.createChar(4, signal5);lcd.createChar(5, signal5);
  
  pinMode(9,OUTPUT);
  digitalWrite(9, LOW);
  
  
  /**/keypad.setHoldTime(1000);
  
  mySerial.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);    // the GPRS baud rate 
  delay(200);
  
  //flashlight
  pinMode(13,OUTPUT);
  digitalWrite(13, LOW);

  //backlight
  pinMode(6,OUTPUT);
  analogWrite(6,brightness);
  lcd.begin(16, 2);  
  lcd.print("Powering on...");
  
  mySerialpowerOn();
  
  mySerial.println("AT");
  delay(200);
  mySerial.println("AT+CLVL=100");
  delay(100);
  mySerial.println();
  


  getsig=getSignal();
  timedate=getTimeHM();
  

  mode="MAIN";

  keypad.addEventListener(keypadEvent);    
}
 
void loop(){

  if(millis()%1000==0){
    if(mode!="INCALL"){
      if(SIM900ringing()==1){
        mode="RINGING";
      }
    }
  }


  if(millis()%30000==0){
    timedate=getTimeHM();
    getsig=getSignal();
  }
  

    if(mode=="MAIN"){
      digitalWrite(13, LOW);
        if(millis()%500==0){
       //battery
      lcd.setCursor(0, 0);
      if(getsig==99){lcd.print("?");}else{lcd.write(byte(getsig));}
        lcd.print(timedate);
        lcd.setCursor(0, 1);
        lcd.print(">CALL       >SMS");
        analogWrite(6,brightness);
        }
      }else if(mode=="LOCKED"){
      digitalWrite(13, LOW);
          if(millis()%500==0){
      lcd.setCursor(0, 0);
        lcd.print("LOCKED          ");
        lcd.setCursor(0, 1);
        lcd.print(">UNLOCK         ");
        analogWrite(6,0);
       }
      }else if(mode=="EDITNUM"){
          if(millis()%100==0){
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(">CALL    >CANCEL");
        lcd.setCursor(0, 0);
        lcd.print(editNum);
        lcd.cursor();
          }
      }else if(mode=="EDITTEXT"){
        if(millis()%100==0){
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(">SEND    >CANCEL");
        lcd.setCursor(0, 0);
        lcd.print(textMessagefornow);
        lcd.cursor();
        }
    }else if(mode=="SENDTEXT"){
        if(millis()%100==0){
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(">SEND    >CANCEL");
        lcd.setCursor(0, 0);
        lcd.print(editNum);
        lcd.cursor();
      }
  }else if(mode=="RINGING"){
    if((millis()-500)%1000==0){
      digitalWrite(13, HIGH);
    }
    if(millis()%1000==0){
      digitalWrite(13, LOW);
    }
    if(millis()%500==0){
      analogWrite(6,brightness);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print(">ANSWER >DECLINE");
      lcd.setCursor(0, 0);
      if(incNum!=""){
      lcd.print(""+incNum+"");
      }
    }
  }else if(mode=="INCALL"){
      digitalWrite(13, LOW);
    if(millis()%100==0){
      analogWrite(6,brightness);
      lcd.clear();
      lcd.setCursor(0, 1);
      lcd.print("        >HANG UP");
      lcd.setCursor(0, 0);
      lcd.print(incNum);
    }
  }
  
  
    if(mode=="MAIN"){if(millis()-lastKeyPressTime>30000){mode="LOCKED";}}

  
  
/*  if (mySerial.available()){
    Serial.write(mySerial.read());
  }*/

  char key = keypad.getKey();   
  if(key){
    if(mode=="EDITNUM"){
      if(key=='!'){
        DialVoiceCall(editNum);
      }else{
      numInput(key);
      }
    }else if(mode=="LOCKED" && key=='!'){
      mode="MAIN";
    }else if(mode=="CALLING" && key=='?'){
      CancelVoiceCall();
    }else if(mode=="MAIN"){
      if(key=='!'){mode="EDITNUM";}else  if(key=='?'){mode="EDITTEXT";}
    }else if(mode=="EDITTEXT"){
      if(key=='!'){
        mode="SENDTEXT";
        editNum="";
      }else{
        textInput(key, textMessage, textMessage.length());
      }
    }else if(mode=="SENDTEXT"){
      if(key=='!'){
        SendTextMessage(editNum,textMessagefornow);
      }else{
        numInput(key);
      }
    }else if(mode=="RINGING"){
      if(key=='!'){
        ////answer ATA
        AnswerVoiceCall();
        mode="INCALL";
      }else if (key=='?'){
        //hangup
        CancelVoiceCall();
      }
    }else if(mode=="INCALL"){
      if (key=='?'){
        //hangup
        CancelVoiceCall();
      }
    }
    lastKeyPressTime=millis();
 //   lastKey=key - '0';
  }
}

 

void textInput(char key, String buf, int len){
  
  if (shiftNextKey==false) {
    letters = lowercase;
  }
  if (shiftNextKey==true) {
    letters = uppercase;
  }

  
      
if(key!='#'&&key!='*'&&key!='!'&&key!='?'&&key!='U'&&key!='L'&&key!='D'&&key!='R'){

  int keyint= String(key).toInt();
  int lastKeyint = String(lastKey).toInt();

  if ((millis() - lastKeyPressTime2) > 1000 || key!=lastKey){
    keycount = 0;
    
    if(textMessagefornow!=""){
      if(key!='0'&&key!='1'&&lastKey!='#'){shiftNextKey = false;letters = lowercase;}
      if(lastKey=='#'){letters = uppercase;}
    }
    
    text = letters[keyint][keycount];
    

    if(textMessagefornow){
      textMessage = textMessagefornow;
    }
    
    textMessagefornow = textMessage + text;
  }else{
    text = letters[keyint][keycount]; 
    textMessagefornow = textMessage + text;
  }
  
  keycount += 1;
  lastKey=key;
  if(letters[keyint][keycount]==0){keycount=0;}
  lastKeyPressTime2=millis();
  }

   if (key == '#') {
    if(shiftNextKey == true){
      shiftNextKey=false;
    }else{
      shiftNextKey=true;
    }
    lastKey='#';
  }
  if (key == 'L') {
    textMessagefornow = textMessagefornow.substring(0, textMessagefornow.length() - 1);
    textMessage=textMessagefornow;
  }
    if(key=='?'){
    textMessage="";
    textMessagefornow="";
    mode="MAIN";
  }
}
 
void SendTextMessage(String nummm,String texttt){
//  Serial.print(nummm+":"+texttt);
  lcd.clear();
  lcd.print("Sending text...");
  mySerial.println("AT");
  delay(200);
  mySerial.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(100);
//  mySerial.println("AT + CMGS = \"+96170686887\"");//send sms message, be careful need to add a country code before the cellphone number
  mySerial.println("AT + CMGS = \""+nummm+"\"");
  delay(100);
  mySerial.println(""+texttt+"");//the content of the message
  delay(100);
  mySerial.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  mySerial.println();
   delay(1000);
   mode="MAIN";
}
 
 /**/
void DialVoiceCall(String phonenumber){
  if(phonenumber.substring(0,1)=="*"){
    getUSSD(phonenumber);
  }else{
  mySerial.println("AT");
  delay(200);
  mySerial.println("ATD + "+phonenumber+";");//dial the number
  delay(100);
  mySerial.println();
  mode="CALLING";
  lcd.begin(16,2);  
  lcd.print("CALLING...");
  lcd.setCursor(0,1);
  lcd.print("            >END");
  delay(1000);
  ShowSerialData2();
  }
}

/**/
void getUSSD(String code){
   mySerial.println("AT");
    delay(200);
      mySerial.println("AT+CUSD=1,\""+code+"\"");
      delay(100);
  mySerial.println(); 
  delay(200);
    mode="USSD";
  lcd.clear();  
  lcd.print("running USSD...");
  delay(3000);
 // mySerial.println("AT+CUSD?");
//  delay(12000);
  String info = ShowSerialData();
   Serial.print(info);
//    lcd.begin(16, 2);  
 if(info.indexOf("+CUSD: 0,\"")==-1){lcd.print("Error with USSD");mode="EDITNUM";}else{
 
  info=info.substring(info.indexOf("+CUSD: 0,\"")+10);
  if(info.indexOf("\"")!=-1){
    info=info.substring(0,info.indexOf("\""));
  }
//  Serial.print(info);
  lcd.print(info); 
 }
}


void CancelVoiceCall(){
    mySerial.println("AT");
    delay(200);
      mySerial.println("ATH");
      delay(100);
  mySerial.println();
  lcd.begin(16,2);
  lcd.print("CALL ENDED");
  mySerialflush();
  delay(2000);
  mode="MAIN";
}

void AnswerVoiceCall(){
    mySerial.println("AT");
    delay(200);
      mySerial.println("ATA");
      delay(100);
  mySerial.println();
  mySerialflush();
  delay(500);
  mode="MAIN";
}
 
void ShowSerialData2(){
  while(mySerial.available()!=0)
  Serial.write(mySerial.read());
}

String ShowSerialData(){
 unsigned long previous;
 char response[200];
  int x=0;
  while(mySerial.available()!=0){
    response[x++]=mySerial.read();
  }
  
  return response;
}

void numInput(char key){
  if(key!='#'&&key!='*'&&key!='!'&&key!='?'&&key!='U'&&key!='L'&&key!='D'&&key!='R'&&key!='0'){
    editNum=editNum+String(key);
//    lcd.clear();  
//    lcd.print(editNum);
//  lcd.cursor();
  }
  if(key=='U'){
    editNum=editNum+"+";
//    lcd.clear();  
//    lcd.print(editNum);
//    lcd.cursor();
  }
  if(key=='?'){
    editNum="";
    mode="MAIN";
//    lcd.clear();  
//    lcd.print("");
  }
    if (key == 'L') {
      editNum = editNum.substring(0, editNum.length() - 1);
//    lcd.clear();  
//    lcd.print(editNum);
//    lcd.cursor();
  }
  if (key == '*' || key == '#') {
    editNum = editNum+String(key);
  }

}

void mySerialpowerOn(){
  mySerialflush();
  // Test command
  mySerial.println("AT");
  mySerial.println();
  delay(500);
  // Power on if no response
  if(mySerial.available()==0){
    Serial.print("was off");
    // Emulate button click
    digitalWrite(9, HIGH);
    delay(1000);//was 1000
    digitalWrite(9, LOW);
    // Wait network connection
    delay(5000);
    // Turn on caller ID notification
    mySerial.println("AT+CLIP=1\r");
    mySerial.println();
    delay(500);
  }else{
    Serial.print("was on");
  }
    
  mySerialflush();
}
/*
void mySerialpowerOn(){
  mySerialflush();
  // Test command
  mySerial.println("AT+CLIP=1\r");
  delay(500);
  // Power on if no response
  if(mySerial.available()==0){
    // Emulate button click
    digitalWrite(9, HIGH);
    delay(1500);//was 1000
    digitalWrite(9, LOW);
    // Wait network connection
    delay(5000);
    // Turn on caller ID notification
    mySerial.println("AT+CLIP=1\r");
    delay(500);
  }
  mySerialflush();
}
*/

void mySerialflush(){
while(mySerial.available()!=0){
mySerial.read();
}
}



void keypadEvent(KeypadEvent key){
  if(mode=="EDITNUM"||mode=="SENDTEXT"){
  if(keypad.getState()==HOLD && key=='0'){
      editNum = editNum+"+";
    lcd.clear();  
      lcd.print(editNum);
      lcd.cursor();
  }else if(keypad.getState()==RELEASED && key=='0'){
    if(millis()-lastKeyPressTime<1000){
      editNum = editNum+"0";
    lcd.clear();  
      lcd.print(editNum);
      lcd.cursor();
    }
  }
  }
}

String getTimeHM(){
  String info=getTime();
  
  String info2=info.substring(info.indexOf(",")-8,info.indexOf(","));
  info2=info.substring(6,8)+"."+info.substring(3,5)+"."+info.substring(0,2);

  info=info.substring(info.indexOf(",")+1);
  info=info.substring(0,info.lastIndexOf(":"));
  return info+"  "+info2;
}

/*String getTimeDMY(){
  String info=getTime();
  info=info.substring(0,info.indexOf(","));
  return info;
}*/

String getTime(){
  mySerial.println("AT");
  delay(1000);
mySerialflush();
  mySerial.println("AT+CCLK?");
  delay(500);
  mySerial.println();
  
  String info = ShowSerialData();
  int firstindex = info.indexOf("+CCLK: \"");
  if(firstindex==-1){return "";}
  info=info.substring(firstindex+8);
  info=info.substring(0,info.indexOf("\""));
  return  info;
}


int getSignal(){
        mySerial.println("AT");
    delay(200);
  mySerial.println("AT+CSQ");
      delay(200);
  mySerial.println();
  delay(2000);
  String info = ShowSerialData();
  int firstindex = info.indexOf("+CSQ: ");
  if(firstindex==-1){return 5;}
  int lastindex = info.indexOf(",");
  if(info=="99"){return 99;}
  String signalstr = info.substring(firstindex+6,lastindex);
 // Serial.print("Signal: "+signalstr);
  int siggg = round(4*signalstr.toInt()/32);
  if(siggg>4){
    return 4;
  }else{
    return siggg;
  } 
}


int SIM900ringing(){
  char aux[100];
  char msg[100];
  int i=0;
  
  if(mySerial.available()!=0){
    while(mySerial.available()!=0){
      msg[i++] = mySerial.read();
    }
    String msg2=String(msg);
    if(msg2.indexOf("RING")!=-1){
      if(msg2.indexOf("+CLIP: \"")==-1||msg2.indexOf("\",")==-1){
        incNum="Unkown Caller";
      }else{
        incNum=msg2.substring(msg2.indexOf("+CLIP: \"")+8,msg2.indexOf("\","));
      }
      return 1;
    }
  }
  return 0;
}


/*
int SIM900ringing(){
  char msg[100];
  int i=0;
  if(mySerial.available()!=0){
    delay(2000);
  
    while(mySerial.available()!=0){
      msg[i++] = mySerial.read();
    }
    String msg2=String(msg);
    if(msg2.indexOf("RING")!=-1){
      return 1;
    }
  }
  return 0;
}
*/
