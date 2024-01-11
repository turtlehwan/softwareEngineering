#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

// 모의로 사용할 함수들
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

  if (Front_Distance < 0 || Left_Distance < 0 || Right_Distance < 0)
	return (-1);
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

int retArr[2];
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
				else if (obstacle_Location == 3) {
					motorValue = MoveBackward(true);
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
				else if (obstacle_Location == 3) {
					motorValue = MoveBackward(true);
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

    retArr[0] = motorValue;
	retArr[1] = cleanerValue;
    return retArr;
}

void main_temp() {
  while (1) {
    order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance);
    usleep(200 * 1000);  //wait(200ms)
  }
}


// Test suite initialization function
int init_suite(void) {
    // Initialize your variables or any other setup needed for tests
    return 0;
}

// Test suite cleanup function
int clean_suite(void) {
    // Cleanup code here
    return 0;
}

// Test functions
/** case 01. [ !F / !L / !R / !D ] */
void test01() {
    Front_Distance = 10;
    Left_Distance = 10;
    Right_Distance = 10;
    Dust_Existance = false;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 02. [ F / !L / !R / !D ] */
void test02() {
    Front_Distance = 3;
    Left_Distance = 10;
    Right_Distance = 10;
    Dust_Existance = false;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 03. [ !F / L / !R / !D ] */
void test03() {
    Front_Distance = 10;
    Left_Distance = 3;
    Right_Distance = 10;
    Dust_Existance = false;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 04. [ !F / !L / R / !D ] */
void test04() {
    Front_Distance = 10;
    Left_Distance = 10;
    Right_Distance = 3;
    Dust_Existance = false;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 05. [ !F / L / R / !D ] */
void test05() {
    Front_Distance = 10;
    Left_Distance = 3;
    Right_Distance = 3;
    Dust_Existance = false;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

	left_count = 0;
	right_count = 0;
    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 06. [ F / L / !R / !D ] */
void test06() {
    Front_Distance = 3;
    Left_Distance = 3;
    Right_Distance = 10;
    Dust_Existance = false;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 07. [ F / !L / R / !D ] */
void test07() {
    Front_Distance = 3;
    Left_Distance = 10;
    Right_Distance = 3;
    Dust_Existance = false;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 08. [ F / L / R / !D ] */
void test08() {
    Front_Distance = 3;
    Left_Distance = 3;
    Right_Distance = 3;
    Dust_Existance = false;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 09. [ !F / !L / !R / D ] */
void test09() {
    Front_Distance = 10;
    Left_Distance = 10;
    Right_Distance = 10;
    Dust_Existance = true;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 1);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 10. [ F / !L / !R / D ] */
void test10() {
    Front_Distance = 3;
    Left_Distance = 10;
    Right_Distance = 10;
    Dust_Existance = true;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 11. [ !F / L / !R / D ] */
void test11() {
    Front_Distance = 10;
    Left_Distance = 3;
    Right_Distance = 10;
    Dust_Existance = true;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 1);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 12. [ !F / !L / R / D ] */
void test12() {
    Front_Distance = 10;
    Left_Distance = 10;
    Right_Distance = 3;
    Dust_Existance = true;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);

	left_count = 0;
	right_count = 0;
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 1);

	left_count = 0;
	right_count = 0;
    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 13. [ !F / L / R / D ] */
void test13() {
    Front_Distance = 10;
    Left_Distance = 3;
    Right_Distance = 3;
    Dust_Existance = true;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 1);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 1);

	left_count = 0;
	right_count = 0;
    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 14. [ F / L / !R / D ] */
void test14() {
    Front_Distance = 3;
    Left_Distance = 3;
    Right_Distance = 10;
    Dust_Existance = true;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 4);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 15. [ F / !L / R / D ] */
void test15() {
    Front_Distance = 3;
    Left_Distance = 10;
    Right_Distance = 3;
    Dust_Existance = true;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 3);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}
/** case 16. [ F / L / R / D ] */
void test16() {
    Front_Distance = 3;
    Left_Distance = 3;
    Right_Distance = 3;
    Dust_Existance = true;
	left_count = 0;
	right_count = 0;

    now_move_statement = 1;
    printf("\nASSERT 1\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 2\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);

    now_move_statement = -1;
    printf("\nASSERT 3\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[0] == 2);
    printf("\nASSERT 4\n");
    CU_ASSERT(order_rvc(Front_Distance, Left_Distance, Right_Distance, Dust_Existance)[1] == 0);
}

//function test
void testDet_OL() {
	printf("\nASSERT 1\n");
    CU_ASSERT(Det_OL(7, 7, 7) == 0);   
	printf("\nASSERT 2\n");
    CU_ASSERT(Det_OL(3, 7, 7) == 0b100);   
	printf("\nASSERT 3\n");
    CU_ASSERT(Det_OL(7, 3, 7) == 0b010);  
	printf("\nASSERT 4\n");
    CU_ASSERT(Det_OL(7, 7, 3) == 0b001);  
	printf("\nASSERT 5\n");
    CU_ASSERT(Det_OL(7, 3, 3) == 0b011);   
	printf("\nASSERT 6\n");
    CU_ASSERT(Det_OL(3, 3, 7) == 0b110);  
	printf("\nASSERT 7\n");
    CU_ASSERT(Det_OL(3, 7, 3) == 0b101);   
	printf("\nASSERT 8\n");
    CU_ASSERT(Det_OL(3, 3, 3) == 0b111);   
}

void testDet_OL_negative() {
	printf("\nASSERT 1\n");
    CU_ASSERT(Det_OL(-7, -7, -7) == -1);   
	printf("\nASSERT 2\n");
    CU_ASSERT(Det_OL(-24234233, 5, 10) == -1);
	printf("\nASSERT 3\n");
    CU_ASSERT(Det_OL( 0, 235, -23425) == -1);
	printf("\nASSERT 4\n");
    CU_ASSERT(Det_OL( 0, -2355, 85) == -1);
}

void testDet_DE() {
    CU_ASSERT_TRUE(Det_DE(true)); // Dust가 있을 경우
    CU_ASSERT_FALSE(Det_DE(false)); // Dust가 없을 경우
}

void testMotorInterface() {
	printf("\nASSERT 1\n");
    CU_ASSERT(MotorInterface(1) == 1);
	printf("\nASSERT 2\n");
    CU_ASSERT(MotorInterface(2) == 2);
	printf("\nASSERT 3\n");
    CU_ASSERT(MotorInterface(3) == 3);
	printf("\nASSERT 4\n");
    CU_ASSERT(MotorInterface(4) == 4);
	printf("\nASSERT 5\n");
    CU_ASSERT(MotorInterface(-1) == -1);
	printf("\nASSERT 6\n");
    CU_ASSERT(MotorInterface(-2) == -2);
	printf("\nASSERT 7\n");
    CU_ASSERT(MotorInterface(234234) == 0);
}

void testCleanerInterface() {
    CU_ASSERT(CleanerInterface(1) == 1);
    CU_ASSERT(CleanerInterface(0) == 0);
}

void testCleanerPower() {
    CU_ASSERT_TRUE(CleanerPower(true));
    CU_ASSERT_FALSE(CleanerPower(false));
}

void testMoveForward() {
    CU_ASSERT(MoveForward(1) == 1);
    CU_ASSERT(MoveForward(0) == -1);
}

void testMoveBackward() {
    CU_ASSERT(MoveBackward(1) == 2);
    CU_ASSERT(MoveBackward(0) == -2);
}

void testTurnLeft() {
    CU_ASSERT(TurnLeft(1) == 3);
    CU_ASSERT(TurnLeft(0) == 0);
}

void testTurnRight() {
    CU_ASSERT(TurnRight(1) == 4);
    CU_ASSERT(TurnRight(0) == 0);
}


// Add tests to suite
void addTests() {
    CU_pSuite suite = CU_add_suite("RVC_TestSuite", init_suite, clean_suite);

	CU_add_test(suite, "Test01\n", test01);
	CU_add_test(suite, "Test02\n", test02);
	CU_add_test(suite, "Test03\n", test03);
	CU_add_test(suite, "Test04\n", test04);
	CU_add_test(suite, "Test05\n", test05);
	CU_add_test(suite, "Test06\n", test06);
	CU_add_test(suite, "Test07\n", test07);
	CU_add_test(suite, "Test08\n", test08);
	CU_add_test(suite, "Test09\n", test09);
	CU_add_test(suite, "Test10\n", test10);
	CU_add_test(suite, "Test11\n", test11);
	CU_add_test(suite, "Test12\n", test12);
	CU_add_test(suite, "Test13\n", test13);
	CU_add_test(suite, "Test14\n", test14);
	CU_add_test(suite, "Test15\n", test15);
	CU_add_test(suite, "Test16\n", test16);


    CU_add_test(suite, "Test of Det_OL()\n", testDet_OL);
    CU_add_test(suite, "Test of Det_DE()\n", testDet_DE);

	CU_add_test(suite, "Test of testMotorInterface()\n", testMotorInterface);
    CU_add_test(suite, "Test of testCleanerInterface()\n", testCleanerInterface);
	CU_add_test(suite, "Test of testCleanerPower()\n", testCleanerPower);
    CU_add_test(suite, "Test of testMoveForward()\n", testMoveForward);
	CU_add_test(suite, "Test of testMoveBackward()\n", testMoveBackward);
    CU_add_test(suite, "Test of testTurnLeft()\n", testTurnLeft);
	CU_add_test(suite, "Test of testTurnRight()\n", testTurnRight);

    CU_add_test(suite, "Test of Det_OL() / negative\n", testDet_OL_negative);
}

// Main function to run the tests
int main() {
    // Initialize CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

    // Add tests to the suite
    addTests();

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();  // Cleanup the test registry
    return CU_get_error();
}