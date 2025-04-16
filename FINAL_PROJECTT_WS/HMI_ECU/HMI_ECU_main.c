/*
 * HMI_ECU_main.c
 *
 * Created on: Nov 5, 2024
 * Author: hassa
 *
 * Description:
 * This is the main file for the HMI (Human-Machine Interface) ECU. It manages
 * password input, authentication, and door control interactions with the control ECU
 * over UART. The system includes functionalities for setting a new password, logging in,
 * opening the door, and changing the password, with protection against unauthorized access.
 */

#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"
#include "std_types.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/*******************************************************************************
 *                          Global Variables                                   *
 *******************************************************************************/
uint8 match = 0;         // Flag to store password setup confirmation from control ECU
uint8 match2 = 0;        // Flag to store login status from control ECU
uint8 match3 = 0;        // Flag for additional control messages
uint8 g_count = 0;       // Counter for time-based operations
uint8 try_count = 0;     // Counter for tracking failed login attempts
uint8 key;

UART_ConfigType config = {eight, EVEN, ONE_BIT, 9600};
Timer_ConfigType configurate = {0, 31250, TIMER1_ID, TIMER0_1_PRESCALER_256, CTC_MODE};

/*******************************************************************************
 *                          Function Definitions                               *
 *******************************************************************************/

/*
 * Description:
 * Timer callback function increments g_count to track elapsed time.
 */
void timer_CallBack() {
	g_count++;
}
/*
 * Description:
 * Flushes the UART receive buffer to prevent any leftover bytes from interfering with operations.
 */
void UART_flushBuffer(void)
{
	while (UCSRA & (1 << RXC)) {
		(void)UDR;  // Read and discard the byte in the receive buffer
	}
}

/*
 * Description:
 * Sends a 5-digit password over UART to the control ECU.
 */
void send_password_to_control_ECU(uint8* password) {
	for (uint8 i = 0; i < 5; i++) {
		UART_sendByte(password[i]);
		_delay_ms(10);
	}
}

/*
 * Description:
 * Clears the provided password array by setting each element to 0.
 */
void clear_password_input(uint8* password) {
	for (uint8 i = 0; i < 5; i++) {
		password[i] = 0;
	}
}

/*
 * Description:
 * Handles user input from the keypad, storing a 5-digit password
 * and displaying '*' for each digit entered on the LCD.
 */
void handle_password_input(uint8* password) {
	uint8 key = 0;
	uint8 pass_counter = 0;

	// Loop until a 5-digit password is entered
	while (pass_counter < 5) {
		key = KEYPAD_getPressedKey();
		_delay_ms(10);
		while (!(key >= 0 && key <= 16)) {
			key = KEYPAD_getPressedKey();
			_delay_ms(10);
		}
		_delay_ms(300);
		password[pass_counter] = key;
		LCD_displayCharacter('*');
		pass_counter++;
	}

	// Wait until the user confirms input with '='
	while (key != '=') {
		key = KEYPAD_getPressedKey();
	}
}

/*
 * Description:
 * Initiates the process of setting a new password and confirms with the control ECU.
 */
void new_password(void) {
	uint8 password[5] = {0};
	uint8 re_entered[5] = {0};
	match = 0;

	// Prompt for the initial password input
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz Enter Pass:");
	LCD_moveCursor(1, 0);
	handle_password_input(password);
	send_password_to_control_ECU(password);

	// Prompt to re-enter the password for confirmation
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Plz re-Enter the");
	LCD_displayStringRowColumn(1, 0, "same pass:");
	LCD_moveCursor(1, 10);
	handle_password_input(re_entered);
	send_password_to_control_ECU(re_entered);

	// Receive confirmation from control ECU for password match
	while (!match) {
		match = UART_recieveByte();
		if (match2 == 0) break;
	}
	clear_password_input(password);
}

/*
 * Description:
 * Initiates the login process by requesting password entry and confirming with control ECU.
 */
void login_password(void) {
	uint8 password[5] = {0};
	match2 = 0;

	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "Please Enter old");
	LCD_displayStringRowColumn(1, 0, "Pass:");
	LCD_moveCursor(1, 5);

	handle_password_input(password);
	send_password_to_control_ECU(password);

	// Clear any remaining data in the UART buffer
	UART_flushBuffer();

	// Wait for the response from control ECU
	while (!match2) {
		match2 = UART_recieveByte();
		if (match2 == 0) break;  // Exit if the password is incorrect
	}
	clear_password_input(password);
}

/*
 * Description:
 * Displays the home page options on the LCD for opening the door or changing the password.
 */
void Home_page_display(void) {
	LCD_clearScreen();
	LCD_displayStringRowColumn(0, 0, "+ : OPEN DOOR");
	LCD_displayStringRowColumn(1, 0, "- : CHANGE PASS");
}

/*
 * Description:
 * Resets the flag variables and counters to initial states.
 */
void reset_flags() {
	match = 0;
	match2 = 0;
	match3 = 0;
	try_count = 0;
	g_count = 0;
}

/*******************************************************************************
 *                          Main Function                                      *
 *******************************************************************************/
int main(void)
{
	reset_flags();
	uint8 pir_receive = 0;  // Variable to store PIR sensor status from control ECU
	SREG |= (1 << 7);       // Enable global interrupts
	UART_init(&config);     // Initialize UART with configured parameters
	LCD_init();             // Initialize LCD
	Timer_init(&configurate);   // Initialize timer with configured parameters
	Timer_setCallBack(timer_CallBack, TIMER1_ID);   // Set timer callback function

	/* Password Setup Phase */
	while (match != 1) {
		// Wait for confirmation of successful password setup
		new_password();
		_delay_ms(100);
	}

	/* Main Control Loop */
	while (1) {
		LCD_clearScreen();
		Home_page_display();
		key = 0;
		reset_flags();

		while (key != '+' && key != '-') {
			key = KEYPAD_getPressedKey();
		}

		/* Main Option Selection */
		switch (key) {
		case '+':  // Door Open Sequence
			UART_sendByte('+');
			match2 = 0;
			key = 0;

			while ((match2 == 0) && (try_count < 3))
			{
				login_password();
				if (match2 == 0) {
					match2 = 1;
					match3 = UART_recieveByte();
					if (match3 == 0) {
						try_count++;
						match2=0;
					} else if (match3 == 'L') {
						match2 = 1;
						match3 = 0;
					}
					if (try_count == 3)
					{
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 0, "System LOCKED");
						LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
						g_count = 0;
						while (g_count < 60) {};
						try_count = 0;
						reset_flags();
						break;
					}
				}
			}

			g_count = 0;
			LCD_clearScreen();
			LCD_displayStringRowColumn(0, 0, "Door Unlocking");
			LCD_displayStringRowColumn(1, 0, "Please wait   ");
			while (g_count != 15) {};

			pir_receive = UART_recieveByte();
			if (pir_receive) {
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0, "Wait for people");
				LCD_displayStringRowColumn(1, 0, "to enter");
				while (pir_receive) {
					pir_receive = UART_recieveByte();
				};
			}

			if (pir_receive == 0) {
				g_count = 0;
				LCD_clearScreen();
				LCD_displayStringRowColumn(0, 0, "Door Locking");
				LCD_displayStringRowColumn(1, 0, "Please wait   ");
				while (g_count != 15) {};
			}

			try_count = 0;
			match2 = 0;
			break;

		case '-':  // Change Password Sequence
			UART_sendByte('-');
			reset_flags();

			while ((match2 == 0) && (try_count < 3))
			{
				login_password();
				if (match2 == 0) {
					match2 = 1;
					match3 = UART_recieveByte();
					if (match3 == 0) {
						try_count++;
						match2=0;
					} else if (match3 == 'L') {
						match2 = 1;
						match3 = 0;
					}
					if (try_count == 3)
					{
						LCD_clearScreen();
						LCD_displayStringRowColumn(0, 0, "System LOCKED");
						LCD_displayStringRowColumn(1, 0, "Wait for 1 min");
						g_count = 0;
						while (g_count < 60) {};
						try_count = 0;
						reset_flags();
						break;
					}
				}
			}

			g_count = 0;
			new_password();
			break;
		}
	}
}
