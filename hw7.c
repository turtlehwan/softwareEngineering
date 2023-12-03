#include <stdio.h>

void main() {
  int obstacle_Location;
  bool dust_Existence;

  while (1) { // 장애물 감지
    obstacle_Location = Det_OL();
    dust_Existence = Det_DE();  
    
    if ()

    wait(200ms);    
  }
}

/*
int Wheel
   1 // 전진
   0 //정지
   -1 // 후진

int Cleaner
   1 // 청소 실시
   0 // 청소 중지

CleanerPower() >> 상태에 따라 Cleaner = 1 || Cleaner = 0;

MoveForward() >> Wheel = 1 // 전진상태로 변환합니다.

MoveBackward() >> Wheel = -1 // 후진상태로 변환합니다.
      
TurnLeft() >> print("좌측으로 회전합니다") 
WaitTickForTurning = 5
if(WaitTickForTurning > 0) WaitTickForTurning--
회전중.. 출력하고 wait(200ms)
// Tick * 5번 동안은 while문에 들어왔을때 TurnLeft함수안으로들어가서
// TurnLeft함수 내부에서 조작을 하고 (WaitTickForTurning가 0이 되면
// 정상적으로 다음 while 루프진행

while() 안에서
WaitTickForTurning >0 상태라면 TurnLeft() 함수 재호출

TurnRight() >> print("우측으로 회전합니다") 
WaitTickForTurning = 5
if(WaitTickForTurning > 0) WaitTickForTurning--
회전중.. 출력하고 wait(200ms)
*/