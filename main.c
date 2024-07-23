/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 4

#define ARRAY_LENGTH(arr) (sizeof(arr)/sizeof(arr[0]))
#define DEBOUNCE_TIME 100
#define MAX_MUSIC 4


typedef unsigned char byte; // byte is not defined here by default


byte snakeBody[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

byte snakeBodyleft[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11101,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};
byte snakeBodyup[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11011,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};
byte snakeBodydown[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b11111,
  0b11011,
  0b11111,
  0b00000,
  0b00000
};


byte snakeBodyright[8] = {
  0b00000,
  0b00000,
  0b11111,
  0b10111,
  0b11111,
  0b11111,
  0b00000,
  0b00000
};

byte apple[8] = {
  0b00000,
  0b00010,
  0b01100,
  0b01010,
  0b01110,
  0b00000,
  0b00000,
  0b00000
};

byte luckBox[8] = {
  0b11111,
  0b10001,
  0b10101,
  0b10101,
  0b10001,
  0b11111,
  0b00100,
  0b00000
};

byte obstacle[] = {
  0b00000,
  0b01110,
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b01110,
  0b00000
};

byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

typedef struct {
    int x;
    int y;
} Point;

Point snake[BOARD_WIDTH * BOARD_HEIGHT];
int snake_length = 4;  // Initial length of the snake
Point apple2;
Point heart2;
int score = 0;
int health = 3; // Initial health
int mode = 2; // Current game mode


uint8_t data[100], d, letter=0;
uint32_t chanceboxTime=0;
int chanceboxflag=1;


int dx = 1, dy = 0; // Initial direction (moving right)

Point luckBox2;
int luckBoxMode;  // 0 for points, 1 for health
int luckBoxVisible = 0;  // 0 for not visible, 1 for visible
int lastLuckBoxTime = 0;
int luckBoxDisplayTime = 0;

int MAX_OBSTACLES=6;

Point obstacles[6];
int lastObstacleUpdateTime = 0;
uint32_t obstacleTime=0;
int obstacleflag=1;

uint16_t snake_speed = 500; // Initial speed in milliseconds

int init2=0, second1_10=0, minute=0;
uint32_t time_minute=0;
int name_flag=0, game_over_flag=0;
uint32_t lastHeartEatenTime = 0;
int heartEaten = 0;
uint32_t lastAppleEatenTime = 0;
appleEaten = 0;

GPIO_TypeDef *const Row_ports[] = {GPIOC, GPIOC, GPIOC, GPIOB};
const uint16_t Row_pins[] = {GPIO_PIN_3, GPIO_PIN_1, GPIO_PIN_7, GPIO_PIN_4};
GPIO_TypeDef *const Column_ports[] = {GPIOB, GPIOB, GPIOC, GPIOC};
const uint16_t Column_pins[] = {GPIO_PIN_5, GPIO_PIN_8, GPIO_PIN_6, GPIO_PIN_4};
volatile uint32_t last_gpio_exti;

extern TIM_HandleTypeDef htim2;
TIM_HandleTypeDef *pwm_timer= &htim2;
uint32_t pwm_channel = TIM_CHANNEL_2;


typedef struct
{
    uint16_t frequency;
    uint16_t duration;
} Tone;

int music_number=0;
int turn_m[MAX_MUSIC];

const Tone *volatile melody_ptr;
volatile uint16_t melody_tone_count=0;
volatile uint16_t current_tone_number=1;
volatile uint32_t current_tone_end=0;
int volume =10;
volatile uint16_t pre_volume;
volatile uint32_t last_butten_pressed;



// Set RTC time
   RTC_TimeTypeDef mytime;
   char timestr[20];

typedef enum {
    STATE_INITIAL_PAGE,
    STATE_GAME_MENU,
    STATE_GAME,
    STATE_SETTINGS,
    STATE_SELECT_MODE,
    STATE_ABOUT,
	STATE_GAME_OVER
} GameState;

GameState current_state = STATE_INITIAL_PAGE;

// Function to display the initial page
void Display_Initial_Page(void) {
	setCursor(0, 0);
	write("                                                                                ");
    setCursor(5, 0);
    print("Snake Game");
    setCursor(3, 1);
    print("Press any key");
    setCursor(6, 2);
    print("to start");
}

const char* menu_options[] = {
    "1. Start Game       ",
    "2. Settings         ",
    "3. Select Mode      ",
    "4. About            "
};
int current_option = 0;
// Function to display the game menu
void Display_Game_Menu(void) {
	setCursor(0, 0);
	write("                                                                                ");
    for (int i = 0; i < 4; i++) {
        setCursor(0, i);
        if (i == current_option) {
            print("> ");
        } else {
            print("  ");
        }
        print(menu_options[i]);
    }
}
// Function to navigate through the menu options
void Navigate_Menu(int direction) {
    current_option += direction;
    if (current_option < 0) {
        current_option = 3;
    } else if (current_option > 3) {
        current_option = 0;
    }
}
// Function to select the current menu option
void Select_Menu_Option(void) {
    switch (current_option) {

        case 0: // Start Game
        	current_state=STATE_GAME;
        	Game_Init();
            break;
        case 1: // Settings
        	current_state = STATE_SETTINGS;
        	name_flag=1;

            break;
        case 2: // Select Mode
        	current_state = STATE_SELECT_MODE;
            break;
        case 3: // About
        	current_state = STATE_ABOUT;
            break;
    }
}
// Function to display the about page
void Display_About_Page(void)
{
	setCursor(0, 0);
	write("                                                                                ");
    setCursor(0, 0);
    print("Group Members:      ");
    setCursor(0, 1);
    print("-ReyhanehYeganeh");
    setCursor(0, 2);
    print("-SeyedehZahraHossini");

}

typedef struct {
    uint8_t health;
    uint8_t speed;
    uint8_t sound_effects; // 0: Off, 1: On
    uint8_t num_obstacles;
    char player_name[20];
} GameSettings;

GameSettings settings = {5, 3, 1, 4, "Player"};


#define SETTINGS_COUNT 5
#define DISPLAY_ROWS 3

uint8_t current_setting = 0;
uint8_t display_start = 0;

// Function to update the settings display
void Update_Settings_Display(void)
{
	if (name_flag){
		char buffer[]="enter player name:\n";
		HAL_UART_Transmit(&huart2, buffer, strlen(buffer), HAL_MAX_DELAY);
		name_flag=0;
	}
	setCursor(0, 0);
	write("                                                                                ");
	setCursor(0, 0);
	print("Settings:           ");

    char buffer[20];
    for (int i = 0; i < DISPLAY_ROWS; i++) {
    	setCursor(19, i + 1);
    	print(" ");
        setCursor(0, i + 1);
        int setting_index = display_start + i;
		if (setting_index >= SETTINGS_COUNT) {
			print("                    ");
		} else {
						switch (setting_index) {
				case 0:
					sprintf(buffer, "Health: %d          ", settings.health);
					break;
				case 1:
					sprintf(buffer, "Speed: %d           ", settings.speed);
					break;
				case 2:
					sprintf(buffer, "Sound: %s          ", settings.sound_effects ? "On" : "Off");
					break;
				case 3:
					sprintf(buffer, "Obstacles: %d       ", settings.num_obstacles);
					break;
				case 4:
					sprintf(buffer, "Name: %s           ", settings.player_name);
					break;
			}
			print(buffer);
			if (setting_index  == current_setting) {
				setCursor(19, i + 1);
				print("<");
			}
		}
    }
}

// Function to adjust the current setting
void Adjust_Setting(int8_t direction)
{

    switch (current_setting) {
        case 0:
            settings.health += direction;
            if (settings.health > 5) settings.health = 5;
            if (settings.health < 1) settings.health = 1;
            break;
        case 1:
            settings.speed += direction;
            if (settings.speed > 10) settings.speed = 10;
            if (settings.speed < 1) settings.speed = 1;
            break;
        case 2:
            settings.sound_effects = !settings.sound_effects;
            break;
        case 3:
            settings.num_obstacles += direction;
            if (settings.num_obstacles > 6) settings.num_obstacles = 6;
            if (settings.num_obstacles < 0) settings.num_obstacles = 0;
            break;
        case 4:
            // For simplicity, adjust player name in another function
            break;
    }
}


#define MODE_COUNT 3

const char* game_modes[MODE_COUNT] = {"   1           ", "   2             ", "   3         "};

uint8_t current_mode = 0;

// Function to update the mode display
void Update_Mode_Display(void)
{

	setCursor(0, 0);
	print("Select Mode:           ");
	for (int i = 0; i < MODE_COUNT; i++) {
		setCursor(19, i + 1);
		print(" ");
		setCursor(0, i + 1);
		print(game_modes[i]);
		if (i == current_mode) {
			setCursor(19, i + 1);
			print("<");
		}
	}
}
// Function to display the game over screen
void Display_Game_Over(void)
{
	if (game_over_flag) {
		setCursor(0, 0);
		print("                                                                                ");

		char s[100];
		sprintf(s, "GAME OVER\nName: %s\nScore: %d", settings.player_name, score);
		HAL_UART_Transmit(&huart2, s, strlen(s), HAL_MAX_DELAY);

		game_over_flag=0;
	}

	setCursor(0, 0);
    print("GAME OVER");


    char str[20];

    sprintf(str, "Name: %s", settings.player_name);
    setCursor(0, 1);
    print(str);

    sprintf(str, "Score: %d", score);
    setCursor(0, 2);
    print(str);


}



void Generate_New_LuckBox(void) {
	int x, y;
	    do {
	        x = rand() % BOARD_WIDTH;
	        y = rand() % BOARD_HEIGHT;
	    } while (!Is_Empty_Position(x, y));

	    luckBox2.x = x;
	    luckBox2.y = y;
	    luckBoxVisible = 1;  // Show the luck box
	    luckBoxMode = rand() % 3;  // Randomly select luck box mode
}

void Generate_New_Heart(void) {
	int x, y;
	    do {
	        x = rand() % BOARD_WIDTH;
	        y = rand() % BOARD_HEIGHT;
	    } while (!Is_Empty_Position(x, y));

	    heart2.x = x;
	    heart2.y = y;
}
void Generate_New_Apple(void) {
	int x, y;
	    do {
	        x = rand() % BOARD_WIDTH;
	        y = rand() % BOARD_HEIGHT;
	    } while (!Is_Empty_Position(x, y));

	    apple2.x = x;
	    apple2.y = y;
}

void Generate_New_Obstacle(int index) {
    int x, y;
    do {
        x = rand() % BOARD_WIDTH;
        y = rand() % BOARD_HEIGHT;
    } while (!Is_Empty_Position(x, y));

    obstacles[index].x = x;
    obstacles[index].y = y;
}

void Generate_Initial_Obstacles(void) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        Generate_New_Obstacle(i);
    }
}


// Function to initialize the game
void Game_Init(void) {
    snake[0].x = 7;
    snake[0].y = 0;
    snake[1].x = 6;
    snake[1].y = 0;
    snake[2].x = 5;
    snake[2].y = 0;
    snake[3].x = 4;
    snake[3].y = 0;

    health = settings.health;
    mode = current_mode+1;
    MAX_OBSTACLES = settings.num_obstacles;
    snake_speed = settings.speed;
    score=0;
    time_minute=HAL_GetTick();
    minute=0;

    melody_tone_count=0;
    current_tone_number=1;
    current_tone_end=0;
    PWM_Cange_Tone(0, 0);


    Generate_Initial_Obstacles();
    Generate_New_Apple();
    Generate_New_Heart(); // Add this line
    Generate_Initial_Obstacles();

    init2=1;

}

void Manage_Apple(void) {

    if (appleEaten && HAL_GetTick() - lastAppleEatenTime >= 5000) {
        Generate_New_Apple();
        appleEaten = 0;
    }
}

void Manage_Heart(void) {

    if (heartEaten && HAL_GetTick() - lastHeartEatenTime >= 5000) {
        Generate_New_Heart();
        heartEaten = 0;
    }
}

void Manage_LuckBox(void) {

    if ((chanceboxflag || HAL_GetTick() - chanceboxTime >= 10000) && !luckBoxVisible) {

        luckBoxVisible = 1;
        Generate_New_LuckBox();
        chanceboxflag=0;
        chanceboxTime=HAL_GetTick();
    }

    if (luckBoxVisible && HAL_GetTick() - chanceboxTime >= 20000) {
    	setCursor(luckBox2.x, luckBox2.y);
    	write(' ');
        luckBoxVisible = 0;
        chanceboxTime=HAL_GetTick();
    }
}

void Manage_Obstacles(void) {

    if (obstacleflag || HAL_GetTick() - obstacleTime >= 10000) {

        int obstacleToRemove = rand() % MAX_OBSTACLES;
        setCursor(obstacles[obstacleToRemove].x, obstacles[obstacleToRemove].y);
        write(' ');

        Generate_New_Obstacle(obstacleToRemove);

        obstacleTime = HAL_GetTick();
        obstacleflag=0;
    }
}

int Is_Empty_Position(int x, int y) {
    // Check if position is occupied by the snake
    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return 0;
        }
    }

    // Check if position is occupied by an apple
    if (x == apple2.x && y == apple2.y) {
        return 0;
    }

    // Check if position is occupied by a heart
    if (x == heart2.x && y == heart2.y) {
        return 0;
    }

    // Check if position is occupied by the luck box
    if (luckBoxVisible && x == luckBox2.x && y == luckBox2.y) {
        return 0;
    }

    // Check if position is occupied by an obstacle
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (x == obstacles[i].x && y == obstacles[i].y) {
            return 0;
        }
    }

    return 1;
}

int speed=0;

void Move_Snake(void) {
    static uint32_t lastSpeedIncreaseTime = 0; // Time when the speed was last increased

    if (mode == 3) {
        // Increase speed every 10 seconds
        if (HAL_GetTick() - lastSpeedIncreaseTime >= 10000) {
            if (snake_speed < 10) {
                snake_speed++;
            }
            lastSpeedIncreaseTime = HAL_GetTick();
        }
    }

    if (speed > 9) {
        speed = 1;
    } else {
        speed++;
    }

    if (10 - snake_speed <= speed) {
        speed = 0;
        Point prev_positions = snake[snake_length - 1];

        for (int i = snake_length - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }

        // Check if the next position has an obstacle
        int next_x = snake[0].x + dx;
        int next_y = snake[0].y + dy;
        int obstacleCollision = 0;

        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (next_x == obstacles[i].x && next_y == obstacles[i].y) {
                obstacleCollision = 1;
                break;
            }
        }

        // If there is an obstacle, turn 90 degrees to the right
        if (obstacleCollision) {
            if (dx != 0) {
                int temp = dx;
                dx = 0;
                dy = (temp == 1) ? -1 : 1;
            } else if (dy != 0) {
                int temp = dy;
                dy = 0;
                dx = (temp == 1) ? 1 : -1;
            }

            health--;
            if (health == 0) {
                // Game over logic
                return;
            }

            if (settings.sound_effects)
                Update_music(4);

        } else {
            // Update the head position
            snake[0].x = next_x;
            snake[0].y = next_y;

            // Handle wall collisions based on mode
            if (mode == 1 || mode == 3) {
                // Mode 1 and Mode 3: Wrap around the edges
                if (snake[0].x < 0) snake[0].x = BOARD_WIDTH - 1;
                if (snake[0].x >= BOARD_WIDTH) snake[0].x = 0;
                if (snake[0].y < 0) snake[0].y = BOARD_HEIGHT - 1;
                if (snake[0].y >= BOARD_HEIGHT) snake[0].y = 0;
            } else if (mode == 2) {
                // Mode 2: Deduct health and turn 90 degrees right on wall collision
                if (snake[0].x < 0 || snake[0].x >= BOARD_WIDTH || snake[0].y < 0 || snake[0].y >= BOARD_HEIGHT) {
                    health--;

                    if (health == 0) {
                        // Game over logic here
                        return;
                    } else {
                        // Turn 90 degrees to the right
                        if (dx != 0) {
                            int temp = dx;
                            dx = 0;
                            dy = (temp == 1) ? -1 : 1;

                            if (temp == 1) {
                                snake[0].x = BOARD_WIDTH - 1;
                            } else {
                                snake[0].x = 0;
                            }
                            snake[0].y += dy;

                        } else if (dy != 0) {
                            int temp = dy;
                            dy = 0;
                            dx = (temp == 1) ? 1 : -1;

                            if (temp == 1) {
                                snake[0].y = BOARD_HEIGHT - 1;
                            } else {
                                snake[0].y = 0;
                            }
                            snake[0].x += dx;
                        }
                    }
                }
            }

            // Clear the previous positions of the snake
            setCursor(prev_positions.x, prev_positions.y);
            write(' ');

            // Check for collisions with self
            for (int i = 1; i < snake_length; i++) {
                if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
                    health--;
                    if (health == 0) {
                        // Game over logic here
                        return;
                    }
                    if (settings.sound_effects)
                        Update_music(4);
                }
            }

            // Check for collisions with heart
            if (snake[0].x == heart2.x && snake[0].y == heart2.y) {
                setCursor(heart2.x, heart2.y);
                write(' ');

                if (health < 99)
                    health++;
                lastHeartEatenTime = HAL_GetTick();
                heartEaten = 1;
                if (settings.sound_effects)
                    Update_music(3);
            }

            // Check for apple
            if (snake[0].x == apple2.x && snake[0].y == apple2.y) {
                setCursor(apple2.x, apple2.y);
                write(' ');
                snake_length++;
                score += 1;
                if (settings.sound_effects)
                    Update_music(2);
                lastAppleEatenTime = HAL_GetTick();
                appleEaten = 1;
            }

            // Check for luck box
            if (luckBoxVisible && snake[0].x == luckBox2.x && snake[0].y == luckBox2.y) {
                setCursor(luckBox2.x, luckBox2.y);
                write(' ');
                if (luckBoxMode == 0) {
                    score += 2;
                } else if (luckBoxMode == 1) {
                    health += 2;
                    if (health > 99)
                        health = 99;
                } else if (luckBoxMode == 2) {
                    snake_speed += 2;
                    if (snake_speed > 10)
                        snake_speed = 10;
                }
                chanceboxTime=HAL_GetTick();
                luckBoxVisible = 0; // Hide the luck box after collision
                if (settings.sound_effects)
                    Update_music(5);
            }

            // Check for collisions with obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                if (snake[0].x == obstacles[i].x && snake[0].y == obstacles[i].y) {
                    // Should not reach here due to obstacle check before updating position
                    // Health decrease logic
                    health--;
                    if (health == 0) {
                        // Game over logic
                        return;
                    }


                    if (settings.sound_effects)
                        Update_music(4);
                }
            }
        }

      
    }
}





void LCD_Update(void) {

	if (init2) {
		setCursor(0, 0);
		print("                                                                                ");
		init2=0;
		char s[100];
		sprintf(s, "GAME Init\nName: %s\n", settings.player_name);
		HAL_UART_Transmit(&huart2, s, strlen(s), HAL_MAX_DELAY);
	}

	setCursor(0, 0);
	char health_display[16];
	sprintf(health_display, "H:%2d", health);
	print(health_display);

	for (int i = 0; i < snake_length; i++) {
		setCursor(snake[i].x, snake[i].y);

		if (i > 0) {
			// Write the snake head
			write(1); // Assuming 0 is the character code for snake head
		} else {
			// Determine the direction of the current segment
			int prev_x = snake[i + 1].x;
			int prev_y = snake[i + 1].y;
			int curr_x = snake[i].x;
			int curr_y = snake[i].y;

			if (curr_x > prev_x) {
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_10);
				// Moving right
				write(0); // Assuming 1 is the character code for snake body
			} else if (curr_x < prev_x) {
				HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_11);
				// Moving left
				write(8); // Assuming 8 is the character code for snake body left
			} else if (curr_y > prev_y) {
				// Moving down
				write(7); // Assuming 7 is the character code for snake body down
			} else if (curr_y < prev_y) {
				// Moving up
				write(6); // Assuming 6 is the character code for snake body up
			}
		}
	}

	// Update the apple position
	if (!appleEaten) {
		setCursor(apple2.x, apple2.y);
		write(2);
	}

	// Update the heart position
	if (!heartEaten) {
		setCursor(heart2.x, heart2.y);
		write(5);
	}

	// Update the luckbox position if visible
	if (luckBoxVisible) {
		setCursor(luckBox2.x, luckBox2.y);
		write(3); // Assuming 6 is the character code for luck box
	}

	// Update the obstacles positions
	for (int i = 0; i < MAX_OBSTACLES; i++) {
		setCursor(obstacles[i].x, obstacles[i].y);
		write(4); // Assuming 7 is the character code for obstacle
	}


}

////////////////////////////////// 7 segments


#define D1_HIGH HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);
#define D1_LOW HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);

#define D2_HIGH HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
#define D2_LOW HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);

#define D3_HIGH HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_SET);
#define D3_LOW HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);

#define D4_HIGH HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);
#define D4_LOW HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);

#define P_HIGH HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
#define P_LOW HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);


uint32_t startTime=0;
uint8_t turn=1;
uint8_t temp1=1, temp2=2, temp3=3, temp4=4;
uint8_t display2=0;

uint8_t segmentNumber[10]={
			0x00,
			0x01,
			0x02,
			0x03,
			0x04,
			0x05,
			0x06,
			0x07,
			0x08,
			0x09
};

void SevenSegment_Update(uint8_t number){
	uint8_t segmentPattern = segmentNumber[number];
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, ((segmentPattern >> 0) & 0x01));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, ((segmentPattern >> 1) & 0x01));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, ((segmentPattern >> 2) & 0x01));
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, ((segmentPattern >> 3) & 0x01));


}



void refresh_7segments () {


	temp1 = (score / 10) % 10;
	temp2 = score % 10;
	temp3 = (minute / 10) % 10;
	temp4 = minute % 10;

	if (turn==1 && ((HAL_GetTick() - startTime) > 5)) {
		D4_LOW;
		SevenSegment_Update(temp1);  // Display the thousands digit
		D1_HIGH;
		turn++;
		startTime = HAL_GetTick();
		P_HIGH;
	}

	if (turn==2 && ((HAL_GetTick() - startTime) > 5)) {
		D1_LOW;
		SevenSegment_Update(temp2);  // Display the thousands digit
		D2_HIGH;
		turn++;
		startTime = HAL_GetTick();
		P_LOW;
	}

	if (turn==3 && ((HAL_GetTick() - startTime) > 5)) {
		D2_LOW;
		SevenSegment_Update(temp3);  // Display the hundreds digit
		D3_HIGH;
		turn++;
		startTime = HAL_GetTick();
		P_HIGH;
	}

	if (turn==4 && ((HAL_GetTick() - startTime) > 5)) {
		D3_LOW;
		SevenSegment_Update(temp4);  // Display the ones digit
		D4_HIGH;
		turn=1;
		startTime = HAL_GetTick();
		P_HIGH;
	}
}

/////////////////////////////////////////


#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
#define REST      0



const Tone super_mario_bros[] = {
	{2637,306}, // E7 x2
	{   0,153}, // x3 <-- Silence
	{2637,153}, // E7
	{   0,153}, // x3
	{2093,153}, // C7
	{2637,153}, // E7
	{   0,153}, // x3
	{3136,153}, // G7
	{   0,459}, // x3
	{1586,153}, // G6
	{   0,459}, // x3

	{2093,153}, // C7
	{   0,306}, // x2
	{1586,153}, // G6
	{   0,306}, // x2
	{1319,153}, // E6
	{   0,306}, // x2
	{1760,153}, // A6
	{   0,153}, // x1
	{1976,153}, // B6
	{   0,153}, // x1
	{1865,153}, // AS6
	{1760,153}, // A6
	{   0,153}, // x1

	{1586,204}, // G6
	{2637,204}, // E7
	{3136,204}, // G7
	{3520,153}, // A7
	{   0,153}, // x1
	{2794,153}, // F7
	{3136,153}, // G7
	{   0,153}, // x1
	{2637,153}, // E7
	{   0,153}, // x1
	{2093,153}, // C7
	{2349,153}, // D7
	{1976,153}, // B6
	{   0,306}, // x2

	{2093,153}, // C7
	{   0,306}, // x2
	{1586,153}, // G6
	{   0,306}, // x2
	{1319,153}, // E6
	{   0,306}, // x2
	{1760,153}, // A6
	{   0,153}, // x1
	{1976,153}, // B6
	{   0,153}, // x1
	{1865,153}, // AS6
	{1760,153}, // A6
	{   0,153}, // x1

	{1586,204}, // G6
	{2637,204}, // E7
	{3136,204}, // G7
	{3520,153}, // A7
	{   0,153}, // x1
	{2794,153}, // F7
	{3136,153}, // G7
	{   0,153}, // x1
	{2637,153}, // E7
	{   0,153}, // x1
	{2093,153}, // C7
	{2349,153}, // D7
	{1976,153}, // B6

	{   0,  0}	// <-- Disable PWM
};

const Tone apple_music[] = {
    // 1 -> 1666           \        -1666 -> 2499           \    1660 -> 1494, 2499 -> 2249
    // 2 -> 833     --------\        -833 -> 1249    --------\    833 -> 750, 1249 -> 1124
    // 4 -> 416     --------/        -416 -> 624     --------/    416 -> 374, 624 -> 561
    // 8 -> 208                /        -208 -> 312                /    208 -> 187, 312 -> 281

    {REST, 500},    // Initial rest to start

    {NOTE_C4, 250}, // Bite
    {NOTE_E4, 250}, // Chew
    {NOTE_G4, 500}, // Chew
    {REST, 250},    // Pause

    {NOTE_B4, 250}, // Bite
    {NOTE_G4, 250}, // Chew
    {NOTE_E4, 500}, // Chew
    {REST, 250},    // Pause

    {NOTE_C5, 250}, // Bite
    {NOTE_E5, 250}, // Chew
    {NOTE_G5, 500}, // Chew
    {REST, 250},    // Pause

    {NOTE_B4, 250}, // Bite
    {NOTE_G4, 250}, // Chew
    {NOTE_E4, 500}, // Chew
    {REST, 250},    // Pause

    {NOTE_C4, 250}, // Bite
    {NOTE_E4, 250}, // Chew
    {NOTE_G4, 500}, // Chew
    {REST, 500},    // Pause

    {0, 0}          // End of sequence
};

const Tone heart_music[] = {
    // 1 -> 1666           \        -1666 -> 2499           \    1660 -> 1494, 2499 -> 2249
    // 2 -> 833     --------\        -833 -> 1249    --------\    833 -> 750, 1249 -> 1124
    // 4 -> 416     --------/        -416 -> 624     --------/    416 -> 374, 624 -> 561
    // 8 -> 208                /        -208 -> 312                /    208 -> 187, 312 -> 281

    {REST, 500}, // Initial rest

    {NOTE_AS4, 250}, // Start with a lively note
    {NOTE_F5, 500},  // Higher note for excitement
    {NOTE_E5, 250},  // Another lively note
    {REST, 250},     // Brief pause

    {NOTE_AS4, 250}, // Repeat with slight variation
    {NOTE_G5, 500},  // Change to a different high note
    {NOTE_E5, 250},  // Consistent lively note
    {REST, 250},     // Brief pause

    {NOTE_C5, 250},  // Add some variation
    {NOTE_F5, 500},  // Return to a high note
    {NOTE_D5, 250},  // Add another note for variation
    {REST, 250},     // Brief pause

    {NOTE_AS4, 250}, // Back to starting note
    {NOTE_F5, 500},  // End with a high note for emphasis
    {NOTE_E5, 250},  // Consistent lively note
    {REST, 500},     // Final rest to end

    {0, 0} // End of sequence
};

const Tone obstacle_music[] = {
    // 1 -> 1666           \        -1666 -> 2499           \    1660 -> 1494, 2499 -> 2249
    // 2 -> 833     --------\        -833 -> 1249    --------\    833 -> 750, 1249 -> 1124
    // 4 -> 416     --------/        -416 -> 624     --------/    416 -> 374, 624 -> 561
    // 8 -> 208                /        -208 -> 312                /    208 -> 187, 312 -> 281

    {REST, 200}, // Initial rest

    {NOTE_E4, 150}, // Short, sharp note
    {REST, 100},    // Brief rest
    {NOTE_E4, 150}, // Repeated note
    {REST, 100},    // Brief rest
    {NOTE_E4, 300}, // Longer note for emphasis
    {REST, 200},    // Brief rest

    {NOTE_G4, 150}, // Higher note for variety
    {REST, 100},    // Brief rest
    {NOTE_G4, 150}, // Repeated note
    {REST, 100},    // Brief rest
    {NOTE_G4, 300}, // Longer note for emphasis
    {REST, 200},    // Brief rest

    {NOTE_E4, 150}, // Return to initial note
    {REST, 100},    // Brief rest
    {NOTE_E4, 150}, // Repeated note
    {REST, 100},    // Brief rest
    {NOTE_E4, 300}, // Longer note for final emphasis
    {REST, 500},    // Final rest to end

    {0, 0} // End of sequence
};

const Tone luckbox_music[] = {
    // 1 -> 1666           \        -1666 -> 2499           \    1660 -> 1494, 2499 -> 2249
    // 2 -> 833     --------\        -833 -> 1249    --------\    833 -> 750, 1249 -> 1124
    // 4 -> 416     --------/        -416 -> 624     --------/    416 -> 374, 624 -> 561
    // 8 -> 208                /        -208 -> 312                /    208 -> 187, 312 -> 281

    {REST, 500},    // Initial rest to start

    {NOTE_C4, 200}, // Lucky note
    {NOTE_E4, 200}, // Lucky note
    {NOTE_G4, 200}, // Lucky note
    {NOTE_C5, 400}, // Higher note for luck

    {REST, 250},    // Pause

    {NOTE_B4, 200}, // Lucky note
    {NOTE_G4, 200}, // Lucky note
    {NOTE_E4, 200}, // Lucky note
    {NOTE_C4, 400}, // Return to C4 for completion

    {REST, 250},    // Pause

    {NOTE_E5, 200}, // Higher note
    {NOTE_D5, 200}, // Descending note
    {NOTE_C5, 200}, // Descending note
    {NOTE_B4, 400}, // Longer note

    {REST, 250},    // Pause

    {NOTE_G4, 200}, // Lucky note
    {NOTE_A4, 200}, // Ascending note
    {NOTE_B4, 200}, // Ascending note
    {NOTE_C5, 400}, // Higher note for luck

    {REST, 500},    // Final pause

    {0, 0}          // End of sequence
};



void PWM_Start() {
    // Start the PWM signal generation on the specified timer and channel
    HAL_TIM_PWM_Start(pwm_timer, pwm_channel);
}

void PWM_Cange_Tone(uint16_t pwm_freq, uint16_t volume) {
    if (pwm_freq == 0 || pwm_freq > 20000) {
        // If the frequency is 0 or exceeds 20 kHz, stop the PWM signal by setting compare value to 0
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, 0);
    } else {
        // Get the internal clock frequency
        const uint32_t internal_clock_freq = HAL_RCC_GetSysClockFreq();

        // Calculate the prescaler value based on the desired PWM frequency
        const uint32_t prescaler = 1 + internal_clock_freq / pwm_freq / 60000;
        // Calculate the timer clock frequency after applying the prescaler
        const uint32_t timer_clock = internal_clock_freq / prescaler;
        // Calculate the period cycles based on the timer clock and desired PWM frequency
        const uint32_t period_cycles = timer_clock / pwm_freq;
        // Calculate the pulse width based on the volume and period cycles
        const uint32_t pulse_width = volume * period_cycles / 1000 / 2;

        // Set the timer prescaler value
        pwm_timer->Instance->PSC = prescaler - 1;
        // Set the auto-reload register value
        pwm_timer->Instance->ARR = period_cycles - 1;
        // Generate an update event to apply the new settings
        pwm_timer->Instance->EGR = TIM_EGR_UG;

        // Set the compare value to adjust the duty cycle
        __HAL_TIM_SET_COMPARE(pwm_timer, pwm_channel, pulse_width);
    }
}

void Update_music(int input) {
    // Change the melody based on the input value
    switch(input) {
        case 1:
            Change_Melody(super_mario_bros, ARRAY_LENGTH(super_mario_bros));
            break;
        case 2:
            Change_Melody(apple_music, ARRAY_LENGTH(apple_music));
            break;
        case 3:
            Change_Melody(heart_music, ARRAY_LENGTH(heart_music));
            break;
        case 4:
            Change_Melody(obstacle_music, ARRAY_LENGTH(obstacle_music));
            break;
        case 5:
            Change_Melody(luckbox_music, ARRAY_LENGTH(luckbox_music));
            break;
    }
}

void Change_Melody(const Tone *melody, uint16_t tone_count) {
    // Set the melody pointer and tone count to the new melody
    melody_ptr = melody;
    melody_tone_count = tone_count;
    // Reset the current tone number to start from the beginning
    current_tone_number = 0;
}

void Update_melody() {
    // Check if the current tone duration has ended and there are more tones to play
    if ((HAL_GetTick() > current_tone_end) && (current_tone_number < melody_tone_count)) {
        // Get the current active tone from the melody
        const Tone active_tone = *(melody_ptr + current_tone_number);
        // Change the PWM frequency and volume to the active tone
        PWM_Cange_Tone(active_tone.frequency, volume);
        // Set the end time for the current tone
        current_tone_end = HAL_GetTick() + active_tone.duration;
        // Move to the next tone in the melody
        current_tone_number++;
    } else if (current_tone_number == melody_tone_count) {
        // If all tones in the melody have been played and not in the game state
        if (current_state != STATE_GAME) {
            // Toggle the state of a GPIO pin (possibly to indicate the end of the melody)
            HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_12);
            // Update the music to start playing the default melody (super mario bros)
            Update_music(1);
        }
    }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  	 mytime.Hours = 0;
     mytime.Minutes = 0;
     mytime.Seconds = 0;

   HAL_RTC_SetTime(&hrtc, &mytime, RTC_FORMAT_BIN);

  LiquidCrystal(GPIOD, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14);


  createChar(0, snakeBodyright);
  createChar(1, snakeBody);
  createChar(2, apple);
  createChar(3, luckBox);
  createChar(4, obstacle);
  createChar(5, heart);
  createChar(6, snakeBodyup);
  createChar(7, snakeBodydown);
  createChar(8, snakeBodyleft);

  HAL_UART_Receive_IT(&huart2, &d, 1);


  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, 1);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, 1);

  HAL_TIM_Base_Start_IT(&htim6);
  HAL_TIM_Base_Start_IT(&htim7);

  Display_Initial_Page();

  PWM_Start();
  Update_music(1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, 0);

  HAL_ADC_Start_IT(&hadc1);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_RTC
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12PLLCLK_DIV1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.SamplingTime = ADC_SAMPLETIME_181CYCLES_5;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 39;
  hrtc.Init.SynchPrediv = 999;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 480-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 100-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 480-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin
                          |LD6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : DRDY_Pin */
  GPIO_InitStruct.Pin = DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DRDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CS_I2C_SPI_Pin LD4_Pin LD3_Pin LD5_Pin
                           LD7_Pin LD9_Pin LD10_Pin LD8_Pin
                           LD6_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|LD4_Pin|LD3_Pin|LD5_Pin
                          |LD7_Pin|LD9_Pin|LD10_Pin|LD8_Pin
                          |LD6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC4 PC6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC1 PC3 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA4 PA8 PA9 PA10
                           PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MISOA7_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MISOA7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DM_Pin DP_Pin */
  GPIO_InitStruct.Pin = DM_Pin|DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF14_USB;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD1 PD2 PD3 PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB5 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : I2C1_SCL_Pin I2C1_SDA_Pin */
  GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
const char *keypad_map[] = {
    "1", "2ABC", "3DEF",
    "4GHI", "5JKL", "6MNO",
    "7PQRS", "8TUV", "9WXYZ",
    "*", "0 ", "#"
};

volatile uint8_t current_key = 0xFF;  // Store the current key pressed
volatile uint8_t current_index = 0;   // Index for character cycling on multi-press keys
volatile uint32_t last_keypress_time = 0; // Time of the last keypress
char settings_buffer[32];  // Buffer for storing settings input
uint8_t buffer_index = 0;  // Index for the settings buffer

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (last_gpio_exti + 200 > HAL_GetTick()) 
    {
        return; // Debounce check
    }
    last_gpio_exti = HAL_GetTick();

    int8_t row_number = -1;
    int8_t column_number = -1;

    if (GPIO_Pin == GPIO_PIN_0)
    {
        // Handle special case for GPIO_PIN_0 if needed
    }

    for (uint8_t row = 0; row < 4; row++) 
    {
        if (GPIO_Pin == Row_pins[row])
        {
            row_number = row; // Determine row number
        }
    }

    // Drive columns low
    HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 0);
    HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 0);
    HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 0);
    HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 0);

    for (uint8_t col = 0; col < 4; col++) 
    {
        HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 1); // Drive one column high at a time
        if (HAL_GPIO_ReadPin(Row_ports[row_number], Row_pins[row_number]))
        {
            column_number = col; // Determine column number
        }
        HAL_GPIO_WritePin(Column_ports[col], Column_pins[col], 0); // Reset column
    }

    // Drive columns back high
    HAL_GPIO_WritePin(Column_ports[0], Column_pins[0], 1);
    HAL_GPIO_WritePin(Column_ports[1], Column_pins[1], 1);
    HAL_GPIO_WritePin(Column_ports[2], Column_pins[2], 1);
    HAL_GPIO_WritePin(Column_ports[3], Column_pins[3], 1);

    if (row_number == -1 || column_number == -1)
    {
        return; // No valid keypress detected
    }

    uint8_t button_number = row_number * 4 + column_number;
    if (current_state == STATE_SETTINGS  && current_setting == 4) {
        if (button_number == 8) {
            button_number = 10; // Special handling for button 8
        }
    }

    switch (button_number + 1)
    {
        case 1:
            if (current_state == STATE_GAME)
                Update_music(1); // Update music in game state
            if (current_state == STATE_INITIAL_PAGE) {
                current_state = STATE_GAME_MENU; // Transition to game menu
            } else if (current_state == STATE_ABOUT || current_state == STATE_SETTINGS ||
                       current_state == STATE_SELECT_MODE || current_state == STATE_GAME || current_state == STATE_GAME_OVER) {
                current_state = STATE_GAME_MENU; // Transition to game menu from other states
            }
            break;
        case 2:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_13); // Toggle GPIO pin for feedback
                    if (buffer_index > 0)
                        buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') { 
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }
            break;
        case 3:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                         buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }
            break;
        case 4:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                        buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }
            break;
        case 5:
            if (current_state == STATE_SETTINGS && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                          buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            } else if (current_state == STATE_GAME) {
                if (!dx) {
                    dx = -1; // Update direction in game state
                    dy = 0;
                }
            } else if (current_state == STATE_SETTINGS) {
                Adjust_Setting(-1); // Adjust setting value
            }
            break;
        case 6:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                         buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            } else if (current_state == STATE_SETTINGS) {
                Adjust_Setting(1); // Adjust setting value
            }
            break;
        case 7:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                         buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }
            break;
        case 8:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                         buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }
            break;
        case 9:
            if (current_state == STATE_GAME_MENU) {
                Navigate_Menu(1); // Navigate down in game menu
            } else if (current_state == STATE_SELECT_MODE) {
                if (MODE_COUNT - 1 == current_mode)
                    current_mode = 0; // Wrap around mode selection
                else
                    current_mode++;
            } else if (current_state == STATE_GAME) {
                if (!dy) {
                    dx = 0; // Update direction in game state
                    dy = 1;
                }
            } else if (current_state == STATE_SETTINGS) {
                if (current_setting < SETTINGS_COUNT - 1) {
                    current_setting++; // Increment setting value

                    if (current_setting >= display_start + DISPLAY_ROWS)
                        display_start++; // Adjust display start if needed
                }
            }
            break;
        case 10:
            if (current_state == STATE_GAME_MENU) {
                Navigate_Menu(-1); // Navigate up in game menu
            } else if (current_state == STATE_SETTINGS) {
                if (current_setting > 0) {
                    current_setting--; // Decrement setting value
                    if (current_setting < display_start) {
                        display_start--; // Adjust display start if needed
                    }
                }
            } else if (current_state == STATE_SELECT_MODE) {
                if (0 == current_mode)
                    current_mode = 2; // Wrap around mode selection
                else
                    current_mode--;
            } else if (current_state == STATE_GAME) {
                if (!dy) {
                    dx = 0; // Update direction in game state
                    dy = -1;
                }
            }
            break;
        case 11:
            if (current_state == STATE_SETTINGS  && current_setting == 4) {
                if (button_number == current_key && HAL_GetTick() - last_keypress_time < 1000) {
                    current_index = (current_index + 1) % strlen(keypad_map[button_number]); // Cycle through characters
                    if (buffer_index > 0)
                         buffer_index--; // Adjust buffer index if necessary
                } else {
                    current_key = button_number;
                    current_index = 0; // Reset index for new key
                }

                last_keypress_time = HAL_GetTick(); // Update last keypress time

                char output_char = keypad_map[button_number][current_index];
                if (output_char != '*') {
                    settings_buffer[buffer_index++] = output_char; // Add character to buffer
                    settings_buffer[buffer_index] = '\0';
                } else if (buffer_index > 0) {
                    buffer_index--; // Remove character from buffer
                    settings_buffer[buffer_index] = '\0';
                }
                strcpy(settings.player_name, settings_buffer); // Copy buffer to player name
            }           
            break;
        case 12:
            // Handle case 12 if needed
            break;
        case 13:
            if (current_state == STATE_GAME_MENU) {
                Select_Menu_Option(); // Select menu option
            } else if (current_state == STATE_GAME) {
                if (!dx) {
                    dx = 1; // Update direction in game state
                    dy = 0;
                }
            }
            break;
        case 14:
            // Handle case 14 if needed
            break;
        case 15:
            // Handle case 15 if needed
            break;
        case 16:
            // Handle case 16 if needed
            break;
        default:
            break;
    }
}

void display_settings_buffer(const char *buffer)
{
    printf("%s", buffer); // Display the settings buffer
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM6) {
        HAL_RTC_GetTime(&hrtc, &mytime, RTC_FORMAT_BIN); // Get current RTC time
    
        if (current_state == STATE_GAME) {
            Manage_Heart(); // Manage game elements
            Manage_Apple();
            Manage_LuckBox();
            Manage_Obstacles();
            Move_Snake();
            LCD_Update();
            if (!health) {
                game_over_flag = 1;
                current_state = STATE_GAME_OVER; // Transition to game over state
            }
        } else if (current_state == STATE_GAME_MENU) {
            Display_Game_Menu(); // Display game menu
        } else if (current_state == STATE_ABOUT) {
            Display_About_Page(); // Display about page

            sprintf(timestr, "Time: %2d:%2d:%2d", mytime.Hours, mytime.Minutes, mytime.Seconds);
            setCursor(0, 3);
            print(timestr); // Display current time
        } else if (current_state == STATE_SETTINGS) {
            Update_Settings_Display(); // Update settings display
        } else if (current_state == STATE_SELECT_MODE) {
            Update_Mode_Display(); // Update mode display
        } else if (current_state == STATE_GAME_OVER) {
            Display_Game_Over(); // Display game over screen
        }
    }

    if (htim->Instance == TIM7) {
        refresh_7segments(); // Refresh 7-segment display
        Update_melody(); // Update melody

        if (current_state == STATE_GAME) {
            if (HAL_GetTick() - time_minute >= 60000) {
                if (minute < 99)
                    minute++; // Increment minute counter

                time_minute = HAL_GetTick(); // Reset minute timer
            }
        }
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART2) {
        HAL_UART_Receive_IT(&huart2, &d, 1); // Receive UART data

        data[letter++] = d; // Store received data
        data[letter] = '\0'; // Null-terminate string

        if (d == '\n' || d == '\r') {
            data[letter - 1] = '\0'; // Replace newline with null terminator
            
            HAL_UART_Transmit(&huart2, data, strlen(data), HAL_MAX_DELAY); // Echo received data
            if (current_state == STATE_SETTINGS) {
                settings.player_name[0] = '\0'; // Clear player name
                strcpy(settings.player_name, data); // Copy received data to player name
                HAL_UART_Transmit(&huart2, settings.player_name, strlen(settings.player_name), HAL_MAX_DELAY); // Echo player name
            }

            letter = 0; data[0] = '\0'; // Reset letter index and data buffer
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {
        volume = ((HAL_ADC_GetValue(&hadc1) * 120) / 4000) - 10; // Calculate volume

        if (volume < 0)
            volume = 0; // Clamp volume to minimum
        if (volume > 100)
            volume = 100; // Clamp volume to maximum
        HAL_ADC_Start_IT(&hadc1); // Restart ADC conversion
    }
} 


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
