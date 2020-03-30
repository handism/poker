#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * �W���[�J�[�̖�����0~2���܂ł̊ԂŕύX�ł��܂��B
 * �ύX����ꍇ�ATRUMPS�̒l��52~54�ɁA
 * trumps[]��500�̒l�̐���0~2�ɕύX���Ă��������B
 */

/* �}�N����` */
#define TRUMPS 54 // �g�����v�̑�����
#define HANDS 5
#define NUMBERS 14 // 0 = JOKER�A1~13
#define STATIC1_ID 201
#define STATIC1_TEXT ""
#define STATIC2_ID 202
#define STATIC2_TEXT "��"
#define BUTTON1_CAPTION "�h���["
#define BUTTON2_CAPTION "���Z�b�g"
#define BUTTON1_ID 101
#define BUTTON2_ID 102
#define CHECK1_ID 301
#define CHECK2_ID 302
#define CHECK3_ID 303
#define CHECK4_ID 304
#define CHECK5_ID 305

// �X�[�g
#define SUIT 0
#define CLUB 1
#define CLUB_TEXT "��"
#define DIA 2
#define DIA_TEXT "��"
#define HEART 3
#define HEART_TEXT "��"
#define SPADE 4
#define SPADE_TEXT "��"
#define JOKER_SUIT 5
#define JOKER_TEXT "��"

// �G�D
#define ACE 1
#define ACE_TEXT "A"
#define JACK 11
#define JACK_TEXT "J"
#define QUEEN 12
#define QUEEN_TEXT "Q"
#define KING 13
#define KING_TEXT "K"
#define JOKER_NUMBER 0

// ��
#define FIVE_OF_A_KIND 10
#define ROYAL_STRAIGHT_FLUSH 9
#define STRAIGHT_FLUSH 8
#define FOUR_OF_A_KIND 7
#define FULL_HOUSE 6
#define FLUSH 5
#define STRAIGHT 4
#define THREE_OF_A_KIND 3
#define TWO_PAIR 2
#define ONE_PAIR 1
#define NO_PAIR 0

/* �v���g�^�C�v�錾 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int shuffle();
int showHands();
int draw();
int judgeHand();
int CreateWnd();
int CreateObj();
int reset();

/* �g�����v��` */
int trumps[] =
{
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, // �N���u
    201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, // �_�C��
    301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, // �n�[�g
    401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, // �X�y�[�h
    500, 500                                                         // �W���[�J�[
};

/* �O���[�o���ϐ� */
WNDCLASS wc; // �E�B���h�E�N���X
MSG msg; // ���b�Z�[�W
HINSTANCE hIns;
HWND hwnd;
HWND static1;
HWND static2;
HWND button1;
HWND button2;
HWND check1;
HWND check2;
HWND check3;
HWND check4;
HWND check5;

int deck[TRUMPS]; // �R�D
int hands[HANDS]; // ��D
int draw_times;   // �h���[��
int deck_p;       // �R�D���ǂ��܂ň��������H

/*******************************
 * ���C���֐��i��ԍŏ��ɌĂ΂��j
 *******************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    int i;
    hIns = hInstance;

    // �E�B���h�E�쐬
    CreateWnd();

    // �I�v�W�F�N�g�쐬
    CreateObj();

    // �����Z�b�g
    reset();

    // ���b�Z�[�W���[�v
    while(GetMessage(&msg, NULL, 0, 0))
    {
        // ���b�Z�[�W���E�B���h�E�v���V�[�W���ɂ��n��
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

/*******************************
 * �E�B���h�E�v���V�[�W��
 *******************************/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    int retBtn;
    int result;

    switch(msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_COMMAND:
            switch(LOWORD(wp))
            {
                case BUTTON1_ID: // �h���[�{�^��������
                    // �`�F�b�N�{�^��1
                    if(SendMessage(check1, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[0] = draw();
                    }
                    // �`�F�b�N�{�^��2
                    if(SendMessage(check2, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[1] = draw();
                    }
                    // �`�F�b�N�{�^��3
                    if(SendMessage(check3, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[2] = draw();
                    }
                    // �`�F�b�N�{�^��4
                    if(SendMessage(check4, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[3] = draw();
                    }
                    // �`�F�b�N�{�^��5
                    if(SendMessage(check5, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[4] = draw();
                    }

                    ShowWindow(button1, SW_HIDE);

                    // ��D���ĕ\��
                    showHands();

                    // ���̔���
                    result = judgeHand();

                    switch(result)
                    {
                        case FIVE_OF_A_KIND:
                            SetWindowText(static2, "5�J�[�h�I");
                            break;
                        case ROYAL_STRAIGHT_FLUSH:
                            SetWindowText(static2, "���C�����X�g���[�g�t���b�V���I");
                            break;
                        case STRAIGHT_FLUSH:
                            SetWindowText(static2, "�X�g���[�g�t���b�V���I");
                            break;
                        case FOUR_OF_A_KIND:
                            SetWindowText(static2, "4�J�[�h�I");
                            break;
                        case FULL_HOUSE:
                            SetWindowText(static2, "�t���n�E�X�I");
                            break;
                        case FLUSH:
                            SetWindowText(static2, "�t���b�V���I");
                            break;
                        case STRAIGHT:
                            SetWindowText(static2, "�X�g���[�g�I");
                            break;
                        case THREE_OF_A_KIND:
                            SetWindowText(static2, "3�J�[�h�I");
                            break;
                        case TWO_PAIR:
                            SetWindowText(static2, "2�y�A�I");
                            break;
                        case ONE_PAIR:
                            SetWindowText(static2, "1�y�A�I");
                            break;
                    }
                    break;
                case BUTTON2_ID: // ���Z�b�g�{�^��������
                    retBtn = MessageBox(hwnd, "���Z�b�g���Ă�\n��낵���ł����H", "���Z�b�g", MB_YESNO | MB_ICONQUESTION);
                    if(retBtn == IDYES)
                    {
                        reset();
                    }
                    break;
            }
            return 0;
        case WM_CREATE:
            break;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
}

/*******************************
 * �h���[�{�^��������
 *******************************/
int draw()
{
    int trump;

    // �R�D����1������
    trump = deck[deck_p];
    deck_p++;
    
    return trump;
}

/*******************************
 * ���Z�b�g�{�^��������
 *******************************/
int reset()
{
    int i;

    // ��ʏ�����
    SetWindowText(static1, "");
    SetWindowText(static2, "");
    SendMessage(check1, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check2, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check3, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check4, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check5, BM_SETCHECK, BST_UNCHECKED, 0);

    // �V���b�t��
    shuffle();

    // 5��h���[
    for(i = 0; i < 5; i++)
    {
        hands[i] = draw();
    }

    // �g�����v�\��
    showHands();

    if(IsWindowVisible(button1) == 0)
    {
        ShowWindow(button1, SW_SHOW);
    }

    return 0;
}

/*******************************
 * �E�B���h�E�쐬
 *******************************/
int CreateWnd()
{
    // ���C���E�B���h�E�N���X�̒�`
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hIns;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "TEST";

    // �E�B���h�E�N���X�̓o�^
    if(!RegisterClass(&wc))
    {
        return -1;
    }

    // �E�B���h�E�쐬�J�n
    hwnd = CreateWindow("Test", "��Poker��", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hIns, NULL);
    if(hwnd == NULL)
    {
        return -1;
    }

    return 0;
}

/*******************************
 * �I�u�W�F�N�g�쐬
 *******************************/
int CreateObj()
{
    // �{�^��1
    // �������Ɛe�E�B���h�E�̃E�B���h�E�v���V�[�W����WM_COMMAND�𑗂�
    button1 = CreateWindow("BUTTON", BUTTON1_CAPTION, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 400, 100, 50, hwnd, (HMENU)BUTTON1_ID, hIns, NULL);

    // �{�^��2
    button2 = CreateWindow("BUTTON", BUTTON2_CAPTION, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 600, 400, 100, 50, hwnd, (HMENU)BUTTON2_ID, hIns, NULL);

    // �X�^�e�B�b�N
    static1 = CreateWindow("STATIC", STATIC1_TEXT, WS_CHILD | WS_VISIBLE | SS_CENTER, 360, 100, 300, 30, hwnd, (HMENU)STATIC1_ID, hIns, NULL);

    // �X�^�e�B�b�N
    static2 = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 360, 130, 300, 30, hwnd, (HMENU)STATIC2_ID, hIns, NULL);

    // �`�F�b�N�{�b�N�X
    check1 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 400, 200, 20, 20, hwnd, (HMENU)CHECK1_ID, hIns, NULL);

    // �`�F�b�N�{�b�N�X
    check2 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 450, 200, 20, 20, hwnd, (HMENU)CHECK2_ID, hIns, NULL);

    // �`�F�b�N�{�b�N�X
    check3 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 500, 200, 20, 20, hwnd, (HMENU)CHECK3_ID, hIns, NULL);

    // �`�F�b�N�{�b�N�X
    check4 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 550, 200, 20, 20, hwnd, (HMENU)CHECK4_ID, hIns, NULL);

    // �`�F�b�N�{�b�N�X
    check5 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 600, 200, 20, 20, hwnd, (HMENU)CHECK5_ID, hIns, NULL);

    return 0;
}

/*******************************
 * �V���b�t�����ĎR�D���쐬
 *******************************/
int shuffle()
{
    int r1;
    int r2;
    int i;
    int tmp;

    srand(time(NULL));

    for(i = 0; i < TRUMPS; i++)
    {
        deck[i] = trumps[i];
    }

    // 1����R�D�������_���ɓ���ւ���Ώ[�������_�����낤
    for(i = 0; i < 10000; i++)
    {
        r1 = rand() % TRUMPS;
        r2 = rand() % TRUMPS;

        tmp = deck[r1];
        deck[r1] = deck[r2];
        deck[r2] = tmp;
    }

    deck_p = 0;

    return 0;
}

/*******************************
 * ��D����ʂɕ\��
 *******************************/
int showHands()
{
    int i;
    char strText[32768];
    char tmpText[6666];
    char suit[999];
    char number[999];
    int suit_i;
    int number_i;

    memset(strText, '\0', sizeof(strText));

    for(i = 0; i < HANDS; i++)
    {
        memset(suit, '\0', sizeof(suit));
        memset(number, '\0', sizeof(number));
        memset(tmpText, '\0', sizeof(tmpText));
        suit_i = hands[i] / 100;
        number_i = hands[i] % 100;

        // �X�[�g�ϊ�
        switch(suit_i)
        {
            case CLUB:
                strcpy(suit, CLUB_TEXT);
                break;
            case DIA:
                strcpy(suit, DIA_TEXT);
                break;
            case HEART:
                strcpy(suit, HEART_TEXT);
                break;
            case SPADE:
                strcpy(suit, SPADE_TEXT);
                break;
            case JOKER_SUIT:
                strcpy(suit, JOKER_TEXT);
                break;
            default:
                break;
        }

        // �����ϊ�
        switch(number_i)
        {
            case ACE:
                strcpy(number, ACE_TEXT);
                break;
            case JACK:
                strcpy(number, JACK_TEXT);
                break;
            case QUEEN:
                strcpy(number, QUEEN_TEXT);
                break;
            case KING:
                strcpy(number, KING_TEXT);
                break;
            case JOKER_NUMBER:
                strcpy(number, "");
                break;
            default:
                sprintf(number, "%d", number_i);
                break;
        }
        sprintf(tmpText, "%s%s�@", suit, number);
        strcat(strText, tmpText);
    }

    SetWindowText(static1, strText);

    return 0;
}

/*******************************
 * ���̔���
 *******************************/
int judgeHand()
{
    int i;
    int j;
    int suit_i[6];
    int number_i[NUMBERS];
    int num_max = 0;
    int serial_cnt = 0;
    int pair_cnt = 0;
    int straight_flg = 0;
    int joker_cnt = 0;
    int joker_cnt_tmp = 0;
    int loyal_cnt = 0;

    // ������
    for(i = 0; i < 6; i++)
    {
        suit_i[i] = 0;
    }

    for(i = 0; i < HANDS; i++)
    {
        suit_i[hands[i] / 100]++;
        number_i[hands[i] % 100]++;
    }

    joker_cnt = number_i[0];
    joker_cnt_tmp = number_i[0];

    // �������͂����H
    // JOKER�͕ʏ����Ȃ̂�1����
    for(i = 1; i < NUMBERS; i++)
    {
        if(num_max < number_i[i])
        {
            num_max = number_i[i];
        }

        // �X�g���[�g�`�F�b�N
        if(number_i[i] == 1)
        {
            serial_cnt++;
            // 10�ȍ~�ɍēx�������������ꍇ�A�W���[�J�[���Z�b�g
            if((serial_cnt == 1)  && (i > 9))
            {
                joker_cnt_tmp = number_i[0];
            }
            if(serial_cnt == 5)
            {
                straight_flg = 1;
            }
        }
        else
        {
            // joker����̏ꍇ��joker�̐�����΂�
            if((joker_cnt_tmp > 0) && (serial_cnt > 0))
            {
                joker_cnt_tmp--;
                serial_cnt++;
                if(serial_cnt == 5)
                {
                    straight_flg = 1;
                }
            }
            else // joker�Ȃ��A�܂���joker�̉񐔂��g���؂����ꍇ
            {
                serial_cnt = 0;
            }
        }
    }

    // 13 | 12,13 | 11,12,13 | 10,11,12,13 �̏ꍇ�͍ăX�g���[�g�`�F�b�N
    // JOKER�͕ʏ����Ȃ̂�1����
    for(i = 1; i < NUMBERS; i++)
    {
        if(number_i[i] == 1)
        {
            serial_cnt++;
            if(serial_cnt == 5)
            {
                straight_flg = 1;
            }
        }
        else // joker�Ȃ��A�܂���joker�̉񐔂��g���؂����ꍇ
        {
            serial_cnt = 0;
            break;
        }
    }

    // �����̃_�u��`�F�b�N
    num_max += joker_cnt; // joker�����l��
    switch(num_max)
    {
        case 5:
            // 5�J�[�h
            return FIVE_OF_A_KIND;
        case 4:
            // 4�J�[�h
            return FOUR_OF_A_KIND;
        case 3:
            if(joker_cnt == 2)
            {
                // �������A�W���[�J�[2���̏ꍇ�͔��f�ۗ�
                break;
            }
            else if(joker_cnt == 1) // �W���[�J�[1���̏ꍇ
            {
                for(i = 0; i < NUMBERS; i++)
                {
                    if(number_i[i] == 2)
                    {
                        pair_cnt++;
                    }
                }
                // �y�A��2�Ȃ�t���n�E�X
                if(pair_cnt == 2)
                {
                    return FULL_HOUSE;
                }
                else // �����łȂ�����3�J�[�h
                {
                    return THREE_OF_A_KIND;
                }
            }
            else // �W���[�J�[�Ȃ��̏ꍇ
            {
                for(i = 0; i < NUMBERS; i++)
                {
                    // �c��̓y�A�H
                    if(number_i[i] == 2)
                    {
                        return FULL_HOUSE;
                    }
                }
                return THREE_OF_A_KIND;
            }
        case 2:
            if(joker_cnt == 1)
            {
                // �������A�W���[�J�[1���̏ꍇ�͔��f�ۗ�
                break;
            }
            for(i = 0; i < NUMBERS; i++)
            {
                if(number_i[i] == 2)
                {
                    pair_cnt++;
                }
            }
            // �y�A�͉��H
            if(pair_cnt == 2)
            {
                return TWO_PAIR;
            }
            else
            {
                return ONE_PAIR;
            }
        case 1:
            break;
        default:
            break;
    }

    // �t���b�V�����H
    for(i = 0; i < 6; i++)
    {
        if(suit_i[i] == (5 - joker_cnt))
        {
            // �X�g���[�g�t���b�V�����H
            if(straight_flg == 1)
            {
                // �܂����A���C����!?
                // 10~13�̑���
                for(i = 10; i < NUMBERS; i++)
                {
                    if(number_i[i] == 1)
                    {
                        loyal_cnt++;
                    }
                }
                if(number_i[1] == 1)
                {
                    loyal_cnt++;
                }

                // 10,11,12,13,1,joker�����v5������΃��C�����̉\��
                if((loyal_cnt + joker_cnt) == 5)
                {
                    return ROYAL_STRAIGHT_FLUSH;
                }
                else
                {
                    return STRAIGHT_FLUSH;
                }
            }
            else
            {
                return FLUSH;
            }   
        }
    }

    // �X�g���[�g���H
    if(straight_flg == 1)
    {
        return STRAIGHT;
    }

    // �����܂ŗ�����c�O��
    switch(joker_cnt)
    {
        case 2:
            return THREE_OF_A_KIND;
        case 1:
            return ONE_PAIR;
        default:
            return NO_PAIR;
    }
}