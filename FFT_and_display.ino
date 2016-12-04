#define LOG_OUT 1 // use the log output function
#define FHT_N 32 // set to 256 point fht

#include <FHT.h> // include the library
#include "U8glib.h"


//U8GLIB_SH1106_128X64 u8g(13, 11, 10, 9, 8);
U8GLIB_SH1106_128X64 u8g(4, 5, 6, 7, 8);  // SW SPI Com: SCK = 4, MOSI = 5, CS = 6, A0 = 7 (new blue HalTec OLED)

void setup(void) {
  Serial.begin(115200);
  // flip screen, if required
  // u8g.setRot180();
  
  // set SPI backup if required
  //u8g.setHardwareBackup(u8g_backup_avr_spi);


  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  }
  else if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  }
  else if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  }
  else if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }

  //Serial.begin(115200); // use the serial port
  TIMSK0 = 0; // turn off timer0 for lower jitter
  ADCSRA = 0xe5; // set the adc to free running mode
  ADMUX = 0x40; // use adc0
  DIDR0 = 0x01; // turn off the digital input for adc0
}
//
//void draw(void) {
//  // graphic commands to redraw the complete screen should be placed here  
//  u8g.setFont(u8g_font_unifont);
//  //u8g.setFont(u8g_font_osb21);
//  //u8g.drawStr( 0, 22, "Hello World!");
//
//  
//}

int heights[16] = {0,0,0,0,0,0,0,0};

void render(void) {
  // picture loop
  Serial.println("start");
  u8g.firstPage();  
  do {
    
    for (byte i = 0 ; i < FHT_N/2 ; i++) {

  
  Serial.println(fht_log_out[i]/4);

      int base[16] = {57,54,32,26,22,19,17,14,13,12,9,9,6,4,4,0};

      const int width = 17;
      int left = (i * 8) + 1;
      int height = fht_log_out[i] / 4;
      height = height - base[i];
      if (height <= 0) {
        height = 1;  
      }

      if (height >= heights[i]) {
        heights[i] = height;
      }
      else {
        heights[i]--;
      }
 
//      Serial.println(i);
//      
//      Serial.println("A");
      u8g.drawBox(left, 64-heights[i],6 ,heights[i]);
    }
    
    
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
}

void loop(void) {


  while(1) { // reduces jitter
    cli();  // UDRE interrupt slows this way down on arduino1.0
    for (int i = 0 ; i < FHT_N ; i++) { // save 256 samples
      while(!(ADCSRA & 0x10)); // wait for adc to be ready
      ADCSRA = 0xf5; // restart adc
      byte m = ADCL; // fetch adc data
      byte j = ADCH;
      int k = (j << 8) | m; // form into an int
      k -= 0x0200; // form into a signed int
      k <<= 6; // form into a 16b signed int
      fht_input[i] = k; // put real data into bins
    }
    fht_window(); // window the data for better frequency response
    fht_reorder(); // reorder the data before doing the fht
    fht_run(); // process the data in the fht
    fht_mag_log(); // take the output of the fht
    sei();
    Serial.println("start");
    

    render();
    delay(300);
    
  }
  
}

