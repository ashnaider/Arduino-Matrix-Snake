#include <Arduino.h>
#include <MD_MAX72xx.h>

#define DEBUG 1

#if DEBUG
#define PRINT(s, v)         \
	{                       \
		Serial.print(F(s)); \
		Serial.print(v);    \
	} // Print a string followed by a value (decimal)

#define PRINTX(s, v)          \
	{                         \
		Serial.print(F(s));   \
		Serial.print(v, HEX); \
	} // Print a string followed by a value (hex)

#define PRINTS(s)           \
	{                       \
		Serial.print(F(s)); \
	} // Print a string
#else
#define PRINT(s, v)	 // Print a string followed by a value (decimal)
#define PRINTX(s, v) // Print a string followed by a value (hex)
#define PRINTS(s)	 // Print a string
#endif

// #define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW  // GOOD one
// #define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW // GOON one too

// #define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW

// #define HARDWARE_TYPE MD_MAX72XX::DR0CR0RR0_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR0CR0RR1_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR0CR1RR0_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR0CR1RR1_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR0_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR1CR0RR1_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR1CR1RR0_HW
// #define HARDWARE_TYPE MD_MAX72XX::DR1CR1RR1_HW


#define MAX_DEVICES 1
#define CLK_PIN 2  // or SCK
#define DATA_PIN 4 // or MOSI
#define CS_PIN 3   // or SS

MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES); // Arbitrary pins


#define LEFT_SWITCH 8
#define RIGHT_SWITCH 6
#define UP_SWITCH 10
#define DOWN_SWITCH 9

#define DELAYTIME 100
#define BTN_DELAY 130
#define GAME_SPEED 500

#define BOARD_WIDTH 8
#define BOARD_HEIGHT 8


void resetDisplay(void)
{
	mx.control(MD_MAX72XX::INTENSITY, MAX_INTENSITY / 2);
	mx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
	mx.clear();
}

void scrollText(const char *p, uint8_t delay_time)
{
	uint8_t charWidth;
	uint8_t cBuf[8]; // this should be ok for all built-in fonts

	PRINTS("\nScrolling text");
	mx.clear();

	while (*p != '\0')
	{
		charWidth = mx.getChar(*p++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);


		for (uint8_t i = 0; i <= charWidth; i++) // allow space between characters
		{
			mx.transform(MD_MAX72XX::TSU);
			if (i < charWidth)	
				mx.setRow(BOARD_HEIGHT - 1, cBuf[i]);
				
			delay(delay_time);
		}
	}
}


void WriteChar(char ch) {
	uint8_t charWidth;
	uint8_t cBuf[8]; // this should be ok for all built-in fonts

	mx.clear();

	charWidth = mx.getChar(ch, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);

	for (uint8_t i = 0; i <= charWidth; i++) // allow space between characters
	{
		if (i < charWidth)	
			mx.setRow(i + 1, cBuf[i]);
	}

}


template <typename T>
class LinkedListNode
{

public:
	LinkedListNode(T new_val) : value(new_val) {}

	T value;

	LinkedListNode *next = nullptr;
	LinkedListNode *prev = nullptr;
};

template <typename T>
class LinkedList
{

public:
	LinkedListNode<T> *head = nullptr;
	LinkedListNode<T> *tail = nullptr;

	T GetFront() const
	{
		if (head == nullptr)
		{
			return T();
		}

		return head->value;
	}

	T GetLast() const
	{
		if (tail == nullptr)
		{
			return T();
		}

		return tail->value;
	}

	bool Contains(T val)
	{
		if (head == nullptr)
		{
			return false;
		}

		LinkedListNode<T> *tmp = head;
		while (tmp != nullptr)
		{
			if (tmp->value == val)
			{
				return true;
			}
			tmp = tmp->prev;
		}

		return false;
	}

	bool ContainsButNotHead(T val)
	{
		if (tail == nullptr)
		{
			return false;
		}

		LinkedListNode<T> *tmp = tail;
		while (tmp != head)
		{
			if (tmp->value == val)
			{
				return true;
			}
			tmp = tmp->next;
		}

		return false;
	}

	void PushFront(T new_val)
	{
		if (head == nullptr)
		{
			head = new LinkedListNode<T>(new_val);
			tail = head;
		}
		else
		{
			head->next = new LinkedListNode<T>(new_val);
			head->next->prev = head;
			head = head->next;
		}
	}

	void PushBack(T new_val)
	{
		if (tail == nullptr)
		{
			tail = new LinkedListNode<T>(new_val);
			head = tail;
		}
		else
		{
			tail->prev = new LinkedListNode<T>(new_val);
			tail->prev->next = tail;
			tail = tail->prev;
		}
	}

	void PopFront()
	{
		if (head == nullptr)
		{
			return;
		}
		else
		{
			if (head->prev == nullptr)
			{
				delete head;
				head = nullptr;
				tail = nullptr;
			}
			else
			{
				head = head->prev;
				delete head->next;
				head->next = nullptr;
			}
		}
	}

	void PopBack()
	{
		if (tail == nullptr)
		{
			return;
		}
		else
		{
			if (tail->next == nullptr)
			{
				delete tail;
				tail = nullptr;
				head = nullptr;
			}
			else
			{
				tail = tail->next;
				delete tail->prev;
				tail->prev = nullptr;
			}
		}
	}

	void Clear() {
		if (tail == nullptr) { return ;}

		LinkedListNode<T> *tmp = tail;
		while (tmp != head) {
			tmp = tmp->next;
			delete tmp->prev;
		}

		delete head;

		head = nullptr;
		tail = nullptr;
	}
};

struct Point
{
public:
	int8_t x;
	int8_t y;

	Point()
	{
		x = -1;
		y = -1;
	};
	Point(int8_t x, int8_t y) : x(x), y(y) {}
};

bool operator==(const Point &lhs, const Point &rhs)
{
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

enum SNAKE_DIRECTION
{
	LEFT=0,
	RIGHT,
	UP=3,
	DOWN,
	NONE=5
};


bool isContrDir(SNAKE_DIRECTION dir1, SNAKE_DIRECTION dir2) {
	if (dir1 == NONE || dir2 == NONE) { return false;}
	
	return (abs(dir1 - dir2) == 1);
}

class Snake
{
private:
	int8_t x;
	int8_t y;

	int8_t board_w;
	int8_t board_h;

	int8_t apples_eaten;

	int8_t last_x;
	int8_t last_y;

	int8_t head, tail;

	bool has_eaten = false;

	SNAKE_DIRECTION currDirection = NONE;

public:
	LinkedList<Point> snake_itself;

	Snake(int8_t board_width, int8_t board_height) : board_w(board_width),
													 board_h(board_height)
	{
		x = board_w / 2 - 1;
		y = board_h / 2 - 1;

		last_x = x - 2;
		last_y = y;

		apples_eaten = 0;

		snake_itself.PushFront(Point(x, y));
		snake_itself.PushFront(Point(x - 1, y));
		snake_itself.PushFront(Point(x - 2, y));
	}

	~Snake() {
		snake_itself.Clear();
	}

	Point GetCurrPos() const
	{
		return snake_itself.GetFront();
	}

	Point GetLast() const
	{
		return snake_itself.GetLast();
	}

	SNAKE_DIRECTION GetDirection() {
		return currDirection;
	}

	void SetDirection(SNAKE_DIRECTION newDir)
	{
		if (isContrDir(currDirection, newDir)) { return ; }
		currDirection = newDir;
	}

	void Eat()
	{
		has_eaten = true;
		++apples_eaten;
	}

	int ApplesEaten() {
		return apples_eaten;
	}

	void Move()
	{
		switch (currDirection)
		{
		case SNAKE_DIRECTION::LEFT:
			MoveLeft();
			break;

		case SNAKE_DIRECTION::RIGHT:
			MoveRight();
			break;

		case SNAKE_DIRECTION::UP:
			MoveUp();
			break;

		case SNAKE_DIRECTION::DOWN:
			MoveDown();
			break;

		default:
			break;
		}

		snake_itself.PushFront(Point(x, y));
		snake_itself.PopBack();
	}

private:
	void CheckBoundaries()
	{
		if (y == board_h)
		{
			y = 0;
		}
		else if (y < 0)
		{
			y = board_h - 1;
		}

		if (x < 0)
		{
			x = board_w - 1;
		}
		else if (x == board_w)
		{
			x = 0;
		}
	}

	void MoveUp()
	{
		--y;
		CheckBoundaries();
	}

	void MoveDown()
	{
		++y;
		CheckBoundaries();
	}

	void MoveLeft()
	{
		--x;
		CheckBoundaries();
	}

	void MoveRight()
	{
		++x;
		CheckBoundaries();
	}
};

void drawBall(Point point, bool bOn = true)
{
	mx.setPoint(point.x, point.y, bOn);
}

Point GetRandomPoint(Snake *snake, int8_t w, int8_t h)
{
	uint8_t x, y;

	while (true)
	{
		x = random(0, w);
		y = random(0, h);

		if (!snake->snake_itself.Contains(Point(x, y)))
		{
			break;
		}
	}

	return Point(x, y);
}

Snake *snake = nullptr;
Point apple;

enum GAME_STATE
{
	INIT,
	PLAY,
	SHOW_RESULT
};

GAME_STATE currGameState = INIT;


void setup(void)
{
	mx.begin();
	
	resetDisplay();

	pinMode(LEFT_SWITCH, INPUT_PULLUP);
	pinMode(RIGHT_SWITCH, INPUT_PULLUP);
	pinMode(UP_SWITCH, INPUT_PULLUP);
	pinMode(DOWN_SWITCH, INPUT_PULLUP);
}

uint32_t last_time_btn_pressed = 0;
uint32_t last_time_game_changed = 0;

bool just_eaten_apple = false;


inline bool LEFT_PRESSED(void) { return (digitalRead(LEFT_SWITCH) == LOW); }
inline bool RIGHT_PRESSED(void) { return (digitalRead(RIGHT_SWITCH) == LOW); }
inline bool UP_PRESSED(void) { return (digitalRead(UP_SWITCH) == LOW); }
inline bool DOWN_PRESSED(void) { return (digitalRead(DOWN_SWITCH) == LOW); }

void loop(void)
{

	if (currGameState == INIT)
	{
		if (LEFT_PRESSED() || RIGHT_PRESSED() || UP_PRESSED() || DOWN_PRESSED())
		{
			resetDisplay();
			snake = new Snake(BOARD_WIDTH, BOARD_HEIGHT);
			apple = GetRandomPoint(snake, BOARD_WIDTH, BOARD_HEIGHT);

			drawBall(apple, true);
			currGameState = PLAY;
		}
	}
	else if (currGameState == PLAY)
	{
		if (millis() - last_time_btn_pressed > BTN_DELAY)
		{

			if (LEFT_PRESSED())
			{
				PRINTS("LEFT pressed\n");
				snake->SetDirection(LEFT);
			}
			else if (RIGHT_PRESSED())
			{
				PRINTS("RIGHT !!! pressed\n");
				snake->SetDirection(RIGHT);
			}
			else if (UP_PRESSED())
			{
				PRINTS("UP pressed\n");
				snake->SetDirection(UP);
			}
			else if (DOWN_PRESSED())
			{
				PRINTS("DOWN pressed\n");
				snake->SetDirection(DOWN);
			}

			last_time_btn_pressed = millis();
		}

		if (millis() - last_time_game_changed > GAME_SPEED)
		{

			Point last = snake->GetLast();

			snake->Move();

			// if (snake->snake_itself.Contains(apple))
			if (snake->GetCurrPos() == apple)
			{
				just_eaten_apple = true;
				snake->Eat();
				snake->snake_itself.PushBack(last);
				drawBall(apple, false);
				apple = GetRandomPoint(snake, BOARD_WIDTH, BOARD_HEIGHT);
				drawBall(apple, true);
			}

			if (snake->snake_itself.ContainsButNotHead(snake->GetCurrPos()))
			{
				currGameState = SHOW_RESULT;
				return;
			}

			last_time_game_changed = millis();

			drawBall(snake->GetCurrPos(), true);
			
			if (just_eaten_apple) {
				just_eaten_apple = false;
				return ;
			}

			drawBall(last, false);
		}
	}

	else if (currGameState == SHOW_RESULT)
	{
		resetDisplay();
		delay(500);
		scrollText("GAME OVER! ", 80);
		mx.clear();

		int total_points = snake->ApplesEaten();

		if (total_points <= 9) {
			// char buff[] = {"   "};
			// buff[1] = total_points + 48;
			// scrollText(buff);
			WriteChar(total_points + 48);
			delay(2000);
			mx.clear();

		} else {
			char buff[] = {"      "};
			buff[2] = (total_points / 10) + 48;
			buff[3] = (total_points % 10) + 48;

			for (short i = 0; i < 3; ++i) {
				scrollText(buff, 100);
			}
			mx.clear();
		}

		delete snake;
		snake = nullptr;
		currGameState = INIT;
	}
}
