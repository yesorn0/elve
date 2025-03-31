/*
 * elevator.h
 *
 *  Created on: Mar 25, 2025
 *      Author: USER
 */

#ifndef INC_ELEVATOR_H_
#define INC_ELEVATOR_H_

#include "main.h"
#include "stepper.h"
#include "photo.h"
#include "button.h"
#include "door.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define R2D 3
#define R1U 2
#define R1D 2
#define RBU 1

#define RF2 3
#define RF1 2
#define RB1 1

typedef enum {
    IDLE,
    DOWN,
    UP,
} ElevatorState;

typedef enum {
    B1 = 1,
    F1 = 2,
    F2 = 3
} ElevatorFloor;

// 전역 배열 선언
extern uint8_t internal_requests[3];
extern uint8_t up_requests[3];
extern uint8_t down_requests[3];
extern uint8_t can_operate_flag;
extern uint8_t current_floor;       // 현재 층 (정적 변수로 외부 접근 제한)
extern uint8_t next_floor;

// 함수 선언
uint8_t * get_internal_requests(void);
uint8_t * get_up_requests(void);
uint8_t * get_down_requests(void);
void elevator_process(void);
void set_currentstate(void);
void door_open_close(void);
void request_process(void);
void push_requests(uint8_t *requests, uint8_t operator);
//void pop_requests(uint8_t *requests);
uint8_t check_request(uint8_t *requests);
int find_request(uint8_t *requests, uint8_t floor);  // 추가
void remove_request(uint8_t *requests, uint8_t floor, uint8_t is_processed);  // 통합, is_processed 추가

#endif /* INC_ELEVATOR_H_ */
