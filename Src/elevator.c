// 전역 변수 선언
#include "elevator.h"
uint8_t internal_requests[3] = {0}; // 내부 요청 배열 (최대 3개 층: B1, F1, F2)
uint8_t up_requests[3] = {0};       // 위로 가는 외부 요청 배열 (최대 3개)
uint8_t down_requests[3] = {0};     // 아래로 가는 외부 요청 배열 (최대 3개)
uint8_t current_floor;       // 현재 층 (정적 변수로 외부 접근 제한)
uint8_t prev_floor;          // 출발 층 설정
uint8_t next_floor;         // 다음 목표 층 (정적 변수로 외부 접근 제한)
uint32_t moving_time=0;
ElevatorState state = IDLE;         // 엘리베이터 상태 (IDLE: 정지, UP: 상승, DOWN: 하강)

// 층을 문자열로 변환하는 헬퍼 함수
// 매개변수: floor - 층 번호 (B1, F1, F2)
// 반환값: 해당 층의 문자열 표현
const char *floor_to_str(uint8_t floor) {
    switch (floor) {
        case B1: return "B1";   // 지하 1층
        case F1: return "F1";   // 1층
        case F2: return "F2";   // 2층
        default: return "Unknown"; // 알 수 없는 층
    }
}

// 상태를 문자열로 변환하는 헬퍼 함수
// 매개변수: s - 엘리베이터 상태 (IDLE, UP, DOWN)
// 반환값: 해당 상태의 문자열 표현
const char *state_to_str(ElevatorState s) {
    switch (s) {
        case IDLE: return "IDLE";   // 정지 상태
        case UP: return "UP";       // 상승 상태
        case DOWN: return "DOWN";   // 하강 상태
        default: return "Unknown";  // 알 수 없는 상태
    }
}

// 내부 요청 배열 포인터 반환
// 반환값: internal_requests 배열의 포인터
uint8_t *get_internal_requests(void)
{
    return internal_requests;
}

// 위로 가는 요청 배열 포인터 반환
// 반환값: up_requests 배열의 포인터
uint8_t *get_up_requests(void)
{
    return up_requests;
}

// 아래로 가는 요청 배열 포인터 반환
// 반환값: down_requests 배열의 포인터
uint8_t *get_down_requests(void)
{
    return down_requests;
}

// 현재 층을 센서 데이터로 업데이트
// 센서 데이터를 기반으로 current_floor 값을 설정
void set_currentfloor(void)
{
    //uint8_t prev_floor = current_floor; // 이전 층 저장
    if (Get_3cm_photo_data() == 1)
    {
        // 3cm 센서 감지 시 B1
        current_floor = B1;
    }
    else if (Get_9cm_photo_data() == 1)
    {
      // 9cm 센서 감지 시 F1
        current_floor = F1;
    }
    else if (Get_15cm_photo_data() == 1)
    {
      // 15cm 센서 감지 시 F2
        current_floor = F2;
    }
    // 비정상적인 층 변경 감지
    if (abs(current_floor - prev_floor) > 1)
    {
        printf("Warning: Abnormal floor change detected! Prev: %s, Current: %s\n",
               floor_to_str(prev_floor), floor_to_str(current_floor));
    }
}

// 엘리베이터 상태 설정
// 현재 층과 다음 층을 비교하여 상태(IDLE, UP, DOWN)를 결정
void set_state(void)
{
    if (current_floor == next_floor)
    {
        state = IDLE;
        printf("State changed to IDLE after request processing \n");
    }
    else if (current_floor < next_floor)
    {
        state = UP;
        printf("State changed to UP after request processing\n");
    }
    else if (current_floor > next_floor)
    {
        state = DOWN;
        printf("State changed to DOWN after request processing\n");
    }
}
// 요청 배열에 요청이 있는지 확인
// 매개변수: requests - 확인할 요청 배열
// 반환값: 1 (요청 있음), 0 (요청 없음)
uint8_t check_request(uint8_t *requests)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] != 0)
        { // 요청이 0이 아니면 (즉, 층 요청이 존재)
            return 1;
        }
    }
    return 0;
}

// 문 열고 닫기 동작 시뮬레이션
// 1초 대기 후 현재 층에서 문 열고 닫는 동작 출력
void door_open_close(void)
{
	printf("Door opened...\r\n");
	BUZZER_Open();
    if(current_floor==F2)
    {
        //RFID 눌렀을 때만 문 열림 실행
        if(0)
        {
            Servo_Rotate(30, 20);
            HAL_Delay(10);
            Servo_Rotate(80,20);
        }
        else
        {
            return;
        }
    }

    Servo_Rotate(30, 20);
    HAL_Delay(5000);
    Servo_Rotate(80,20);
    printf("Door closed...\r\n");
    printf("Door open/close at floor %s\n", floor_to_str(current_floor));
    HAL_Delay(1000); // 1초 대기 (문 열고 닫는 시간)
    BUZZER_Close();


}

// 엘리베이터를 한 층 위로 이동
// 스텝 모터를 사용해 상승 동작 수행
void operate_elevator_up(void)
{
    __HAL_TIM_SET_COUNTER(&htim9, 0);
    for(uint8_t i=0; i<5 ;i++)
    {
        rotate_1cm(DIR_CCW);
    }
}

// 엘리베이터+를 한 층 아래로 이동
// 스텝 모터를 사용해 하강 동작 수행
void operate_elevator_down(void)
{
	__HAL_TIM_SET_COUNTER(&htim9, 0);
	for(uint8_t i=0; i<5 ;i++)
	{
		rotate_1cm(DIR_CW);
	}
}

// 특정 층 요청의 인덱스 찾기
// 매개변수: requests - 요청 배열, floor - 찾을 층 번호
// 반환값: 요청 인덱스 (없으면 -1)
uint8_t find_request_index(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] == floor)
        {
            return i; // 해당 층의 인덱스 반환
        }
    }
    return -1; // 요청이 없으면 -1 반환
}

//특정 층 요청이 있는지 확인
uint8_t find_request_name(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] == floor)
        {
            return 1; // 해당 층의 요청 있으면 1 반환
        }
    }
    return -1; // 요청이 없으면 -1 반환
}

//현재 층보다 높은 층의 요청이 있는지 확인 후 인덱스 반환
uint8_t find_request_adove(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if(requests[i] !=0)
        {
            if (requests[i] > floor)
            {
                return 1; // 해당 층의 요청 있으면 1 반환
            }
        }
    }
    return -1; // 요청이 없으면 -1 반환
}

//현재 층보다 높은 층의 요청이 있는지 확인 후 인덱스 반환
uint8_t find_request_below(uint8_t *requests, uint8_t floor)
{
    for (int i = 0; i < 3; i++)
    {
        if(requests[i] !=0)
        {
            if (requests[i] < floor)
            {
                return 1; // 해당 층의 요청 있으면 1 반환
            }
        }
    }
    return -1; // 요청이 없으면 -1 반환
}


// 외부 인터럽트로 요청 추가
// 매개변수: requests - 추가할 요청 배열, operator - 추가할 층 번호
void push_requests(uint8_t *requests, uint8_t operator)
{
    uint8_t count = 0;
    for (int i = 0; i < 3; i++)
    {
        if (requests[i] != 0)
        {
            count++; // 현재 요청 개수 계산
        }
        if (requests[i] == operator)
        {
            return; // 중복 요청이면 추가하지 않음
        }
    }
    if (count < 3)
    { // 배열이 꽉 차지 않았으면
        requests[count] = operator; // 빈 자리에 요청 추가
        char msg[64];
        sprintf(msg, "Pushed request %d to %s\n", operator,
                (requests == internal_requests) ? "internal" :
                (requests == up_requests) ? "up" : "down");
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        sprintf(msg, "After push - [%d,%d,%d]\n", requests[0], requests[1], requests[2]);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}

// 요청 제거 (처리 완료 또는 버튼 토글)
// 매개변수: requests - 요청 배열, floor - 제거할 층, is_processed - 처리 완료 여부 (1: 완료, 0: 토글)
void remove_request(uint8_t *requests, uint8_t floor, uint8_t is_processed)
{
    int index = find_request_index(requests, floor);
    if (index != -1)
    { // 요청이 존재하면
        for (int i = index; i < 2; i++)
        {
            requests[i] = requests[i + 1]; // 뒤의 요소를 앞으로 이동
        }
        requests[2] = 0; // 마지막 요소 초기화
        char msg[64];
        if (is_processed)
        {
            sprintf(msg, "Popped request %d from %s\n", floor,
                    (requests == internal_requests) ? "internal" :
                    (requests == up_requests) ? "up" : "down");
        }
        else
        {
            sprintf(msg, "Request %d removed from %s\n", floor,
                    (requests == internal_requests) ? "internal_requests" :
                    (requests == up_requests) ? "up_requests" : "down_requests");
        }
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        sprintf(msg, "After remove - [%d,%d,%d]\n", requests[0], requests[1], requests[2]);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
    }
}
//정지상태에서 요청 처리
//요청은 처리를 하나 끝내고 다시 요청을 확인

uint8_t choose_closest_floor(uint8_t * requests)
{
	uint8_t closest_floor=0xFF;
    for(uint8_t i=0;i<3;i++)
    {
    	if(requests[i] != 0)
    	{
    		if(closest_floor > abs(requests[i] - current_floor))
    		{
    			closest_floor = requests[i];
    		}
    	}
    }
    return closest_floor;
}

void request_process_STOPPED(void)
{
    set_currentfloor();
    if(!check_request(internal_requests) && !check_request(up_requests) && !check_request(down_requests))
    {
      printf("NO REQUSESTS. current floor : %s , current state : %s\r\n", floor_to_str(current_floor), state_to_str(state));

      state=IDLE;
    }
    //현재 층 설정

    //**** 같은 층관 관련된 요청 우선 처리*****//
    // 내부 요청 우선 처리
    printf("====START PROCESSING SAME FLOOR REQUESTS====\r\n");
    // 같은 층 요청 처리여부 확인하는 flag
    uint8_t same_floor_request_complete_flag = 0;
    //내부요청이 있는데 현재 층과 관련된 내부요청이라면
    if (check_request(internal_requests)) {

        for(uint8_t i = 0 ; i < 3 ; i++)
        {
        // 내부 요청에서 같은 층 요청이 있다면
           // 현재 1층 (F1)-> 2
           // internal_requests[i]에 2가 있다면 같은 층 요청 있다고 판단
           if(internal_requests[i] == current_floor)
           {
             //현재 층 및 처리할 요청 출력
              printf("current floor : %s ,internal requests processing begin \r\n",floor_to_str(current_floor));
              //다음 층으로 현재 층으로 설정해서 상태 결정
              next_floor=current_floor;

              //아무것도 실행하지 않고 요청 처리
              remove_request(internal_requests, current_floor, 1);
              //같은 층과 관련된 외부요청을 전부 처리상태로 변경(삭제)
              if(check_request(up_requests) == 1 )
              {
                   remove_request(up_requests, current_floor, 1);
              }
              if(check_request(down_requests) == 1)
              {
                   remove_request(down_requests, current_floor, 1);
              }
              printf("all external requests removed same to current_floor after processing internal requests");
              same_floor_request_complete_flag=1;
              printf("current floor : %s internal_requests to process complete !! \r\n ",floor_to_str(current_floor));
              break;
           }
        }
    }
    if(!same_floor_request_complete_flag)
    {
        printf(" No (current floor : %s) internal requests \n", floor_to_str(current_floor));
    }

    //같은 층 내부 요청 없고 같은 층 외부요청이 있다면
    if((same_floor_request_complete_flag == 0) && (check_request(up_requests) || (check_request(down_requests))))
    {
        uint8_t is_request_here=0;
        for(uint8_t i=0;i<3;i++)
        {
            if(up_requests[i] == current_floor)
            {
                printf("current floor : %s , same floor external  UP requests processing begin \r\n",floor_to_str(current_floor));
                next_floor=current_floor;
                is_request_here=1;
                remove_request(up_requests,current_floor,1);
            }
            if(down_requests[i]==current_floor)
            {
                printf("current floor : %s , same floor external  DOWN requests processing begin \r\n",floor_to_str(current_floor));
                next_floor=current_floor;
                is_request_here=1;
                remove_request(down_requests,current_floor,1);
            }
        }
        if(is_request_here == 1)
        {
            door_open_close();
        }
        printf("all external requests removed same to current_floor \r\n");
        same_floor_request_complete_flag=1;
        printf("current floor : %s external requests  to process complete !! \r\n ",floor_to_str(current_floor));
    }

    if(!same_floor_request_complete_flag)
    {
        printf(" No requests related to current floor %s. Begin to check Other Floors requests \n", floor_to_str(current_floor));
        same_floor_request_complete_flag=1;
    }

    printf("====START PROCESSING OTHER FLOOR REQUSESTS ====\r\n");

    //****같은 층 요청 처리 다하고 다른 층 요청 확인 후 요청 처리 시작****//
    //같은 층 요청이 존재하지 않는디 확인
    //내부 외부 가릴 것 없이 가까운 층 확인
    if (same_floor_request_complete_flag == 1)
    {
          //같은 층 요청 전부 처리했는데  다른 층 요청도 없다면 state IDLE로 설정
        if(!check_request(internal_requests) && !check_request(up_requests) && !check_request(down_requests))
        {
            printf("NO REQUSESTS (AFTER PROCESSING INTERNAL REQUESTS)\r\n");
            state=IDLE;
        }

        //다른 층을 목표로한  내부 요청 만 존재
        if(check_request(internal_requests)&&(!check_request(up_requests) || !check_request(down_requests)))
        {
            //현재 층이 중간 층일 때 요청 처리
            //상승 방향 요청 우선 순위
            if(current_floor == 1)
            {
                //현재 층 1층일때 내부요청 중 가장 먼저 눌린 요청 실행
                printf("current_floor : %s internal request processing for other floors begin \r\n", floor_to_str(current_floor));
                //상승상태에서 현재 층 보다 낮은 내부 입력은 무시(처리 하지 않음)

				next_floor=internal_requests[0];
				set_state();
			   //remove_request(internal_requests,next_floor,1);

            }
            //현재 층이 중간 층을 제외한 층일 때
            else
            {
                printf("current_floor : %s internal request processing for other floors (only internal) begin \r\n", floor_to_str(current_floor));
                //요청 중 가장 가까운 층 결정
                next_floor=choose_closest_floor(internal_requests);
                set_state();
                if(state==UP)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_up();
                    return;
                }
                else if(state == DOWN)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_down();
                    return;
                }
                //remove_request(internal_requests,next_floor,1);
            }
        }
        //내부 요청 없고 외부요청만 있을때.
        else if(!check_request(internal_requests)&&(check_request(up_requests)||check_request(down_requests)))
        {
            //1층이라면 상승 요청을 우선순위해서 처리함
            //현재 층보다 높은 층의 요청을 우선 처리함
            printf("current floor : %s external request processing for other floors (only external)begin \r\n", floor_to_str(current_floor));
            if(current_floor == F1)
            {
                //1층에서 확인할 수 있는 외부 2층 요청은 down_requests밖에 없음
                if(check_request(down_requests))
                {
                    next_floor=choose_closest_floor(down_requests);
                    set_state();
                    //엘베 이동 시작
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_up();
                    //remove_request(down_requests,next_floor,1);
                }
                //외부 1층 요청은 up_requests밖에 없음
                else if(check_request(up_requests))
                {
                    next_floor=choose_closest_floor(up_requests);
                    set_state();
                    //엘베 이동 시작
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_down();
                    //remove_request(up_requests,next_floor,1);
                }
            }
            //현재 층 지하 1층 or 2층
            else
            {
                uint8_t closest_floor_down_requests= choose_closest_floor(down_requests);
                uint8_t closest_floor_up_requests= choose_closest_floor(up_requests);
                next_floor = (closest_floor_up_requests > closest_floor_down_requests) ? closest_floor_down_requests : closest_floor_up_requests;
                set_state();

                //엘리베이터 이동
                //set_state()에서 현재 상태 바뀜
                if(state == UP)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    prev_floor=current_floor;
                    operate_elevator_up();
                }
                else if(state == DOWN)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    prev_floor=current_floor;
                    operate_elevator_down();
                }

                //요청 처리
                (closest_floor_up_requests > closest_floor_down_requests) ? remove_request(down_requests,next_floor,1) : remove_request(up_requests,next_floor,1);
            }
        }
        //-->3층 건물이라 가능함. 중간층에 있을 때 요청 처리는 현재 프로젝트에서는 필요없지만 층 수가 늘어나면 개정 필요
        //1층이 아닌 끝 층
        else if(check_request(internal_requests)&&(check_request(up_requests)||check_request(down_requests)))
        {
            if(current_floor == F1)
            {
                // 2층관련된 요청이 없다면
                //내부 요청 우선처리 하는데 외부가 내부보다 가까울 경우 를 생각해야함
                //1층의 경우 상승 방향 우선
                if(find_request_name(internal_requests,3)|| find_request_name(down_requests,3))
                {
                    next_floor=3;
                    set_state();
                    prev_floor=current_floor;
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_down();
                }
                //지하 1층 관련 요청
                else
                {
                    next_floor=1;
                    set_state();
                    prev_floor=current_floor;
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_down();
                }
            }
            else
            {
                uint8_t closest_floor_internal_requests = choose_closest_floor(internal_requests);
                uint8_t closest_floor_up_requests = choose_closest_floor(up_requests);
                uint8_t closest_floor_down_requests = choose_closest_floor(down_requests);
                //a<b ? (a<c?a:c) : (b<c?b:c)

                next_floor = (closest_floor_internal_requests < closest_floor_up_requests) ?
                            ((closest_floor_internal_requests < closest_floor_down_requests) ?
                            closest_floor_internal_requests : closest_floor_down_requests) :
                            ((closest_floor_up_requests < closest_floor_down_requests) ?
                        closest_floor_up_requests : closest_floor_down_requests);

                set_state();

                if(state == UP)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_up();
                }
                else if(state == DOWN)
                {
                    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
                    operate_elevator_down();
                }
                prev_floor=current_floor;
                //remove_request(internal_requests,next_floor,1);
                //remove_request(up_requests,next_floor,1);
                //remove_request(down_requests,next_floor,1);
            }
        }
    }
}


//이동 상태에서 요청 처리
void request_process_MOVING(void)
{
    //main문에서는 1초마다 확인하지만 이동 중 요청 처리는 엘리베이터가 출발하고 5초후
    if(__HAL_TIM_GET_COUNTER(&htim2)==5000)
    {
        //현재 층 설정
        set_currentfloor();
        //출발한지 5초가 지났는지 확인
        if(current_floor != prev_floor)
        {

            if(state == UP)
            {
            	rotate_1cm(DIR_CCW);
                //이동 중 요청 확인
                if(find_request_name(internal_requests,current_floor))
                {
                	printf("current floor [%s] internal requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
                	while(__HAL_TIM_GET_COUNTER(&htim2) <= 6000)
                	{
                		//카운터 6000될떄까지 대기
                	}
                	HAL_Delay(1000);
                    door_open_close();
                    //현재 층과 관련된 요청 전부 처리
                    remove_request(internal_requests,next_floor,1);
					remove_request(up_requests,next_floor,1);
					remove_request(down_requests,next_floor,1);
					//내부요청으로 정지 후 현재 층보다 높은 층에 대한 요청이 있다면
					if(find_request_adove(internal_requests, current_floor) || find_request_adove(up_requests, current_floor) || find_request_adove (down_requests, current_floor))
					{
						next_floor=current_floor++;
						set_state();
						printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
						operate_elevator_up();

                        //일단은 1층을 지나가는 경우 밖에 없으니까 next_floor를

					}
					//현재 층 없다면 정지 후 IDLE 상태 처리
					else
					{
						next_floor=current_floor;
						set_state();
					}
                }
                //현재 층에 대한 상승요청이 있다면
                else if(find_request_name(up_requests, current_floor))
                {
                	//현재 층 보다 위층의 요청여부 확인
                	if(find_request_adove(internal_requests, current_floor) || find_request_adove(up_requests, current_floor) || find_request_adove (down_requests, current_floor))
					{
                		printf("current floor [%s] external up requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
		                while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
				     	{
						//카운터 6000될떄까지 대기
					    }
					 	HAL_Delay(1000);
						door_open_close();
						operate_elevator_up();
						printf("adove floor detected. current state [%s] \r\n",state_to_str(state));
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
					    printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
					}
                	//없다면
                	else
                	{
                		printf("current floor [%s] external up requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
					    while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
							//카운터 6000될떄까지 대기
						}
						HAL_Delay(1000);
						door_open_close();
						next_floor=current_floor;
						set_state();
						printf("no adove floor detected. current state [%s] \r\n",state_to_str(state));
						//현재 층과 관련된 요청 전부 처리
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
                	}
                }
                //현재 DOWN요청이 있다면 처리 보류하고 2층 요청 여부 확인
                else if(find_request_name(down_requests, current_floor))
                {
                	if(find_request_adove(internal_requests, current_floor) || find_request_adove(up_requests, current_floor) || find_request_adove (down_requests, current_floor))
					{
						printf("current floor [%s] external down requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
					    while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
					    //카운터 6000될떄까지 대기
						}
						///HAL_Delay(1000);
						//door_open_close();
						next_floor=current_floor++;
						set_state();
						operate_elevator_up();
						printf("adove floor detected. current state [%s] \r\n",state_to_str(state));
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						//remove_request(down_requests,next_floor,1);
						printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
					}
                	else
                	{
                		printf("current floor [%s] external up requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
						while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
							//카운터 6000될떄까지 대기
						}
						HAL_Delay(1000);
						door_open_close();
						next_floor=current_floor;
						set_state();
						printf("no adove floor detected. current state [%s] \r\n",state_to_str(state));
						//현재 층과 관련된 요청 전부 처리
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
                	}
                }
                //아무런 요청이 없다면
                else
                {
                	printf("no current floor [%s] requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
					while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
					{
						//카운터 6000될떄까지 대기
					}
					HAL_Delay(1000);
					door_open_close();
					next_floor=current_floor;
					set_state();
					//printf("no current floor requests detected. current state [%s] \r\n",state_to_str(state));
					//현재 층과 관련된 요청 전부 처리
					remove_request(internal_requests,next_floor,1);
					remove_request(up_requests,next_floor,1);
					remove_request(down_requests,next_floor,1);
                }

            }
            //현재 상태 하강일때 이동중 요청 처리
            else if(state == DOWN)
            {
            	rotate_1cm(DIR_CW);
				//이동 중 요청 확인
				if(find_request_name(internal_requests,current_floor))
				{
					printf("current floor [%s] internal requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
					while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
					{
						//카운터 6000될떄까지 대기
					}
					HAL_Delay(1000);
					door_open_close();
					//현재 층과 관련된 요청 전부 처리
					remove_request(internal_requests,next_floor,1);
					remove_request(up_requests,next_floor,1);
					remove_request(down_requests,next_floor,1);
					//내부요청으로 정지 후 현재 층보다 높은 층에 대한 요청이 있다면
					if(find_request_below(internal_requests, current_floor) || find_request_below(up_requests, current_floor) || find_request_below (down_requests, current_floor))
					{
						next_floor=current_floor--;
						set_state();
						printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
						operate_elevator_down();
					}
					//현재 층 없다면 정지 후 IDLE 상태 처리
					else
					{
						next_floor=current_floor;
						set_state();
					}
				}
				//현재 층에 대한 하강요청이 있다면
				else if(find_request_name(down_requests, current_floor))
				{
					//현재 층 보다 아래 층의 요청여부 확인
					if(find_request_adove(internal_requests, current_floor) || find_request_adove(up_requests, current_floor) || find_request_adove (down_requests, current_floor))
					{
						printf("current floor [%s] external down requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
						while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
						//카운터 6000될떄까지 대기
						}
						HAL_Delay(1000);
						door_open_close();
						next_floor=current_floor--;
						set_state();
						printf("adove floor detected. current state [%s] \r\n",state_to_str(state));
						operate_elevator_down();
						printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
					}
					//없다면
					else
					{
						printf("current floor [%s] external up requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
						while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
							//카운터 6000될떄까지 대기
						}
						HAL_Delay(1000);
						door_open_close();
						next_floor=current_floor;
						set_state();
						printf("no below floor detected. current state [%s] \r\n",state_to_str(state));
						//현재 층과 관련된 요청 전부 처리
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
					}
				}
				//현재 상승요청이 있다면 처리 보류하고 지하 1층 요청 여부 확인
				else if(find_request_name(up_requests, current_floor))
				{
					if(find_request_below(internal_requests, current_floor) || find_request_below(up_requests, current_floor) || find_request_below (down_requests, current_floor))
					{
						printf("current floor [%s] external down requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
						while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
						//카운터 6000될떄까지 대기
						}
						///HAL_Delay(1000);
						//door_open_close();
						next_floor=current_floor--;
						set_state();
						operate_elevator_down();
						printf("below floor detected. current state [%s] \r\n",state_to_str(state));
						remove_request(internal_requests,next_floor,1);
						//remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
						printf("elevator starts [state : %s , current_floor : %s -> next_floor : %s ]",state_to_str(state),floor_to_str(current_floor),floor_to_str(next_floor));
					}
					else
					{
						printf("current floor [%s] external up requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
						while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
						{
							//카운터 6000될떄까지 대기
						}
						HAL_Delay(1000);
						door_open_close();
						next_floor=current_floor;
						set_state();
						printf("no below floor detected. current state [%s] \r\n",state_to_str(state));
						//현재 층과 관련된 요청 전부 처리
						remove_request(internal_requests,next_floor,1);
						remove_request(up_requests,next_floor,1);
						remove_request(down_requests,next_floor,1);
					}
				}
				//아무런 요청이 없다면
				else
				{
					printf("no current floor [%s] requests detected. current state [%s]\r\n",floor_to_str(current_floor),state_to_str(state));
					while(__HAL_TIM_GET_COUNTER(&htim9) <= 6000)
					{
						//카운터 6000될떄까지 대기
					}
					HAL_Delay(1000);
					door_open_close();
					next_floor=current_floor;
					set_state();
					//printf("no current floor requests detected. current state [%s] \r\n",state_to_str(state));
					//현재 층과 관련된 요청 전부 처리
					remove_request(internal_requests,next_floor,1);
					remove_request(up_requests,next_floor,1);
					remove_request(down_requests,next_floor,1);
				}
            }
			else
			{
				 printf("SYSTEM ERROR OCCURRED. RESET THE SYSTEM \r\n");
			}
        }
    }
}

// 엘리베이터 동작 관리
void elevator_process(void)
{
	set_currentfloor();

    if (state == IDLE)
    {
       request_process_STOPPED();
    }
    else if(state == UP || state ==DOWN)
    {
       //request_process_MOVING();
    }
}

