#include "RFID.h"



// 고유 카드 번호를 비교하는 함수
int check_card_id(uint8_t* card_id)
{
    // 등록된 카드 번호와 비교
    uint8_t card1[] = CARD1_ID;
    uint8_t card2[] = CARD2_ID;

    // 첫 번째 카드 번호 비교
    if (memcmp(card_id, card1, 5) == 0)
    {
        return CARD_FOUND; // 첫 번째 카드 발견
    }

    // 두 번째 카드 번호 비교
    if (memcmp(card_id, card2, 5) == 0)
    {
        return CARD_FOUND; // 두 번째 카드 발견
    }

    return CARD_NOT_FOUND; // 등록되지 않은 카드
}

// 카드 인식 함수
int read_rfid_card(void)
{
    uint8_t str[5];  // RFID 카드 ID를 저장할 변수
    int status;

    // RFID 카드 요청 및 응답
    status = MFRC522_Request(PICC_REQIDL, str);  // 카드 인식 요청
    if (status != MI_OK)
    {
        return CARD_NOT_FOUND; // 카드가 없으면 리턴
    }

    // 카드 고유 번호 읽기
    status = MFRC522_Anticoll(str);
    if (status != MI_OK)
    {
        return CARD_NOT_FOUND; // 오류가 발생하면 리턴
    }

    // 카드 고유 번호가 등록된 번호와 일치하는지 확인
    return check_card_id(str);
}

