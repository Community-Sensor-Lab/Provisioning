

void setup() {
  Serial.begin(9600);
  delay(3000);
  Serial.println(__FILE__);

  String ssidl,passcodel,gsidl;

}

void loop() {
  int great, small, x, y;

  Serial.println("Enter two numbers: \n");
  while (Serial.available() == 0);
  String input = Serial.readStringUntil('\n');
  sscanf(input.c_str(),"%d %d", &x, &y);
  Serial.print("x: "); Serial.print(x); Serial.print(" y: "); Serial.println(y);
  
  // The last two arguments are passed
  // by giving addresses of memory locations
  compare(x, y, great, small);
  Serial.print("The greater number is "); Serial.print(great);
  Serial.print(" and the smaller number is "); Serial.println(small);
  delay(3000);
}


void compare(int a, int b, int &add_great, int &add_small)
{
  if (a > b) {
    add_great = a;
    add_small = b;
  }
  else {
    add_great = b;
    add_small = a;
  }
}
