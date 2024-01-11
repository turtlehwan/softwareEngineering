#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

//Sensor에서 특정 거리 이하면 장애물 존재한다고 판단하도록
#define OBSTACLE_STANDARD 5 

int Front_Distance = 0; //Front Sensor Interface 값
int Left_Distance = 10; //Left Sensor Interface 값
int Right_Distance = 1; //Right Sensor Interface 값
bool Dust_Existance = false;  //Dust Sensor Interface

int now_move_statement = 1; //이전 state가 Forward : 1 / Backword : -1 / 그 외 : 0
// MoveLeft, MoveRight는 5 Tick 동안 진행되어야 하므로 Tick을 측정할 변수
int left_count = 0;
int right_count = 0;

//Determine Obstacle Location
int Det_OL(int Front_Distance, int Left_Distance, int Right_Distance) { 
  // 이진수 000 : Front - Left - Right
  // 장애물 존재하면 해당 bit 1 / 존재하지 않으면 해당 bit 0
  int obstacle = 0b000;

  if (Front_Distance <= OBSTACLE_STANDARD) {
    obstacle += 0b100;
    printf("전방 장애물이 감지됨\n");
  }
  if (Left_Distance <= OBSTACLE_STANDARD) {
    obstacle += 0b010;
    printf("좌측 장애물이 감지됨\n");
  }
  if (Right_Distance <= OBSTACLE_STANDARD) {
    obstacle += 0b001;
    printf("우측 장애물이 감지됨\n");
  }
  return (obstacle);
}

bool Det_DE(bool Dust_Existance) {
  return (Dust_Existance);
}

int MotorInterface(int Direction){
  if(Direction == 1){
    printf("전진 수행\n");
    return (Direction);
  }
  else if(Direction == 2){
    printf("후진 수행\n");
    return (Direction);
  }
  else if(Direction == 3){
    printf("좌회전 수행\n");
    return (Direction);
  }
  else if(Direction == 4){
    printf("우회전 수행\n");
    return (Direction);
  }
  else if(Direction == -1){
    printf("전진 중지\n");
    return (Direction);
  }
  else if(Direction == -2){
    printf("후진 중지\n");
    return (Direction);
  }
  return (0); //실패 시
}

int CleanerInterface(bool Trigger){
  if(Trigger){
    printf("청소 가동\n");
    return (1);
  }
  else{
    printf("청소 중지\n");
    return (0);
  }
}

int CleanerPower(bool Trigger) {
  return CleanerInterface(Trigger);
}

int MoveForward(bool Enable_Disable) {
  if (Enable_Disable)
    return MotorInterface(1);
  else
    return MotorInterface(-1);
}

int MoveBackward(bool Enable_Disable) {
  if (Enable_Disable)
    return MotorInterface(2);
  else
    return MotorInterface(-2);
}

int TurnLeft(bool Trigger) {
  if (Trigger) {
    if (left_count > 0)
      left_count -= 1;
    else if (left_count == 0)
      left_count = 4;
    printf("좌회전 %d Tick 남음\n", left_count);
    return MotorInterface(3);
  }
  return (0); //실패 시
}

int TurnRight(bool Trigger) {
  if (Trigger) {
    if (right_count > 0)
      right_count -= 1;
    else if (right_count == 0)
      right_count = 4;
    printf("우회전 %d Tick 남음\n", right_count);
    return MotorInterface(4);
  }
  return (0); //실패 시
}

int* order_rvc(int f, int l, int r, bool d) {
    int obstacle_Location = Det_OL(f, l, r);
    bool dust_Existence = Det_DE(d);
    int motorValue = 0;
    int cleanerValue = 0;


    if (left_count == 0 && right_count == 0) {
        if (obstacle_Location < 4 && !dust_Existence) {
            if (now_move_statement == -1) {
                if (obstacle_Location == 1 || obstacle_Location == 0) {
                    motorValue = TurnLeft(true);
                    cleanerValue = CleanerPower(false);
                }
                else if (obstacle_Location == 2) {
                    motorValue = TurnRight(true);
                    cleanerValue = CleanerPower(false);
                }
            }
            else {
                motorValue = MoveForward(true);
                now_move_statement = 1;
                cleanerValue = CleanerPower(false);
            }
        }
        else if (obstacle_Location < 4 && dust_Existence) {
            if (now_move_statement == -1) {
                if (obstacle_Location == 1 || obstacle_Location == 0) {
                    motorValue = TurnLeft(true);
                    cleanerValue = CleanerPower(false);
                }
                else if (obstacle_Location == 2) {
                    motorValue = TurnRight(true);
                    cleanerValue = CleanerPower(false);
                }
            }
            else {
                motorValue = MoveForward(true);
                now_move_statement = 1;
                cleanerValue = CleanerPower(true);
            }
        }
        else if (obstacle_Location == 4 || obstacle_Location == 5) { //Tick[F && !L] || Tick[!L]
            if (now_move_statement == 1) {
                MoveForward(false);
                now_move_statement = 0;
            }
            else if (now_move_statement == -1) {
                MoveBackward(false);
                now_move_statement = 0;
            }
            motorValue = TurnLeft(true);
            cleanerValue = CleanerPower(false);
        }
        else if (obstacle_Location == 6) {  //Tick[F && L && !R]
            if (now_move_statement == 1) {
                MoveForward(false);
                now_move_statement = 0;
            }
            else if (now_move_statement == -1) {
                MoveBackward(false);
                now_move_statement = 0;
            }
            motorValue = TurnRight(true);
            cleanerValue = CleanerPower(false);
        }
        else if (obstacle_Location == 7) {  //Tick[F && L && R]
            if (now_move_statement == 1)
                MoveForward(false);
            motorValue = MoveBackward(true);
            now_move_statement = -1;
            cleanerValue = CleanerPower(false);
        }
    }
    else {
        if (left_count > 0) {
            motorValue = TurnLeft(true);
            cleanerValue = CleanerPower(false);
        }
        else if (right_count > 0) {
            motorValue = TurnRight(true);
            cleanerValue = CleanerPower(false);
        }
    }

    int retArr[2] = { motorValue , cleanerValue };
    return retArr;
}

void init() {
  while (1) {
    order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance);
    usleep(200 * 1000);  //wait(200ms)
  }
}

void main() {
  init();
}

