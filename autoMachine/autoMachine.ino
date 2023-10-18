#include <string.h>

#include <avr/eeprom.h>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

byte customChar1[8] = {//бувка Ж
  B10101,
  B10101,
  B10101,
  B01110,
  B01110,
  B10101,
  B10101,
  B10101
};

byte customChar2[8] = {//бувка Л
  B00111,
  B00101,
  B00101,
  B00101,
  B01001,
  B01001,
  B01001,
  B10001
};

byte customChar3[8] = {//бувка Г
  B11111,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte customChar4[8] = {//бувка Ч
  B10001,
  B10001,
  B10001,
  B01111,
  B00001,
  B00001,
  B00001,
  B00001
};

#include <GyverEncoder.h>
#define CLK A2
#define DT A3
#define SW A1
Encoder enc(CLK, DT, SW);

#define MainMenu 0
#define SettingMenu 1
#define SettingEdition 2

#define C1 2 //первый цилиндр, отвечающий за подем
#define C2 3 //второй цилиндр, отвечающий за трафаретную раму
#define C3 4 //третий цилиндр, отвечающий за нанисение краски
#define C4 5 //четвертый цилиндр, отвечающий за отвод краски
#define k1 6 //первый ключ, отвечающий за положение первого цилиндра 
#define k2 7 //второй ключ, отвечающий за положения трафаретной рамы отведеной до конца
#define k3 8 //третий ключ, отвечающий за положения трафаретной рамы в исходном положение
#define k4 9 //педаль для начинания цыкла автомизации
#define k5 10 //для миносовки тиража 

unsigned long int GeneralCounter, EditionCounter = 0, Counterdown = 0;
unsigned short int MenuSettingsCursor = 0, SelectMenu = 0, MenuCursor = 0;
bool flag1, flag2, flag3, flag4, CounterDownFlag = true;

void return_the_initial_state()
{
  Serial.println("return_the_initial_state");
  flag1 = false;
  flag2 = false;
  flag3 = false; 
  flag4 = false;
  digitalWrite(C3, LOW);
  digitalWrite(C4, LOW);
  digitalWrite(C2, LOW);
  digitalWrite(C1, LOW);
  lcd.clear();
  lcd.print("wait 3 key");
  while(1)
  {
    Serial.println ("while");
    delay(2000);//==================================================================
    if(digitalRead(k3) == 0)
    {
      Serial.println ("if");
      break;
    }
  }
}

void auto_mode()
{
  return_the_initial_state();
  digitalWrite(C1, HIGH);
  do{
    delay(1000);//==================================================================
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("first mov 1");
  }while(digitalRead(k1));//
  digitalWrite(C3, HIGH);
  delay(1000);
  do{
    delay(1000);//==================================================================
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Second mov 2");
    digitalWrite(C2, HIGH);
  }while(!digitalRead(k2));//
  digitalWrite(C3, LOW);
  digitalWrite(C4, HIGH);
  do{
    delay(1000);//==================================================================
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Third mov 3");
    digitalWrite(C2, LOW);
    digitalWrite(C1, LOW);
  }while(digitalRead(k3));//
  GeneralCounter++;
  eeprom_update_block((void*)&GeneralCounter,0,sizeof(GeneralCounter));
  if(!CounterDownFlag)Counterdown--;
  else EditionCounter++;
  return_the_initial_state();
}

void port_setting()
{
  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(C4, OUTPUT);
  pinMode(k1, INPUT);
  pinMode(k2, INPUT);
  pinMode(k3, INPUT);
  pinMode(k4, INPUT);
  pinMode(k5, INPUT);
}

//остаеться доделать главную логику настройки тиража
//доделать прорисовку меню настройки тиража, дальше отладка

void main_logic()
{
  if(SelectMenu == MainMenu)
  {
    if(MenuSettingsCursor == 1)
    {
      Serial.println ("MainMenu butt");
      SelectMenu = SettingMenu;
      MenuSettingsCursor = 0;
      draw_menu(SettingMenu);
    }
  }
  else if(SelectMenu == SettingMenu)
  {
    switch(MenuSettingsCursor)
    {
      case 0:
      SelectMenu = MainMenu; 
      MenuSettingsCursor = 0;
      draw_menu(MainMenu);
      break;

      case 1:
      Serial.println ("case 1");
      flag1 = !flag1;
      digitalWrite(C1, flag1);
      break;

      case 2:
      Serial.println ("case 2");
      flag2 = !flag2;
      digitalWrite(C2, flag2);
      break;

      case 3:
      Serial.println ("case 3");
      flag3 = !flag3;
      digitalWrite(C3, flag3);
      break;

      case 4:
      Serial.println ("case 4");
      flag4 = !flag4;
      digitalWrite(C4, flag4);
      break;

      case 5:
      Serial.println("SettingEdition");
      Serial.println ("case 5");
      SelectMenu = SettingEdition; 
      MenuSettingsCursor = 0;
      draw_menu(SelectMenu);
      break;

      case 6:
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print(GeneralCounter);
      delay(5000);
      draw_menu(SettingMenu);
      break;

      default:
      SelectMenu = MainMenu; 
      draw_menu(MainMenu);
      MenuSettingsCursor = 0;
      break;
    }
  }

  else if(SelectMenu == SettingEdition)
  {
    switch(MenuSettingsCursor)
    {
      case 0:     
      SelectMenu = SettingMenu;
      MenuSettingsCursor = 0;
      draw_menu(SettingMenu); 
      break;


      case 1:
      SelectMenu = -1;
      draw_menu(-1);
      break;


      case 2:
      CounterDownFlag = !CounterDownFlag;
      draw_menu(SettingEdition);
      break;
    }
  }
  else{
      Serial.println("SettingEdition");
      Serial.println ("case 5");
      SelectMenu = SettingEdition; 
      MenuSettingsCursor = 0;
      draw_menu(SelectMenu);
  }
}

void draw_menu(short int select_page)
{
  if(select_page == MainMenu)
  {
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("ABTO PE");
    lcd.write(byte(0));
    lcd.print("M");
    lcd.setCursor(11, 0);
    if(CounterDownFlag)
    {
      lcd.print(EditionCounter);
    }
    else{
      lcd.print(Counterdown);
    }
    lcd.setCursor(0, 1);
    lcd.print("HA");
    lcd.write(byte(1));
    lcd.print("AWTYB");
    lcd.setCursor(9, 0);
  }
  else if(select_page == SettingMenu)
  {
    lcd.setCursor(0, 0);
    lcd.clear();
    switch(MenuSettingsCursor)
    {
      case 0:
      lcd.print("HA3AD");
      lcd.setCursor(0, 1);
      lcd.print("HAC1 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      break; 
      case 1:
      lcd.print("HAC1 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      lcd.setCursor(0, 1);
      lcd.print("HAC2 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      break;            
      case 2: 
      lcd.print("HAC2 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      lcd.setCursor(0, 1);
      lcd.print("HAC3 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      break;
      case 3: 
      lcd.print("HAC3 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      lcd.setCursor(0, 1);
      lcd.print("HAC4 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      break;
      case 4: 
      lcd.print("HAC4 BK");
      lcd.write(byte(1));
      lcd.print("BUK");
      lcd.write(byte(1));
      lcd.setCursor(0, 1);
      lcd.print("HA");
      lcd.write(byte(1));
      lcd.print("AWTYB TP");
      lcd.write(byte(0));
      break;
      case 5: 
      lcd.print("HA");
      lcd.write(byte(1));
      lcd.print("AWTYB TP");
      lcd.write(byte(0));
      lcd.setCursor(0, 1);
      lcd.write(byte(2));
      lcd.write(byte(1));
      lcd.print("AB ");
      lcd.write(byte(1));
      lcd.print("I");
      lcd.write(byte(3));
      break;
      case 6: 
      lcd.write(byte(2));
      lcd.write(byte(1));
      lcd.print("AB ");
      lcd.write(byte(1));
      lcd.print("I");
      lcd.write(byte(3));
      lcd.setCursor(0, 1);
      lcd.print("HA3AD");
      break;
    }
    lcd.setCursor(15, 0);
    //lcd.print(MenuItems[MenuSettingsCursor]);
    //lcd.setCursor(0, 1);
    //if(MenuSettingsCursor == 6){
    //  lcd.print(MenuItems[0]);
    //}
    //else lcd.print(MenuItems[MenuSettingsCursor+1]);
    //lcd.setCursor(15, 0);
  }
  else if(select_page == SettingEdition)
  {
    Serial.println(MenuSettingsCursor);
    lcd.setCursor(0, 0);
    lcd.clear();
    switch(MenuSettingsCursor)
    {
      case 0:
      lcd.print("HA3AD");
      lcd.setCursor(0, 1);
      lcd.print("BCTH ");
      lcd.write(byte(1));
      lcd.print("I");
      lcd.write(byte(3));
      break;
      case 1:
      lcd.print("BCTH ");
      lcd.write(byte(1));
      lcd.print("I");
      lcd.write(byte(3));
      lcd.setCursor(0, 1);
      lcd.print("BCTH BID/DO ");
      lcd.print(CounterDownFlag);
      break;
      case 2:
      lcd.print("BCTH BID/DO ");
      lcd.print(CounterDownFlag);
      lcd.setCursor(0, 1);
      lcd.print("HA3AD");
      break;
    }
    lcd.setCursor(15, 0);
    //lcd.print(MenuItemsSettingsEdition[MenuSettingsCursor]);
    //lcd.setCursor(0, 1);
    //if(MenuSettingsCursor == 2){
    //  lcd.print(MenuItemsSettingsEdition[0]);
    //}
    //else lcd.print(MenuItemsSettingsEdition[MenuSettingsCursor+1]);
    //lcd.setCursor(15, 0);
  }
  else
  {
    lcd.clear();
    lcd.print(Counterdown);
  }
  lcd.blink();
}

void move_cursor()
{
  if(SelectMenu == MainMenu)
  {
    if(enc.isRight())
    {
      MenuSettingsCursor++;
      lcd.setCursor(9, 1);
      if(MenuSettingsCursor > 1) MenuSettingsCursor = 0;
    }
    else if(enc.isLeft())
    {
      MenuSettingsCursor--;
      lcd.setCursor(9, 0);
      if(MenuSettingsCursor > 1) MenuSettingsCursor = 1;
    }
    Serial.println(MenuSettingsCursor);
    Serial.println("MainMenu");
    return; 
  }

  if(SelectMenu == SettingMenu)
  {
    lcd.setCursor(15, 0);
    Serial.println("move_cursor");
    if(enc.isRight())
    {
      MenuSettingsCursor++;
      if(MenuSettingsCursor > 6) lcd.setCursor(15, 1);
      if(MenuSettingsCursor > 6)MenuSettingsCursor = 0;
    }
    else if(enc.isLeft())
    {
      MenuSettingsCursor--;
      if(MenuSettingsCursor < 7) lcd.setCursor(15, 0);
      if(MenuSettingsCursor > 7)MenuSettingsCursor = 6;
    }
    Serial.println(MenuSettingsCursor);
    draw_menu(SettingMenu);
    return; 
  }

  if(SelectMenu == SettingEdition)
  {
    lcd.setCursor(15, 0);
    Serial.println("move_cursor");
    if(enc.isRight())
    {
      MenuSettingsCursor++;
      if(MenuSettingsCursor > 2)MenuSettingsCursor = 0;
    }
    else if(enc.isLeft())
    {
      MenuSettingsCursor--;
      if(MenuSettingsCursor > 2)MenuSettingsCursor = 2;
    }
    draw_menu(SettingEdition);
    return; 
  }
  
  if(SelectMenu == -1)
  {
    if(enc.isRight())
    {
      Counterdown++;
      if(Counterdown > 10000)Counterdown = 0;
    }
    else if(enc.isLeft())
    {
      Counterdown--;
      if(Counterdown > 10000)Counterdown = 10000;
    }
    draw_menu(-1);
  }
}

void setup() {
  lcd.init();
  lcd.backlight();
  port_setting();
  enc.setType(TYPE2);
  lcd.createChar(0, customChar1);
  lcd.createChar(1, customChar2);
  lcd.createChar(2, customChar3);
  lcd.createChar(3, customChar4);
  eeprom_read_block((void*)&GeneralCounter,0,sizeof(GeneralCounter));

  draw_menu(MainMenu);
}

void loop() {
  enc.tick();

  if(enc.isTurn()){
    move_cursor();
  }
  if(enc.isPress()){
    main_logic();
  }

  if(!digitalRead(k4))
  {
    Serial.println(digitalRead(k4)); 
    if(SelectMenu==MainMenu && MenuSettingsCursor==0){
      auto_mode();
      draw_menu(MainMenu);
    }
  }

  if(!digitalRead(k5))
  {
    if(SelectMenu==MainMenu)
    {
      if(CounterDownFlag)
      {
        Serial.println(CounterDownFlag); 
        EditionCounter--;
      }
      else{
        Serial.println(CounterDownFlag); 
        Counterdown++;
      }
      draw_menu(MainMenu);
    }
  }
}
