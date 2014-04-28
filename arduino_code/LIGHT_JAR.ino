#include "SPI.h"
#include <WiFi.h>
#include <math.h>
#include "Adafruit_WS2801.h"
  
// Global variables for Wifi Connection
char ssid[] = "Sally"; // network SSID
char pass[] = "03230323";  // network password

int status = WL_IDLE_STATUS;
WiFiServer server(80);

String HTTP_req;          // stores the HTTP request
String HTTP_req_path;          // stores the HTTP HTTP_req_path

String light_status = "off"; // off, on, show
uint32_t light_color = Color(255,255,255);
int light_brightness = 255;
String light_show = "null"; // null, breathe, rainbow_static, rainbow_sync, rainbow_cycle
int light_show_speed = 30;

// Global variables for RGB LED Stripe
uint8_t dataPin  = 2;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 3;    // Green wire on Adafruit Pixels

// Set the first variable to the NUMBER of pixels. 25 = 25 pixels in a row
Adafruit_WS2801 strip = Adafruit_WS2801(25, dataPin, clockPin);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }
  strip.begin();
  // Update LED contents, to start they are all 'off'
  strip.show();
  
  // check for the presence of the wifi-shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present"); 
    while(true);
  }
  
  // attempt to connect to Wifi network
  colorSolid(Color(0, 0, 100));
  //colorSolid(Dim(Color(0, 0, 100), light_brightness));
  while ( status != WL_CONNECTED) { 
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    
    // wait 10 seconds for connection
    delay(10000);
  }
  //colorSolid(Dim(Color(0, 100, 0), light_brightness));
  light_status = "on";
  server.begin();
  printWifiStatus();    
}

void loop() {
  if(light_status == "off") {
    colorSolid(Color(0, 0, 0));  
  } else if(light_status == "on") {
    colorSolid(Dim(light_color,light_brightness));  
  } else if(light_status == "show") {
    playShow(light_show);
  }

  WiFiClient client = server.available(); // try to get client
  if (client) { // got client?
    Serial.println("new client");
    HTTP_req = getHttpRequestString(client);
    HTTP_req_path = getRequestPath(HTTP_req);

    // change Light Status from Request Data
    if(HTTP_req_path.length() > 0)  {
      setLightStatus(HTTP_req_path);     
    }
    // send response to client
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    
    // send web page
    //client.print("<html><h1>hello</h1></html>");
    /*
    client.print("{ light_color: \"");
    client.print(String(light_color, HEX));
    client.print("\", light_status: \"");
    client.print(light_status);
    client.print("\", light_brightness: ");
    client.print(light_brightness);
    client.print(", light_show: \"");
    c    client.print("\" }");n 
    */

    HTTP_req = "";
    HTTP_req_path = ""; 
        
    delay(10);  // give the web browser time to receive the data
    client.stop(); // close the connection
    Serial.println("client disonnected");  
    /* server - end */
    Serial.println("{ light_color: \"");
    Serial.println(String(light_color, HEX));
    Serial.println("\", light_status: \"");
    Serial.println(light_status);
    Serial.println("\", light_brightness: ");
    Serial.println(light_brightness);
    Serial.println(", light_show: \"");  }
  // Some example procedures showing how to display to the pixels
  //colorSolid(Color(0, 0, 255));
  //colorSolid(Dim(Color(0, 0, 255),10));
  //colorWipe(Color(255, 0, 0), 50);
  //colorWipe(Color(0, 255, 70), 50);
  //rainbow(30);
  //rainbowCycle(10);
}

void breathe(uint32_t color, uint8_t wait) {
  int i = 0, lower_bound = 10;
  double t=0;
  double speed = wait/30;
  //pow (2.7,sin(t)) // x to power of y
 for( t=0 ; t<2*M_PI ; ) {
   int brightness_value = (int) map(pow(2.7,sin(t))*10000, 3703, 26999, 0, 255);
   
    colorSolid(Dim(Dim(color,(int) map(pow(2.7,sin(t))*10000, 3703, 26999, 0, 255)), light_brightness));
    //Serial.print("pow : ");Serial.println(pow(2.7,sin(t))*10000);
    //Serial.print("after mapping : ");Serial.println((int) map(pow(2.7,sin(t))*10000, 3703, 26999, 0, 255));
     if((brightness_value < 20) || (brightness_value > 235)) {
       t += 0.009 * speed;
     } else {
       t += 0.006 * speed;
     }
    delay(1);  
 }
  /*
  for(i = lower_bound; i< 255 ; i++){
    colorSolid(Dim(Dim(color,(int) pow(2.7,sin(i))), light_brightness));
    delay(1);
  }
  delay((wait/3)*40);
  for(; i > lower_bound ; i--){
    colorSolid(Dim(Dim(color,(int) pow(2.7,sin(i))), light_brightness));
    delay(1);
  }
  */
}

void rainbow(uint8_t wait) {
  int i, j;
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Dim(Wheel( (i + j) % 255), light_brightness));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}


// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Dim(Wheel( ((i * 256 / strip.numPixels()) + j) % 256), light_brightness));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// solid Color Setting
void colorSolid(uint32_t c) {
  int i;
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t Dim(uint32_t c, int brightness) {
  uint32_t c_temp_r=c;
  uint32_t c_temp_g=c;
  uint32_t c_temp_b=c;
  byte r = (c_temp_r >> 16);
  byte g = (c_temp_g >> 8);
  byte b = (c_temp_b);  
  int r_int = (int) r * (brightness/256.0);
  int g_int = (int) g * (brightness/256.0);
  int b_int = (int) b * (brightness/256.0);
  
  return Color(r_int, g_int, b_int);
}


//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

// Functions for WiFi
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

String getRequestPath(String HTTP_req) {
  String HTTP_req_path = "";
  int first_space_idx = HTTP_req.indexOf(" ");
  int second_space_idx = HTTP_req.indexOf(" ",first_space_idx+1);
  HTTP_req_path = HTTP_req.substring(first_space_idx+1,second_space_idx);
  Serial.println("--- Incoming HTTP REQUEST --- \n"+HTTP_req);
  Serial.println("----- paht : " + HTTP_req_path);
  return HTTP_req_path;
}
// 클라이언트의 요청이 들어오면 클라이언트 스트림에서 요청 문자열을 읽어 완성한다.
// getHttpRequestString()
String getHttpRequestString(WiFiClient client) {
  String result;
  //boolean currentLineIsBlank = true;
  while(true) {
    if(client.available()) {
      char c = client.read();  // read 1 byte (character) from client
      //Serial.print(c);
      if (c == '\n') {
        break;
      }
      result = result + c;
      /*
      if (c == '\n' && currentLineIsBlank) {
        break;
      }
      if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
      */
    }
  }
  while(true) {
    if(client.available()) {
      char c = client.read();
    } else {
      break;
    }
  }
  return result;
}

// Function for Light Jar
void playShow(String show_title) {
  if(show_title == "rainbow_sync") {
    rainbow(light_show_speed);
  } else if (show_title == "rainbow_cycle") {
    rainbowCycle(light_show_speed);
  } else if (show_title == "breathe") {
    breathe(light_color, light_show_speed);
  } else if (show_title == "siren") {
    siren(light_color, light_show_speed);
  } 
}

void setLightStatus(String requestPath) {
  if (HTTP_req_path.indexOf("/api/turn_on_light") > -1) {
    light_status = "on";
  } else if (HTTP_req_path.indexOf("/api/turn_off_light") > -1) {
    light_status = "off";
  } else if (HTTP_req_path.indexOf("/api/blink_light") > -1) {
    if(light_status != "off") {
      int blink_count;
      uint32_t previous_color = light_color;
      if(HTTP_req_path.indexOf("change_color/") > -1) {
        String new_color_path = HTTP_req_path.substring(HTTP_req_path.indexOf("change_color/"));     
        change_color(new_color_path);   
      }
      if(HTTP_req_path.indexOf("blink_light/") > -1) {
        int first_idx = HTTP_req_path.indexOf("_light/") + 7;
        String blink_count_s = HTTP_req_path.substring(first_idx);
        blink_count_s = blink_count_s.substring(0,blink_count_s.indexOf("/"));
        //Serial.println(blink_count_s);
        blink_count = blink_count_s.toInt();
      } else {
        blink_count = 1;
      }
      blink(blink_count);
      light_color = previous_color;
    }
  } else if (HTTP_req_path.indexOf("/api/dim_light/") > -1) {
    if(light_status != "off") {
      int first_idx = 15;
      String new_brightness_s = HTTP_req_path.substring(first_idx);
      int new_brightness = new_brightness_s.toInt();
      if(new_brightness >= 0 && new_brightness <= 255) {
        light_brightness = new_brightness;
      }  
    }
  } else if (HTTP_req_path.indexOf("/api/set_show_speed/") > -1) {
    if(light_status != "off") {
      int first_idx = 20;
      String new_speed_s = HTTP_req_path.substring(first_idx);
      int new_speed = new_speed_s.toInt();
      if(new_speed >= 0 && new_speed <= 255) {
        light_show_speed = new_speed;
      }  
    }
  } else if (HTTP_req_path.indexOf("/api/change_color/") > -1) {
    if(light_status == "on" || (light_status == "show" && ( light_show == "breathe" || light_show == "siren" ))) {
      change_color(HTTP_req_path); 
    }
  } else if (HTTP_req_path.indexOf("/api/turn_on_rainbow/sync") > -1) {
    if(light_status != "off") { 
      light_status = "show";
      light_show = "rainbow_sync";
    } 
  } else if (HTTP_req_path.indexOf("/api/turn_on_rainbow/cycle") > -1) {
    if(light_status != "off") { 
      light_status = "show";
      light_show = "rainbow_cycle";
    } 
  } else if (HTTP_req_path.indexOf("/api/turn_on_breathe") > -1) {
    if(light_status != "off") { 
      light_status = "show";
      light_show = "breathe";
    } 
  } else if (HTTP_req_path.indexOf("/api/turn_off_show") > -1) {
    if(light_status == "show") { 
      light_status = "on";
      light_show = "null";
    } 
  } else if (HTTP_req_path.indexOf("/api/change_to_random_color") > -1) {
    if(light_status == "on" || (light_status == "show" && ( light_show == "breathe" || light_show == "siren" ))) {
      light_color = Color(random(10, 240), random(10, 240), random(10, 240));
    } 
  } else if (HTTP_req_path.indexOf("/api/turn_on_siren") > -1) {
    if(light_status != "off") { 
      light_status = "show";
      light_show = "siren";
    } 
  }     
  Serial.print("status : ");
  Serial.println(light_status);   
  Serial.print("color : ");
  Serial.println(String(light_color, HEX));
  Serial.print("brightness : ");
  Serial.println(light_brightness); 
  Serial.print("show : ");
  Serial.println(light_show);  
}

void change_color(String path_str) {
  // extract_color_string
  int first_idx = path_str.indexOf("change_color") + 13; 
  String color_string = path_str.substring(first_idx);
  // Serial.println("color_string : "+color_string );  
  if(color_string.length() >= 5) {
    int first_slash_idx = color_string.indexOf("/");
    int second_slash_idx = color_string.indexOf("/",first_slash_idx+1);
    int third_slash_idx = color_string.indexOf("/",second_slash_idx+1);
    
    String red_value_s = color_string.substring(0,first_slash_idx);
    String green_value_s = color_string.substring(first_slash_idx+1,second_slash_idx);
    String blue_value_s;
    if(third_slash_idx != -1) {
      blue_value_s = color_string.substring(second_slash_idx+1);
    } else {
      blue_value_s = color_string.substring(second_slash_idx+1,third_slash_idx);
    }
    
    int red_value = red_value_s.toInt();
    int green_value = green_value_s.toInt();
    int blue_value = blue_value_s.toInt();
    
    if((red_value >= 0 && red_value <= 255)&&(green_value >= 0 && green_value <= 255)&&(blue_value >= 0 && blue_value <= 255)) {
      /*
      Serial.println("red_value : "+red_value_s );
      Serial.println("green_value : "+green_value_s );
      Serial.println("blue_value : "+blue_value_s );
      */
      light_color = Color(red_value, green_value, blue_value);
    }  
  }
}

void blink(int blink_count) {
  int i;
  for(i=0;i<blink_count;i++) {
    colorSolid(Color(0,0,0));
    delay(500);
    colorSolid(Dim(light_color,light_brightness));
    delay(500);
  }
}

void siren(uint32_t c, uint8_t wait) {
  //colorSolid(Color(0, 0, 0));
  int i,j;
  uint32_t b = Color(0, 0, 0);
  /* two line solid */
  /*
  for(i=0;i<6;i++) {
    for(j=0;j<25;j++) {
      if(j/4==i) {
        strip.setPixelColor(j, c);
      } else {
        if(j >= 21) {
          if((j-24+4)/4==i) {
            strip.setPixelColor(j, c);  
          } else {
            strip.setPixelColor(j, b);
          }         
        } else {
          if((j+4)/4==i) {
            strip.setPixelColor(j, c);  
          } else {
            strip.setPixelColor(j, b);
          }
        }
      }    
    }
    strip.show();
    delay(80); 
  }
  */
  
  /* on line solid */
  /*
  for(i=0 ; i < 6 ; i++) {
    for (j=0 ; j < 25 ; j++) {
      if(j/4==i) {
        strip.setPixelColor(j, c);  
      } else {
        strip.setPixelColor(j, b);
      }
    }
    strip.show();
    delay(80); 
  }
  */
  /* three line gradient */ 
   for(i=0 ; i < 6 ; i++) {
    for (j=0 ; j < 25 ; j++) {
      if(j/4==i) {
        strip.setPixelColor(j, Dim(c, light_brightness));  
      } else {
        if(j >= 21) {
          if((j-24+4)/4==i) {
            strip.setPixelColor(j, Dim(Dim(c,190), light_brightness));  
          } else if((j-24-4)/4==i) {
            strip.setPixelColor(j, Dim(Dim(c,190), light_brightness));  
          } else {
            strip.setPixelColor(j, b);
          }         
        } else {
          if((j+4)/4==i) {
            strip.setPixelColor(j, Dim(Dim(c,190), light_brightness));  
          } if((j-4)/4==i) {
            strip.setPixelColor(j, Dim(Dim(c,190), light_brightness));  
          } else {
            strip.setPixelColor(j, b);
          }
        }
      }
    }
    strip.show();
    delay(wait); 
  } 
}

