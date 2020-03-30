#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
 * ジョーカーの枚数を0~2枚までの間で変更できます。
 * 変更する場合、TRUMPSの値を52~54に、
 * trumps[]の500の値の数を0~2個に変更してください。
 */

/* マクロ定義 */
#define TRUMPS 54 // トランプの総枚数
#define HANDS 5
#define NUMBERS 14 // 0 = JOKER、1~13
#define STATIC1_ID 201
#define STATIC1_TEXT ""
#define STATIC2_ID 202
#define STATIC2_TEXT "役"
#define BUTTON1_CAPTION "ドロー"
#define BUTTON2_CAPTION "リセット"
#define BUTTON1_ID 101
#define BUTTON2_ID 102
#define CHECK1_ID 301
#define CHECK2_ID 302
#define CHECK3_ID 303
#define CHECK4_ID 304
#define CHECK5_ID 305

// スート
#define SUIT 0
#define CLUB 1
#define CLUB_TEXT "■"
#define DIA 2
#define DIA_TEXT "◆"
#define HEART 3
#define HEART_TEXT "●"
#define SPADE 4
#define SPADE_TEXT "▲"
#define JOKER_SUIT 5
#define JOKER_TEXT "★"

// 絵札
#define ACE 1
#define ACE_TEXT "A"
#define JACK 11
#define JACK_TEXT "J"
#define QUEEN 12
#define QUEEN_TEXT "Q"
#define KING 13
#define KING_TEXT "K"
#define JOKER_NUMBER 0

// 役
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

/* プロトタイプ宣言 */
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int shuffle();
int showHands();
int draw();
int judgeHand();
int CreateWnd();
int CreateObj();
int reset();

/* トランプ定義 */
int trumps[] =
{
    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, // クラブ
    201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 213, // ダイヤ
    301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, // ハート
    401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, // スペード
    500, 500                                                         // ジョーカー
};

/* グローバル変数 */
WNDCLASS wc; // ウィンドウクラス
MSG msg; // メッセージ
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

int deck[TRUMPS]; // 山札
int hands[HANDS]; // 手札
int draw_times;   // ドロー回数
int deck_p;       // 山札をどこまで引いたか？

/*******************************
 * メイン関数（一番最初に呼ばれる）
 *******************************/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    int i;
    hIns = hInstance;

    // ウィンドウ作成
    CreateWnd();

    // オプジェクト作成
    CreateObj();

    // 環境リセット
    reset();

    // メッセージループ
    while(GetMessage(&msg, NULL, 0, 0))
    {
        // メッセージをウィンドウプロシージャにも渡す
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

/*******************************
 * ウィンドウプロシージャ
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
                case BUTTON1_ID: // ドローボタン押下時
                    // チェックボタン1
                    if(SendMessage(check1, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[0] = draw();
                    }
                    // チェックボタン2
                    if(SendMessage(check2, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[1] = draw();
                    }
                    // チェックボタン3
                    if(SendMessage(check3, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[2] = draw();
                    }
                    // チェックボタン4
                    if(SendMessage(check4, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[3] = draw();
                    }
                    // チェックボタン5
                    if(SendMessage(check5, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
                    {
                        hands[4] = draw();
                    }

                    ShowWindow(button1, SW_HIDE);

                    // 手札を再表示
                    showHands();

                    // 役の判定
                    result = judgeHand();

                    switch(result)
                    {
                        case FIVE_OF_A_KIND:
                            SetWindowText(static2, "5カード！");
                            break;
                        case ROYAL_STRAIGHT_FLUSH:
                            SetWindowText(static2, "ロイヤルストレートフラッシュ！");
                            break;
                        case STRAIGHT_FLUSH:
                            SetWindowText(static2, "ストレートフラッシュ！");
                            break;
                        case FOUR_OF_A_KIND:
                            SetWindowText(static2, "4カード！");
                            break;
                        case FULL_HOUSE:
                            SetWindowText(static2, "フルハウス！");
                            break;
                        case FLUSH:
                            SetWindowText(static2, "フラッシュ！");
                            break;
                        case STRAIGHT:
                            SetWindowText(static2, "ストレート！");
                            break;
                        case THREE_OF_A_KIND:
                            SetWindowText(static2, "3カード！");
                            break;
                        case TWO_PAIR:
                            SetWindowText(static2, "2ペア！");
                            break;
                        case ONE_PAIR:
                            SetWindowText(static2, "1ペア！");
                            break;
                    }
                    break;
                case BUTTON2_ID: // リセットボタン押下時
                    retBtn = MessageBox(hwnd, "リセットしても\nよろしいですか？", "リセット", MB_YESNO | MB_ICONQUESTION);
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
 * ドローボタン押下時
 *******************************/
int draw()
{
    int trump;

    // 山札から1枚引く
    trump = deck[deck_p];
    deck_p++;
    
    return trump;
}

/*******************************
 * リセットボタン押下時
 *******************************/
int reset()
{
    int i;

    // 画面初期化
    SetWindowText(static1, "");
    SetWindowText(static2, "");
    SendMessage(check1, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check2, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check3, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check4, BM_SETCHECK, BST_UNCHECKED, 0);
    SendMessage(check5, BM_SETCHECK, BST_UNCHECKED, 0);

    // シャッフル
    shuffle();

    // 5回ドロー
    for(i = 0; i < 5; i++)
    {
        hands[i] = draw();
    }

    // トランプ表示
    showHands();

    if(IsWindowVisible(button1) == 0)
    {
        ShowWindow(button1, SW_SHOW);
    }

    return 0;
}

/*******************************
 * ウィンドウ作成
 *******************************/
int CreateWnd()
{
    // メインウィンドウクラスの定義
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

    // ウィンドウクラスの登録
    if(!RegisterClass(&wc))
    {
        return -1;
    }

    // ウィンドウ作成開始
    hwnd = CreateWindow("Test", "■Poker■", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hIns, NULL);
    if(hwnd == NULL)
    {
        return -1;
    }

    return 0;
}

/*******************************
 * オブジェクト作成
 *******************************/
int CreateObj()
{
    // ボタン1
    // 押されると親ウィンドウのウィンドウプロシージャにWM_COMMANDを送る
    button1 = CreateWindow("BUTTON", BUTTON1_CAPTION, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 300, 400, 100, 50, hwnd, (HMENU)BUTTON1_ID, hIns, NULL);

    // ボタン2
    button2 = CreateWindow("BUTTON", BUTTON2_CAPTION, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON, 600, 400, 100, 50, hwnd, (HMENU)BUTTON2_ID, hIns, NULL);

    // スタティック
    static1 = CreateWindow("STATIC", STATIC1_TEXT, WS_CHILD | WS_VISIBLE | SS_CENTER, 360, 100, 300, 30, hwnd, (HMENU)STATIC1_ID, hIns, NULL);

    // スタティック
    static2 = CreateWindow("STATIC", "", WS_CHILD | WS_VISIBLE | SS_CENTER, 360, 130, 300, 30, hwnd, (HMENU)STATIC2_ID, hIns, NULL);

    // チェックボックス
    check1 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 400, 200, 20, 20, hwnd, (HMENU)CHECK1_ID, hIns, NULL);

    // チェックボックス
    check2 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 450, 200, 20, 20, hwnd, (HMENU)CHECK2_ID, hIns, NULL);

    // チェックボックス
    check3 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 500, 200, 20, 20, hwnd, (HMENU)CHECK3_ID, hIns, NULL);

    // チェックボックス
    check4 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 550, 200, 20, 20, hwnd, (HMENU)CHECK4_ID, hIns, NULL);

    // チェックボックス
    check5 = CreateWindow("BUTTON", "", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, 600, 200, 20, 20, hwnd, (HMENU)CHECK5_ID, hIns, NULL);

    return 0;
}

/*******************************
 * シャッフルして山札を作成
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

    // 1万回山札をランダムに入れ替えれば充分ランダムだろう
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
 * 手札を画面に表示
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

        // スート変換
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

        // 数字変換
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
        sprintf(tmpText, "%s%s　", suit, number);
        strcat(strText, tmpText);
    }

    SetWindowText(static1, strText);

    return 0;
}

/*******************************
 * 役の判定
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

    // 初期化
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

    // 同じ数はいくつ？
    // JOKERは別処理なので1から
    for(i = 1; i < NUMBERS; i++)
    {
        if(num_max < number_i[i])
        {
            num_max = number_i[i];
        }

        // ストレートチェック
        if(number_i[i] == 1)
        {
            serial_cnt++;
            // 10以降に再度引っかかった場合、ジョーカーリセット
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
            // joker入りの場合はjokerの数分飛ばす
            if((joker_cnt_tmp > 0) && (serial_cnt > 0))
            {
                joker_cnt_tmp--;
                serial_cnt++;
                if(serial_cnt == 5)
                {
                    straight_flg = 1;
                }
            }
            else // jokerなし、またはjokerの回数を使い切った場合
            {
                serial_cnt = 0;
            }
        }
    }

    // 13 | 12,13 | 11,12,13 | 10,11,12,13 の場合は再ストレートチェック
    // JOKERは別処理なので1から
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
        else // jokerなし、またはjokerの回数を使い切った場合
        {
            serial_cnt = 0;
            break;
        }
    }

    // 数字のダブりチェック
    num_max += joker_cnt; // joker分を考慮
    switch(num_max)
    {
        case 5:
            // 5カード
            return FIVE_OF_A_KIND;
        case 4:
            // 4カード
            return FOUR_OF_A_KIND;
        case 3:
            if(joker_cnt == 2)
            {
                // ただし、ジョーカー2枚の場合は判断保留
                break;
            }
            else if(joker_cnt == 1) // ジョーカー1枚の場合
            {
                for(i = 0; i < NUMBERS; i++)
                {
                    if(number_i[i] == 2)
                    {
                        pair_cnt++;
                    }
                }
                // ペアが2個ならフルハウス
                if(pair_cnt == 2)
                {
                    return FULL_HOUSE;
                }
                else // そうでないから3カード
                {
                    return THREE_OF_A_KIND;
                }
            }
            else // ジョーカーなしの場合
            {
                for(i = 0; i < NUMBERS; i++)
                {
                    // 残りはペア？
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
                // ただし、ジョーカー1枚の場合は判断保留
                break;
            }
            for(i = 0; i < NUMBERS; i++)
            {
                if(number_i[i] == 2)
                {
                    pair_cnt++;
                }
            }
            // ペアは何個？
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

    // フラッシュか？
    for(i = 0; i < 6; i++)
    {
        if(suit_i[i] == (5 - joker_cnt))
        {
            // ストレートフラッシュか？
            if(straight_flg == 1)
            {
                // まさか、ロイヤル!?
                // 10~13の走査
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

                // 10,11,12,13,1,jokerが合計5枚あればロイヤルの可能性
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

    // ストレートか？
    if(straight_flg == 1)
    {
        return STRAIGHT;
    }

    // ここまで来たら残念賞
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