#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2); 

MFRC522 mfrc522(SS_PIN, RST_PIN);
//------------------------------------------------------------
#include <Keypad.h>
  
const byte rows = 4; //số hàng
const byte columns = 4; //số cột
 
int holdDelay = 700; //Thời gian trễ để xem là nhấn 1 nút nhằm tránh nhiễu
int n = 3; // 
int state = 0; //nếu state =0 ko nhấn,state =1 nhấn thời gian nhỏ , state = 2 nhấn giữ lâu
char key = 0;
 
char keys[rows][columns] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'},
};
 
byte rowPins[rows] = {3, 4, 5, 6}; 
byte columnPins[columns] = {A3, A2, A1, A0};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rows, columns);
//------------------------------------------------------------
String pass_id="";
String Password="";
String Orpin="OR PIN: ";
String RFID="";
int firsttime=1;
//-------------------------------------------------------------
void setup() {
  pinMode(2, OUTPUT); 
  pinMode(8, OUTPUT); 
  pinMode(7, OUTPUT); 
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  //-----------------state 1----------------
  lcd.init();                    
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SCAN RFID CARD");
  lcd.setCursor(0,1);
  lcd.print("OR ID: ");
  digitalWrite(2, HIGH);
  // ---------------quẹt thẻ-------------------
  Serial.begin(9600);
  SPI.begin();       
  mfrc522.PCD_Init();
  //-----------------------------------
}
int index=0;
void loop() {
  
  if(firsttime==1){
    PrintLCD("ENTER PIN FFT:","");
		while ( index< 4) {
			char temp = keypad.getKey();

			if ((int)keypad.getState() == PRESSED) {
				if (temp != 0) {
					key = temp;
				}
			}
			if ((int)keypad.getState() == HOLD) {
				state++;
				state = constrain(state, 1, n - 1);
				delay(holdDelay);
			}

			if ((int)keypad.getState() == RELEASED) {
				key += state;
				state = 0;
        index++;
				Password =Password+ key;
				lcd.setCursor(0, 1);
				lcd.print(Password);
			}
		}


		PrintLCD("SET PASSWORD", "SUCCESSFULLY!");
		delay(1000);
		firsttime = 0;
    PrintLCD("SCAN RFID", "FIRST TIME: ");
    while(RFID==""){
       ganbd();
       delay(1000);
    }
    PrintLCD("SET RFID", "SUCCESSFULLY!");
    delay(1000);
    PrintLCD("SCAN RFID CARD", "OR PIN: ");
    Serial.print(Password);
    Serial.println(RFID);
  }
  GD2();
}
//---------------------Giai đoạn 2------------------------------
void GD2(){
  if(theid()==1){
    pass_id="";
    Orpin="OR PIN:";
    GD4();
    return;
  }
  char temp = keypad.getKey();
  if ((int)keypad.getState() ==  PRESSED) {
    if (temp != 0) {
      key = temp;
    }
  }
  if ((int)keypad.getState() ==  HOLD) {
    state++;
    state = constrain(state, 1, n-1);
    delay(holdDelay);
  }
 
  if ((int)keypad.getState() ==  RELEASED) {
    key += state;
    state = 0;
    Orpin+="*";
    pass_id+=key;
    if(key=='*'){
      pass_id="";
      Orpin="OR PIN:          ";
      lcd.setCursor(0,1);
      lcd.print(Orpin);
      Orpin="OR PIN:";
    }
    lcd.setCursor(0,1);
    lcd.print(Orpin);
  }
  if(pass_id.length()==4){
    if(pass_id.equals(Password)){
        pass_id = "";
        Orpin="OR PIN:";
        GD4();
    }
    else{
        lcd.clear();
        lcd.print("Access Denied!");
        //------------nháy 5 lần--------------
        for (int i = 0; i < 5; i++) {
          digitalWrite(2, LOW);
          delay(300);
          digitalWrite(2, HIGH);
          delay(300);
        } 
        //------------------------------
        lcd.clear();
        lcd.print("SCAN RFID CARD");
        lcd.setCursor(0,1);
        lcd.print("OR ID: ");     
    }
    pass_id = "";
    
    Orpin="OR PIN:";
  }
  Serial.print(pass_id);
  delay(100);
}
//------------------------------------------------------
void GD4(){
  
  PrintLCD("Authenticated","Exit by 5 Sec");
  digitalWrite(7, HIGH);
  digitalWrite(2, LOW);
  int wait=5000;

  char temp = keypad.getKey();

  while(wait)
  {
    digitalWrite(8,1);
    delay(100);
    digitalWrite(8,0);
    delay(100);
    wait-=200;
    if (wait%1000==0) 
    {
      lcd.setCursor(8, 1);
	    lcd.print(String(wait/1000));
    }
  }
  lcd.clear();
  lcd.setCursor(0,0); 
  lcd.print("Enter # to");
  lcd.setCursor(0,1);  
  lcd.print("Sign out");
  while(true){
  		char temp = keypad.getKey();
  		if ((int)keypad.getState() ==  PRESSED) {
    		if (temp != 0) {
      				key = temp;
    		}
  		}
  		if ((int)keypad.getState() ==  HOLD) {
    		state++;
    		state = constrain(state, 1, n-1);
    		delay(holdDelay);
  		}
 
  		if ((int)keypad.getState() ==  RELEASED) {
    		key += state;
    		state = 0;
        if (key == '#') 
        {

          break;
        }
        }
  }
  PrintLCD("SCAN RFID CARD", "OR PIN: ");
	digitalWrite(7, LOW);
  digitalWrite(2, 1);
  GD2();
}
void PrintLCD(String s1, String s2) {
	lcd.clear();
	lcd.print(s1);
	lcd.setCursor(0, 1);
	lcd.print(s2);
}
int theid(){
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID tag: ");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();

  if (content.substring(1) == RFID) { // Thay đổi UID ở đây nếu cần
    Serial.println("Truy cập được phép");
    Serial.println();
    delay(3000);
    return 1;
  } else {
    Serial.println("Truy cập bị từ chối");
    delay(3000);
    return 0;
  }
}

void ganbd(){
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  Serial.print("UID tag: ");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message: ");
  content.toUpperCase();
  Serial.print(content.substring(1));
  RFID=content.substring(1);
  
}
