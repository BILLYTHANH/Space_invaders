#include "vt100.h"
#include "serial.h"
#include "start.h"

/*	Dimensions fenêtre de jeu	*/
void window(uint8_t character) {
	uint8_t i;
	for (i = 1; i < 80; i++) {
		vt100_move(i, 1);
		serial_putchar(character);
		vt100_move(i, 24);
		serial_putchar(character);
	}
	for (i = 1; i < 24; i++) {
		vt100_move(1, i);
		serial_putchar(character);
		vt100_move(80, i);
		serial_putchar(character);
	}
}

/*	Définition page d'acceuil	*/
void home_page(void) {
	vt100_move(35, 8);
	serial_puts("SPACE INVADERS");
	vt100_move(30, 15);
	serial_puts("PRESS [SPACE] TO PLAY");
	vt100_move(30, 16);
	serial_puts("PRESS [SPACE] TO FIRE");
	vt100_move(30, 17);
	serial_puts("PRESS [Q] TO MOVE LEFT");
	vt100_move(30, 18);
	serial_puts("PRESS [D] TO MOVE RIGHT");
	vt100_move(2, 2);
	serial_puts("CREDITS < >");
	vt100_move(33, 2);
	serial_puts("HIGH SCORE < >");
	vt100_move(70, 2);
	serial_puts("SCORE < >");
	vt100_move(2, 12);
	serial_puts("------      ------      ------      ------      ------      ------      ------");
}

/*	Lancement du jeu	*/
void game_launch(void) {
	while (serial_get_last_char() != ' ') {
	}
	vt100_clear_screen();
	window('#');

}
