
#define UP      0
#define DOWN    1
#define LEFT    2
#define RIGHT   3

#define MAX_BENDS 100

struct Snake_Data {
  int length;
  int head_x; // Stores Head X Coordinate
  int head_y; // Stores Head Y Coordinate
  int head_dir; // Stores Head Direction
  int tail_x; // Stores Tail X Coordinat
  int tail_y; // Stores Tail Y Coordinat
  int tail_dir; // Stores Tail Direction
  int bend_x [MAX_BENDS]; //Stores X Bend Coordinate Declare it big enough to accomodate maximum bends
  int bend_y[MAX_BENDS]; // Stores Y Bend Coordinate Declare it big enough to accomodate maximum bends 
  int bend_dir [MAX_BENDS]; // Stores Bend direction when tail reaches that X Coordinate
} Snake; // Declares a variable of the structure


void initgamedata();
void movesnake();
void userinput();

void setup(){
  initgamedata();
  Serial.begin(115200);
  //Draw Intial Snake Body   

}

void loop(){
  delay(100);
  userinput ();
  movesnake ();
}

void initgamedata ( ) //Snakes starting coordinate if you modify any one make sure also modify dependent values
{
  int i;
  Snake.length = 6;
  Snake.head_x = 6;
  Snake.head_y = 5;
  Snake.head_dir = RIGHT  ;
  Snake.tail_x = Snake.head_x- Snake.length+1  ;
  Snake.tail_y = Snake.head_y;
  Snake.tail_dir = RIGHT;
  
  for (int i = 0; i <MAX_BENDS;i++) // There is no bend initally
  {
    Snake.bend_x[i] = 0;
    Snake.bend_y[i] = 0; 
  }
    
  //This part should be redesigned for change of code of intial values   
  for (int i = Snake.length; i>0;i--){ 
    Screen.setPixel(Snake.head_x-i+1,Snake.head_y);
  }  

}

//Moves The Snake Across the Screen
void movesnake (){

  //Move the Head
  if (Snake.head_dir == LEFT){
    if (Snake.head_x == 0)
      Snake.head_x = 7;   
    else
     Snake.head_x --;
  }
  
  if (Snake.head_dir == RIGHT){
    if (Snake.head_x == 7)
      Snake.head_x = 0;   
    else
     Snake.head_x ++;
  }
  
  if (Snake.head_dir == UP){
    if (Snake.head_y == 0)
      Snake.head_y = 7;   
    else
      Snake.head_y --;

  }
  
  if (Snake.head_dir == DOWN){
    if (Snake.head_y == 7)
      Snake.head_y = 0;   
    else
      Snake.head_y ++;

  } 
  
  Screen.setPixel (Snake.head_x, Snake.head_y);
   
  //Move the Tail
  Screen.clearPixel (Snake.tail_x, Snake.tail_y);
   
  if (Snake.tail_dir == LEFT){
    if (Snake.tail_x == 0)
      Snake.tail_x = 7;   
    else
      Snake.tail_x--;

  }
  
  if (Snake.tail_dir == RIGHT){
    if (Snake.tail_x == 7)
      Snake.tail_x = 0;   
    else
      Snake.tail_x ++;

  }
  
  if (Snake.tail_dir == UP){
    if (Snake.tail_y == 0)
      Snake.tail_y = 7;   
    else
      Snake.tail_y--;

  }
  
  if (Snake.tail_dir == DOWN){
    if (Snake.tail_y == 7)
      Snake.tail_y = 0;   
    else
      Snake.tail_y ++;

  }
  
}

void userinput ()// This Function changes the direction of motion of snake according to input.
{
  static int i = 0;
  if ( i > MAX_BENDS) i = 0; // Makes the bend array a circular queue
  
  static int j = 0;
  if ( j > MAX_BENDS) j = 0;
  
  char input;
    
  if (Serial.available()>0){
    input = Serial.read();
    
    while (Serial.available()>0)
      Serial.read();
      
    //Change Respective Return value to our MACRO Direction Code Value 
               
     if (input == 's') input = DOWN;
               
     if (input == 'w') input = UP;
               
     if (input == 'a') input = LEFT;
               
     if (input == 'd') input = RIGHT;
     
     if (input == 'r') {
       Screen.clearScreen();
       initgamedata();
       return;
     }

   //Change head direction based on logic
               
     if (input == LEFT && Snake.head_dir != RIGHT && Snake.head_dir != LEFT)
        {
          Snake.head_dir = LEFT; 
          Snake.bend_x [i] = Snake.head_x;
          Snake.bend_y [i] = Snake.head_y;
          Snake.bend_dir [i] = LEFT;
          i++;
        } 
     if (input == RIGHT && Snake.head_dir != LEFT && Snake.head_dir != RIGHT)
        {
          Snake.head_dir = RIGHT;
          Snake.bend_x [i] = Snake.head_x;
          Snake.bend_y [i] = Snake.head_y;
          Snake.bend_dir [i] = RIGHT;
          i++; 
         }
     if (input == UP && Snake.head_dir != DOWN && Snake.head_dir != UP)
        {
          Snake.head_dir = UP;
          Snake.bend_x [i] = Snake.head_x;
          Snake.bend_y [i] = Snake.head_y;
          Snake.bend_dir [i] = UP;
          i++; 
        }
    if (input == DOWN && Snake.head_dir != UP && Snake.head_dir != DOWN)
    {
          Snake.head_dir = DOWN; 
          Snake.bend_x [i] = Snake.head_x;
          Snake.bend_y [i] = Snake.head_y;
          Snake.bend_dir [i] = DOWN;
          i++; 
    }

     }
     
   //Code to change the y direction at respective time
   if (Snake.tail_x == Snake.bend_x [j] && Snake.tail_y == Snake.bend_y [j])
   {
        Snake.tail_dir = Snake.bend_dir [j];
        j++;
   }
       
}