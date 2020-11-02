
enum Press{Right, Up, Down, Left, SEL};

void test() 
{  
  analogWrite(A1, 255);
  delay(1000);
  analogWrite(A1, 0);
  delay(1000);
  analogWrite(A2, 255);
  delay(1000);
  analogWrite(A2, 0);
  delay(1000);
/*
  for (int i=0; i<1000; i++) {
    digitalWrite(5, i);
    delay(200);
  }
*/    
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(A0, INPUT);   //buttons
  pinMode(A1, OUTPUT); //Led Yellow
  pinMode(A2, OUTPUT); //Led green
//  pinMode(5, OUTPUT); //Buzzer

  test();
  Serial.println("Starting");
}

void loop() {
  // put your main code here, to run repeatedly:
  int button = analogRead(A0);

  if (button < 897) {
    Serial.println(button);
      
    if ((button >= 631) && (button <= 896)) { //Up
      analogWrite(A1, 255);
    } else if ((button >= 408) && (button <= 630)) { //Down
      analogWrite(A2, 255);
    } else if ((button >= 231) && (button <= 407)) { //Rigth
      digitalWrite(5, 250);
    } else if ((button >= 69) && (button <= 230)) { //Left
      digitalWrite(5, 0);
    } else {
      test(); //Sel
    }

    delay(300);
  }

}  
   
 
