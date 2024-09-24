void OLED() {
  display.clearDisplay();

  if (voltage <= 3.60) {
    display.setTextColor(BLACK, WHITE);
    display.setCursor(42, 32);
    display.println(voltage);
  } else {
    display.setTextColor(WHITE);
    display.setCursor(42, 32);
    display.print (voltage);
    display.print("V");
     
  }
  balkenL = analogRead(potL);
  balkenL = map(balkenL, 0, 4095, 0, 63);
  balkenR = analogRead(potR);  // reads the value of the potentiometer (value between 0 and 1023)
  balkenR = map(balkenR, 4095, 0, 0, 63);

  display.drawRect(0, 0, 2, 63, WHITE);
  display.drawRect(0, 0, 2, balkenL, BLACK);
  display.drawRect(126, 0, 2, 63, WHITE);
  display.drawRect(126, 0, 2, balkenR, BLACK);

//4.Zeile
  display.setTextColor(WHITE);
  display.setCursor(8,  48);
  if (fireMode != 0){display.print(fireMode); display.print("er-Salve");} 
  else if(unlocked == true) {display.setCursor(5,  48); display.print ("Lasse es!!");}
  else                      {display.setCursor(5,  48); display.print ("Laser-Mode");} //Mogel-Mode, Laser-Mode
 

  display.setTextColor(WHITE);
  display.setCursor(90,  0);
  display.println(spinUpTime);

  display.setTextColor(WHITE);
  display.setCursor(4, 0);
  display.print(speedESC);
  display.print("%");

 
if ((voltageTime-500) <= millis()){display.setCursor(4, 16);display.setTextColor(BLACK, WHITE); display.print(" BAT DEAD!");
  display.setTextColor(WHITE);}
  else if((voltageTime-1500) <= millis()){display.setCursor(4, 16); display.print(" BAT LOW");
  
  } else if(unlocked == true){ display.setCursor(4, 16);display.setTextColor(BLACK, WHITE); display.print(" LETHAL!!!");
   display.setTextColor(WHITE);
  }






  display.display();
}
