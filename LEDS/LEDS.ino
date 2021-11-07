#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
WiFiServer server(80);//set web server in port 80

#define NUM_LEDS 300  //number of leds in strip
#define DATA_PIN 14    //pin to communicate wtih strip

CRGB leds[NUM_LEDS];    //the data array of leds

char* ssid = "SSid";      //wifi and password
char* password = "password";

String CurrentReq = "E";  //holds the old command
String NewREQ;        //holds new command if there is one

int MovingPatternSpeed = 0;
int OldPatternTimeSample = 0;//for timing calculations

int MovingSnakeSpeed = 0;
int OldSnakeTimeSample = 0;//for timing calculations

CRGB snake[50];//used for the snake effect, holds the snake
CRGB savedpattern[50];//used for the snake effect, holds the place where the snake has overriden the leds
int snakeposition = 0;//used for the snake effect

void setup() {//this function only runs ones, its used to set things up

  Serial.begin(9600);//serial(usb etc...) communication baud rate
  pinMode(DATA_PIN, OUTPUT);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)//connect to wifi
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("ip address: ");//prints the devices ip address
  Serial.print(WiFi.localIP());
  Serial.print("\nMAC: ");      //prints the devices mac address
  Serial.println(WiFi.macAddress());

  pinMode(2, OUTPUT);     //blinks the led so that the user will know that the device is connected to the wifi
  for (int i = 0; i < 5; i++) {
    digitalWrite(2, HIGH);
    delay (400);
    digitalWrite(2, LOW);
    delay (400);

  }
  digitalWrite(2, LOW);
  pinMode(2, INPUT);


  server.begin();
  Serial.println("\nServer started");

}

//this function runs in a loop
//this is the command type  NewREQ.charAt(0)  , all the rest are parameters
void loop() {
  while (WiFi.status() != WL_CONNECTED)// if diconnected from wifi then retry
  {
    Serial.print(".");
    delay(500);
  }
  CheckClient();
  if (!NewREQ.equals(CurrentReq))//if recieved new commands then its stored in NewReq, and it will be different from the current one which is stored in CurrentReq
  {

    if (CurrentReq.charAt(0) == 'C')//if the old command was a moving snake then stop it, reset the pattern and setup the new command
    {




      if (NewREQ.charAt(0) == 'A' || NewREQ.charAt(0) == 'B' || NewREQ.charAt(0) == 'F' || NewREQ.charAt(0) == 'C' || NewREQ.charAt(0) == 'D' || NewREQ.charAt(0) == 'H')
      {
        //set the led back to normal
        for (int i = 0; i < 50; i++)
        {
          leds[(snakeposition + i) % 300] = savedpattern[i];
        }
        FastLED.show();
        ResetMovingAnimations();




      }
      if (NewREQ.charAt(0) == 'G')//set snake's speed
      {
        MovingSnakeSpeed = NewREQ.substring(2, 4).toInt();
        NewREQ = CurrentReq;//no need to change command
        Serial.println("\n!!!!!!new speed!!!!!!!!\n");
      }
    }

    if (CurrentReq.charAt(0) == 'D')
    {
      ResetMovingAnimations();
    }






    //'A' is for setting a specific color
    if (NewREQ.charAt(0) == 'A')
    {


      switch (NewREQ.charAt(2))
      {
        case 'R'://red
          Set_Single_Color(CRGB::Red);
          break;
        case 'O'://O for orange
          Set_Single_Color(CRGB::Orange);
          ResetMovingAnimations();
          break;
        case 'Y'://Y for yellow
          Set_Single_Color(CRGB::Yellow);
          ResetMovingAnimations();
          break;
        case 'G'://G for green
          Set_Single_Color(CRGB::Green);
          ResetMovingAnimations();
          break;
        case 'C'://C fo cyan
          Set_Single_Color(CRGB::Cyan);
          ResetMovingAnimations();
          break;
        case 'B'://B for blue
          Set_Single_Color(CRGB::Blue);
          ResetMovingAnimations();
          break;
        case 'P'://big P for purple
          Set_Single_Color(CRGB::Purple);
          ResetMovingAnimations();
          break;
        case 'p'://small p for pink
          Set_Single_Color(CRGB::HotPink);
          ResetMovingAnimations();
          break;
        case 'M'://M for Magenta
          Set_Single_Color(CRGB::Magenta);
          ResetMovingAnimations();
          break;
        case 'W'://W for white
          Set_Single_Color(CRGB::White);
          ResetMovingAnimations();
          break;
      }
    }
    //B is for Setting a preset pattern
    if (NewREQ.charAt(0) == 'B')
    {
      switch (NewREQ.charAt(2))
      {
          Set_Single_Color(CRGB::Black);
        case 'A':                       ////first pattern is rainbow, CRGB library has some bugs, so I do stuff with arrays instead of CRGB
          {
            ResetMovingAnimations();

            int a[3] = {255, 0, 0};
            int b[3] = {255, 255, 0};
            lerp_colors(a, b, 0, 75); //r-y


            a[1] = 255;
            b[0] = 0;

            //a = {255, 255, 0};
            //b = {0, 255, 0};
            lerp_colors(a, b, 75, 150); //y to g
            a[0] = 0;
            b[1] = 0;
            b[2] = 255;

            //a = {0, 255, 0};
            //b = {0, 0, 255};
            lerp_colors(a, b, 150, 225); //g to b

            a[1] = 0;
            a[2] = 255;
            b[0] = 255;
            b[2] = 0;

            //a = {0, 0, 255};
            //b = {255, 0, 0};
            lerp_colors(a, b, 225, 300); //b to r

            FastLED.show();
          }
          break;





        case 'B':                     //purple - magenta - cyan - purple pattern
          {
            ResetMovingAnimations();
            for (int i = 0; i < 5; i++)
            {
              int Purplea[3] = {128, 0, 128};
              int Magentaa[3] = {255, 0, 255};
              int Cyana[3] = {0, 255, 255};
              int Bluea[3] = {0, 0, 255};

              lerp_colors(Purplea, Magentaa, (60 * i) + 0, ((60 * i) + 15));
              lerp_colors(Magentaa, Cyana, (60 * i) + 15, ((60 * i) + 30));
              lerp_colors(Cyana, Bluea, (60 * i) + 30, ((60 * i) + 45));
              lerp_colors(Bluea, Purplea, (60 * i) + 45, ((60 * i) + 60));
            }

            FastLED.show();
          }
          break;







      }
    }

    //C is for setting a moving snake
    if (NewREQ.charAt(0) == 'C' && !NewREQ.equals(CurrentReq))
    {
      Serial.print("\nMessage C:");//debuging
      ResetMovingAnimations();

      if (NewREQ.charAt(2) == 'A')
      {
        Serial.print("\n 2 is A and setting color.");
        snakeposition = 0;
        MovingSnakeSpeed = NewREQ.substring(9, 11).toInt();
        OldSnakeTimeSample = millis();
        //set up color
        CRGB newcolor = CRGB::Black;
        char Cc = NewREQ.charAt(4);
        String cs;
        if (Cc == 'R')

        { newcolor = CRGB::Red;
          cs = "red";
        }


        if (Cc == 'O')
        { newcolor = CRGB::Orange;
          cs = "orange";
        }

        if (Cc == 'Y')
        { newcolor = CRGB::Yellow;
          cs = "yellow";
        }

        if (Cc == 'G')
        {
          newcolor = CRGB::Green;
          cs = "green";
        }

        if (Cc == 'C') {
          newcolor = CRGB::Cyan;
          cs = "cyan";
        }
        if (Cc == 'B') {
          newcolor = CRGB::Blue;
          cs = "blue";
        }

        if (Cc == 'P')
        { newcolor = CRGB::Purple;
          cs = "Purple";
        }

        if (Cc == 'p')
        { newcolor = CRGB::Pink;
          cs = "pink";
        }

        if (Cc == 'M') {
          newcolor = CRGB::Magenta;
          cs = "magenta";
        }
        if (Cc == 'W') {
          newcolor = CRGB::White;
          cs = "white";
        }
        String shtut = "setting snake color as : ";
        shtut = shtut + cs;
        Serial.print(shtut);//debuging

        //set up snake
        int snakelength = NewREQ.substring(6, 8).toInt();
        snakelength = (snakelength % 50); //top input for snake length is 50
        for (int i = 0; i < snakelength; i++)
        {
          snake[i] = newcolor;
        }

        for (int i = snakelength; i < 50; i++)
        {
          snake[i].r = 0; snake[i].b = 0; snake[i].g = 0;
        }
        //save leds that will be coverd by the snake
        for (int i = 0; i < 50; i++)
        {
          savedpattern[i] = leds[i];
        }
        Serial.print("\ncolor is: ");//debugging
        Serial.print(cs);
        Serial.print("\nlength is: ");
        Serial.print(snakelength);
        Serial.print("\nspeed is: ");
        Serial.print(MovingSnakeSpeed);


      }
      else if (NewREQ.charAt(2) == 'B')//if snake pattern--needed to be filled
      {
        snakeposition = 0;
        MovingSnakeSpeed = (NewREQ.charAt(6) - '0') - 17;
        OldSnakeTimeSample = millis();

        if (NewREQ.charAt(4) == 'A')
        {}//set pattern A
        if (NewREQ.charAt(4) == 'B')
        {}//set pattern B
        //etc.......
      }

    }


    if (NewREQ.charAt(0) == 'D')
    {
      MovingSnakeSpeed = 0;
      OldSnakeTimeSample = 0;
      MovingPatternSpeed = NewREQ.substring(2, 4).toInt();
      OldPatternTimeSample  = millis();
      Serial.print("\nspeed is :");
      Serial.print(MovingPatternSpeed);




    }
    if (NewREQ.charAt(0) == 'E')
    {
      int level = NewREQ.substring(2, 4).toInt();
      FastLED.setBrightness(level);
      FastLED.show();
      NewREQ = CurrentReq;

    }
    if (NewREQ.charAt(0) == 'F')
    {
      ResetMovingAnimations();
      Set_Single_Color(CRGB::Black);
    }


    if (NewREQ.charAt(0) == 'H')//set a specific color range
    {
      int StartPosition = NewREQ.substring(2, 5).toInt();;
      int EndPosition = NewREQ.substring(6, 9).toInt();;

      switch (NewREQ.charAt(10))
      {
        case 'R'://red
          { SetRange(StartPosition, EndPosition, CRGB::Red);
            FastLED.show();
          }
          break;
        case 'O'://O for orange
          { SetRange(StartPosition, EndPosition, CRGB::Orange);
            FastLED.show();
          }
          break;
        case 'Y'://Y for yellow
          { SetRange(StartPosition, EndPosition, CRGB::Yellow);
            FastLED.show();
          }
          break;
        case 'G'://G for green
          { SetRange(StartPosition, EndPosition, CRGB::Green);
            FastLED.show();
          }
          break;
        case 'C'://C fo cyan
          { SetRange(StartPosition, EndPosition, CRGB::Cyan);
            FastLED.show();
          }
          break;
        case 'B'://B for blue
          { SetRange(StartPosition, EndPosition, CRGB::Blue);
            FastLED.show();
          }
          break;
        case 'P'://big P for purple
          { SetRange(StartPosition, EndPosition, CRGB::Purple);
            FastLED.show();
          }
          break;
        case 'p'://small p for pink
          { SetRange(StartPosition, EndPosition, CRGB::Pink);
            FastLED.show();
          }
          break;
        case 'M'://M for Magenta
          { SetRange(StartPosition, EndPosition, CRGB::Magenta);
            FastLED.show();
          }
          break;
        case 'W'://W for white
          { SetRange(StartPosition, EndPosition, CRGB::White);
            FastLED.show();
          }
          break;

      }
    }







    CurrentReq = NewREQ;
    Serial.println("\nfinished input check");
  }



  //////////////////////////looping for moving snake/////////////////////
  if (MovingSnakeSpeed > 0)
  {
    int CurrentTimeSample = millis();
    if (CurrentTimeSample < OldSnakeTimeSample)//In case of millis overflowing and restarting
    {
      OldSnakeTimeSample = CurrentTimeSample;
    }
    if (((CurrentTimeSample - OldSnakeTimeSample )*MovingSnakeSpeed / 1000) > 1)//if the time as passed then iterate the snake
    {
      OldSnakeTimeSample = CurrentTimeSample;
      //set the led strip back to normal(delete snake)
      for (int i = 0; i < 50; i++)
      {
        leds[(snakeposition + i) % 300] = savedpattern[i];
      }
      snakeposition = (snakeposition + 1) % 300;

      //save area that is to be snake-infested
      for (int i = 0; i < 50; i++)
      {
        savedpattern[i] = leds[(snakeposition + i) % 300];
      }
      //infest with snake
      for (int i = 0; i < 50; i++)
      {
        if (!((snake[i].r == snake[i].g) && (snake[i].g == snake[i].b) && (snake[i].b == 0)))
          leds[(snakeposition + i) % 300] = snake[i];
      }
      FastLED.show();
      Serial.print("\nupdating snake\n");//debuging
    }
  }






  //////////////////////////looping for moving pattern///////////
  if (MovingPatternSpeed > 0)
  {
    int CurrentTimeSample = millis();
    if (CurrentTimeSample < OldSnakeTimeSample) //In case of millis overflowing and restarting
    {
      OldSnakeTimeSample = CurrentTimeSample;
    }

    if (((CurrentTimeSample - OldPatternTimeSample )*MovingPatternSpeed / 1000) > 1)//if the time has passed then iterate the pattern
    {
      OldPatternTimeSample = CurrentTimeSample;

      leds[299].setRGB(leds[0].r, leds[0].g, leds[0].b);
      for (int i = 0; i < 299; i++)
      {
        leds[i].setRGB(leds[i + 1].r, leds[i + 1].g, leds[i + 1].b );
      }
      delay(1);
      FastLED.show();
      delay(10);
    }
  }

  //correction code for pattern/snake speed bad values
  if (MovingPatternSpeed < 0)
  {
    MovingPatternSpeed = 0;
    OldPatternTimeSample = 0;
  }

  if (MovingSnakeSpeed < 0)
  {
    MovingSnakeSpeed = 0;
    OldSnakeTimeSample = 0;
  }
}


//self fucking explanatory
void ResetMovingAnimations() {
  MovingPatternSpeed = 0;
  OldPatternTimeSample = 0;

  MovingSnakeSpeed = 0;
  OldSnakeTimeSample = 0;
}

//search if a client is connected
void CheckClient() {

  WiFiClient client = server.available();
  if (0 != client) {
    Serial.println("new client");
    delay(1);
    handleclient(client);
  }

}
//handle the goddamn idiot client
void handleclient(WiFiClient client) {
  while (client.connected())
  {
    while (!client.available()) {
      if (!client.connected())
        return;
      delay(1);
    }

    String req = client.readString();
    Serial.println(req);
    client.flush();
    NewREQ = req;
    Serial.println("\nClient Disconnected\n");
    client.stop();
  }
}
// set a single color all over the strip
void Set_Single_Color (CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++)
    leds[i] = color;
  FastLED.show();
  Serial.println("\n changed color\n");

}
//this functions makes a lerp effect between two colors in a given range
//requirment endpos>startPos
void lerp_colors(int startC[3], int endC[3], int startPos, int endPos) { //not including endpos
  int D = endPos - startPos;
  leds[startPos].setRGB(startC[0], startC[1], startC[2]);
  for (int i = startPos + 1; i < endPos; i++)
  {

    int Rd = endC[0] - startC[0];//check differences
    int Gd = endC[1] - startC[1];
    int Bd = endC[2] - startC[2];
    int R = (startC[0] + int(float(Rd) * (float(i) / float(D))));
    int G = (startC[1] + int(float(Gd) * (float(i) / float(D))));
    int B = (startC[2] + int(float(Bd) * (float(i) / float(D))));
    leds[i].setRGB(R, G, B);
  }
}



void SetRange(int StartPosition, int EndPosition, CRGB setcolor)
{
  if ((StartPosition >= 0) && (StartPosition < EndPosition) && (EndPosition <= 300))
  {

    for (int i = StartPosition; i < EndPosition; i++)
    {
      leds[i] = setcolor;
    }




  }





}
