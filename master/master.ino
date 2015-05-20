#include <SoftwareSerial.h>
#include <SD.h>
SoftwareSerial xbee(3, 4) ;

char space = ' ' ;
char alpa[4] = {
  'A', 'B', 'C', 'D'} 
;
byte check, last ;
File data ;
double val[3] = {0, } ;

void setup()
{
  xbee.begin(57600) ;
  pinMode(10, OUTPUT) ;
  if (!SD.begin(10)) {
    while(1) {
    }
  }
  delay(3000) ;
  xbee.write(0xBB) ;
}

void loop()
{
  data = SD.open("data.txt", FILE_WRITE) ;
  if (data) {
    while(1) {
      while(!(xbee.available() >= 8)) ;
      String test = get_data() ;
      if (last == check && test != "fail") {
        test += "  time : " + String(millis()) ;
        data.println(test) ;
        break ;
      }
    }
  }
  data.close() ;  
}

String get_data()
{
  String dataString = "" ;
  byte where = xbee.read() ;
  char temp[6] ;
  check = where ;
  if (where >= 0x0A && where <= 0x0D) dataString = String(alpa[where - 10]) ;
  else {
    xbee.flush() ;
    return "fail";
  }
  for (int i = 0; i < 3; i++) {
    val[i] = 0 ;
    byte tem = xbee.read() ;
    check ^= tem ;
    
    val[i] = tem >> 4 ;
    val[i] += (tem & 0x0F) / 10. ;
    
    tem = xbee.read() ;
    check ^= tem ;
    
    val[i] += (tem >> 4) / 100. ;
    
    if ((tem & 0x0F) == 0x02) val[i] *= -1 ;
    else if ((tem & 0x0F) != 0x01) {
      xbee.flush() ;
      return "fail" ;
    }
    dataString += String(dtostrf(val[i], 6, 2, temp)) ;
    if (i == 2) {
      last = xbee.read() ;
      return dataString ;
    }
  }
}
