/*
 * control_ECU_main.c
 * Created on: Nov 5, 2024
 * Author: hassa
 */

#include "buzzer.h"
#include "pwm.h"
#include "pir.h"
#include "timer.h"
#include "twi.h"
#include "dcmotor.h"
#include "uart.h"
#include "std_types.h"
#include "external_eeprom.h"
#include <util/delay.h>

/* EEPROM memory addresses for password storage */
#define PASSWORD_ADDRESS_1 0x10
#define PASSWORD_ADDRESS_2 0x15
#define PASSWORD_ADDRESS_3 0x1A
#define PASSWORD_ADDRESS_4 0x1F

/* Signals for login status */
#define SUCCESS_SIGNAL 0xA5
#define FAILURE_SIGNAL 0xA6

/* Password settings */
#define PASSWORD_LENGTH 5
#define MAX_ATTEMPTS 3

/* Door operation durations in seconds */
#define DOOR_OPERATION_DURATION 15
#define LOCKOUT_DURATION 60

/* Global variables for system state management */
uint8 login_success = 0;
uint8 g_count = 0;
uint8 current_pir_state = 0xFF;
uint8 setup_complete = 0;
uint8 try = 0;
uint8 renew_success = 0;

/* UART, Timer, and TWI configuration structures */
UART_ConfigType uart_config = {eight, EVEN, ONE_BIT, 9600};
Timer_ConfigType timer_config = {0, 31250, TIMER1_ID, TIMER0_1_PRESCALER_256, CTC_MODE};
TWI_ConfigType TWI_config = {TWI_BAUDRATE_400K, ADDRESS_1};

/*
 * Callback function for timer interrupt to increment global count
 */
void timer_callback(void) {
	g_count++;
}

/*
 * Receives password via UART and stores it in EEPROM at specified address
 */
void receive_and_store_password(uint16 address, uint8* password) {
	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		password[i] = UART_recieveByte();
		EEPROM_writeByte(address + i, password[i]);
		_delay_ms(10);
	}
}

/*
 * Compares two passwords stored at given EEPROM addresses
 * Returns 1 if passwords match, 0 otherwise
 */
uint8 compare_passwords(uint16 address1, uint16 address2) {
	uint8 password1[PASSWORD_LENGTH];
	uint8 password2[PASSWORD_LENGTH];

	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		EEPROM_readByte(address1 + i, &password1[i]);
		EEPROM_readByte(address2 + i, &password2[i]);
		_delay_ms(10);
	}

	for (uint8 i = 0; i < PASSWORD_LENGTH; i++) {
		if (password1[i] != password2[i]) return 0;
	}
	return 1;
}

/*
 * Opens the door by rotating the motor in the clockwise direction
 */
void door_open(void) {
	g_count = 0;
	Timer_init(&timer_config);
	Timer_setCallBack(timer_callback, TIMER1_ID);
	DcMotor_Rotate(CW, 100);

	while (g_count < DOOR_OPERATION_DURATION) {}

	DcMotor_Rotate(STOP, 0);
	Timer_deInit(TIMER1_ID);
}

/*
 * Closes the door by rotating the motor in the anti-clockwise direction
 */
void door_close(void) {
	g_count = 0;
	Timer_init(&timer_config);
	Timer_setCallBack(timer_callback, TIMER1_ID);
	DcMotor_Rotate(A_CW, 100);

	while (g_count < DOOR_OPERATION_DURATION) {}

	DcMotor_Rotate(STOP, 0);
	Timer_deInit(TIMER1_ID);
}

/*
 * Activates the buzzer continuously until a signal to stop is received
 */
void continuous_buzzer_alert(void) {
	uint8 buzzer_status = UART_recieveByte();
	while (buzzer_status == 1) {
		Buzzer_on();
		buzzer_status = UART_recieveByte();
	}
	Buzzer_off();
}

/*
 * Sets up the initial password, requiring the user to enter and re-enter it for confirmation
 */
void setup_password(void) {
	uint8 password[PASSWORD_LENGTH] = {0};
	uint8 re_entered_password[PASSWORD_LENGTH] = {0};

	while (!setup_complete) {
		receive_and_store_password(PASSWORD_ADDRESS_1, password);
		_delay_ms(500);
		receive_and_store_password(PASSWORD_ADDRESS_2, re_entered_password);
		_delay_ms(500);

		if (compare_passwords(PASSWORD_ADDRESS_1, PASSWORD_ADDRESS_2)) {
			UART_sendByte(1);
			setup_complete = 1;
		} else {
			UART_sendByte(0);
		}
	}
}

/*
 * Manages the login process by receiving the login password and comparing it with the stored password
 */
void login_password(void) {
	uint8 login_password[PASSWORD_LENGTH] = {0};

	receive_and_store_password(PASSWORD_ADDRESS_3, login_password);

	if (compare_passwords(PASSWORD_ADDRESS_3, PASSWORD_ADDRESS_1)) {
		_delay_ms(50);
		UART_sendByte(1);
		login_success = 1;
	}
	else
	{
		UART_sendByte(0);
		try++;
	}
	if (try == MAX_ATTEMPTS)
	{
		try = 0;
		g_count = 0;
		UART_sendByte(0);
		Timer_init(&timer_config);
		Timer_setCallBack(timer_callback, TIMER1_ID);
		Buzzer_on();
		while (g_count < LOCKOUT_DURATION) {}
		Buzzer_off();
	}
}

/*
 * Renews the password by requiring the user to enter the current password for verification
 */
void renew_password(void) {
	uint8 renew_password[PASSWORD_LENGTH] = {0};
	try = 0;
	renew_success = 0;


	receive_and_store_password(PASSWORD_ADDRESS_4, renew_password);

	if (compare_passwords(PASSWORD_ADDRESS_4, PASSWORD_ADDRESS_1)) {
		UART_sendByte(1);
		renew_success = 1;
	} else {
		UART_sendByte(0);
		try++;
	}
	if (try == MAX_ATTEMPTS) {
		g_count = 0;
		UART_sendByte(0);
		Timer_init(&timer_config);
		Timer_setCallBack(timer_callback, TIMER1_ID);
		Buzzer_on();
		while (g_count < LOCKOUT_DURATION) {}
		Buzzer_off();
		try = 0;
	}

}

/*
 * Handles PIR sensor and controls the door's open and close operations based on PIR state
 */
void handle_pir_and_door(void) {
	current_pir_state = 0xFF;
	uint8 pir_state;

	while (1) {
		pir_state = PIR_getState();

		if (pir_state != current_pir_state) {
			UART_sendByte(pir_state);
			current_pir_state = pir_state;
		}

		if (pir_state == 0) {
			UART_sendByte(0);
			door_close();
			break;
		}
	}
}

/*
 * Main function to initialize peripherals, setup password, and manage user inputs for login and password renewal
 */
int main(void) {
	SREG |= (1 << 7); /* Enable global interrupts */
	TWI_init(&TWI_config);
	UART_init(&uart_config);
	Buzzer_init();
	DcMotor_Init();
	PIR_init();
	Timer_init(&timer_config);
	Timer_setCallBack(timer_callback, TIMER1_ID);

	while (setup_complete != 1) {
		setup_password();
	}
	while (1) {
		uint8 recieverByte = 0;
		setup_complete = 0;
		while ((recieverByte != '+') && (recieverByte != '-')) {
			recieverByte = UART_recieveByte();
		};
		switch (recieverByte) {
		case '+':
			g_count = 0;
			try = 0;
			renew_success = 0;
			login_success = 0;
			while (!login_success) {
				login_password();
				if(login_success) {
					UART_sendByte('L');
				} else {
					UART_sendByte(0);
				}
			}
			door_open();
			handle_pir_and_door();
			recieverByte = 0;
			login_success=0;
			break;

		case '-':
			login_success = 0;
			g_count = 0;
			setup_complete = 0;
			renew_success = 0;
			try = 0;
			while (!renew_success)
			{
				renew_password();
				if(renew_success) {
					UART_sendByte('L');
				} else {
					UART_sendByte(0);
				}
			}
			renew_success = 0;
			recieverByte = 0;
			setup_complete = 0;
			break;
		}
	}
}
