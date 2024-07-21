# Snake Game for STM32

![snake-game](https://github.com/user-attachments/assets/856d42c5-7430-4cf8-a44a-a767accc156b)

## Overview

This project is a comprehensive implementation of the classic Snake game designed for the STM32 microcontroller. The game features an engaging user interface and interactive gameplay, bringing nostalgia to the modern era. Players control a snake that grows in length by consuming food while avoiding collisions with walls and its own body.

## Features

- **Main Menu**: Upon starting the game, a welcome screen displays the game title along with a selectable soundtrack. Players can navigate through a menu with four options:
  1. **Start Game**
  2. **Settings**
  3. **Game Modes**
  4. **About**

- **About Section**: Displays the names of team members along with the real-time date and time, updating every second using an RTC module.

- **Settings**: Players can configure initial health, snake speed, sound effects, and obstacle count (up to 6). Additionally, players can input their names.

- **Game Modes**:
  - **Mode 1**: Snake can re-enter the game area from the opposite wall but loses health only upon self-collision.
  - **Mode 2**: Snake loses health when hitting walls but can continue if health is not zero; a 90-degree right turn occurs.
  - **Mode 3**: Increasing game time results in faster snake movement, with wall collision behavior similar to Mode 1.

- **Gameplay**:
  - Players control the snake using four directional buttons, making 90-degree turns.
  - The snake is visually represented with custom characters (head, body, and tail).
  - A four-digit seven-segment display shows elapsed minutes and player score.

- **Game Elements**:
  - **Apples**: Increase snake length by one unit upon consumption.
  - **Health Tokens**: Increase snake health by one unit.
  - **Obstacles**: Reduce health when collided; initially, four random obstacles are placed on the map.
  - **Chance Box**: Appears every minute for 5 seconds, randomly providing benefits such as score increase, health increase, or speed boost.

- **End of Game**: When the snake's health reaches zero, the game ends with an animation, displaying the player's name and score.

- **Sound Management**: Sound volume can be adjusted in increments of 5%.

- **Game History**: The game maintains a log that includes player name, score, and timestamps for start and end times, stored in a file format compatible with your PC.

## Requirements

![Screenshot 2024-07-21 202856](https://github.com/user-attachments/assets/f2b2e3e9-267c-4915-a8f1-7021b283585f)


- STM32 Microcontroller
- LCD for display
- RTC module for real-time clock functionality
- Keypad for user input
- Buzzer for sound effects

## Installation

1. Clone this repository to your local machine.
2. Open the project in your preferred IDE for STM32 (e.g., STM32CubeIDE).
3. Connect your STM32 board and upload the code.
4. Follow the in-game instructions to navigate through the menu and enjoy the game!

## Contributing

Feel free to contribute to the project by submitting issues, improving the code, or adding new features. Your contributions are welcome!






