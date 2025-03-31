

#ifndef INC_RFID_H_
#define INC_RFID_H_

#include  "RC522.h"
#include "string.h"
#include  "door.h"

// 고유 카드 번호 설정 (등록된 카드 번호 예시)
#define CARD1_ID {53, 35, 14, 173, 181}
#define CARD2_ID {83, 127, 245, 18, 203}

// 리턴 값 정의
#define CARD_NOT_FOUND 0
#define CARD_FOUND 1

int check_card_id(uint8_t* card_id);
int read_rfid_card(void);
void read_check();

#endif /* INC_RFID_H_ */
