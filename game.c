#include "vt100.h"
#include "serial.h"
#include "start.h"
#include "game.h"

/*	STRUCURE CONTENANT POSITIONS ENNEMIES/ ENVIRONNEMENT	*/
typedef struct {
	uint8_t l1[15][3];
	uint8_t l2[15][3];
	uint8_t l3[15][3];
} s_enemy_position;

/*	STRUCURE CONTENANT COORDONNÉES JOUEUR	*/
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t LIFE;
} s_player_position;

s_player_position player;
s_enemy_position enemy;

/*	DÉCLARATION VARIABLES STATIQUES	*/
static uint8_t counter = 0;
static uint8_t a = 0x50;
static uint8_t x_enemy_fire;
static uint8_t y_enemy_fire = 0;
static uint8_t win = 0;
static uint8_t score_hundred = 48;
static uint8_t score_thousand = 48;

s_player_position *player_pointer = &player;
s_enemy_position *enemy_pointer = &enemy;

/*	INITIALISATION ENNEMIES	*/
void init_enemy(void) {
	uint8_t column = 0;
	uint8_t x;
	win = 0;
	for (x = 5; x < 76; x = x + 5) {
		enemy_pointer->l1[column][2] = 1;
		enemy_pointer->l2[column][2] = 1;
		enemy_pointer->l3[column][2] = 1;
		enemy_display(column, 1, x, 3);
		enemy_display(column, 2, x, 6);
		enemy_display(column, 3, x, 15);
		column++;
	}
}

/*	INITIALISATION JOUEUR	*/
void init_player(void) {
	player_pointer->x = 39;
	player_pointer->y = 21;
	player_pointer->LIFE = 3;
	vt100_move(player_pointer->x, player_pointer->y);
	serial_puts("(0)");
	score_display();
	life_Player_display();
}

/*	AFFICHAGE NOMBRE DE VIES DU JOUEUR	*/
void life_Player_display(void) {
	if (player_pointer->LIFE == 3) {
		vt100_move(2, 2);
		serial_puts("LIFE : 3");
	} else if (player_pointer->LIFE == 2) {
		vt100_move(2, 2);
		serial_puts("LIFE : 2");
	} else if (player_pointer->LIFE == 1) {
		vt100_move(2, 2);
		serial_puts("Life : 1");
	} else if (player_pointer->LIFE == 0) {
		vt100_clear_screen();
		vt100_move(35, 10);
		serial_puts("GAME OVER");
		score_hundred = 48;
		score_thousand = 48;
		restart();
	}
}

/*	AFFICHAGE SCORE JOUEUR	*/
void score_display(void) {
	vt100_move(78, 2);
	serial_puts("00");
	vt100_move(77, 2);
	serial_putchar(score_hundred);
	vt100_move(76, 2);
	serial_putchar(score_thousand);
	vt100_move(68, 2);
	serial_puts("SCORE :");
}

/*	AFFICHAGE ENNEMIES	*/
void enemy_display(uint8_t column, uint8_t line, uint8_t x, uint8_t y) {
	if (enemy_destroyed_check(column, line) == 1) {
		return;
	}
	if (line == 1) {
		enemy_pointer->l1[column][0] = x;
		enemy_pointer->l1[column][1] = y;
		vt100_move(enemy_pointer->l1[column][0], enemy_pointer->l1[column][1]);
		serial_puts(">X<");
	} else if (line == 2) {
		enemy_pointer->l2[column][0] = x;
		enemy_pointer->l2[column][1] = y;
		vt100_move(enemy_pointer->l2[column][0], enemy_pointer->l2[column][1]);
		serial_puts(">V<");
	}else if (line == 3) {
		enemy_pointer->l3[column][0] = x;
		enemy_pointer->l3[column][1] = y;
		vt100_move(enemy_pointer->l3[column][0], enemy_pointer->l3[column][1]);
		serial_puts("___");
	}
}

/*	DESTRUCTION ENNEMIES	*/
void enemy_destroyed(uint8_t column, uint8_t line) {
	if (enemy_destroyed_check(column, line) == 1) {
		return;
	}
	if (line == 1) {
		vt100_move(enemy_pointer->l1[column][0], enemy_pointer->l1[column][1]);
		serial_puts("   ");
	} else if (line == 2) {
		vt100_move(enemy_pointer->l2[column][0], enemy_pointer->l2[column][1]);
		serial_puts("   ");
	}
}

uint8_t PRNG_LSFR(void) {
	uint8_t bit0, bit3, bit5, xor;

	bit0 = (a >> 0) & 1;
	bit3 = (a >> 3) & 1;
	bit5 = (a >> 5) & 1;
	xor = bit0 ^ bit3 ^ bit5;
	a = a >> 1;
	a |= xor << 7;

	return a % 15;
}

/*	INITIALISATION TIRS ENNEMIES	*/
void enemy_fire(void) {
	if (y_enemy_fire == 0) {
		uint8_t column = PRNG_LSFR();
		if (enemy_destroyed_check(column, 2) == 0) {
			x_enemy_fire = enemy_pointer->l2[column][0] + 1;
			y_enemy_fire = enemy_pointer->l2[column][1] + 1;
		} else if (enemy_destroyed_check(column, 1) == 0) {
			x_enemy_fire = enemy_pointer->l1[column][0] + 1;
			y_enemy_fire = enemy_pointer->l1[column][1] + 1;
		} else {
			return;
		}
		vt100_move(x_enemy_fire, y_enemy_fire);
		serial_putchar('|');
	}
	vt100_move(x_enemy_fire, y_enemy_fire);
	serial_putchar(' ');
	y_enemy_fire++;
	vt100_move(x_enemy_fire, y_enemy_fire);
	serial_putchar('|');
	if (y_enemy_fire == player_pointer->y) {
		if (hitbox(x_enemy_fire, y_enemy_fire) == 1) {
			y_enemy_fire = 0;
		} else {
			vt100_move(x_enemy_fire, y_enemy_fire);
			serial_putchar(' ');
			y_enemy_fire = 0;
		}
	}
}

/*	INITIALISATION TIRS JOUEUR	*/
void player_fire(void) {
	uint8_t x_player_fire = player_pointer->x + 1;
	uint8_t y_player_fire = player_pointer->y;
	uint32_t k = 0;
	uint32_t i = 0;
	uint8_t j = 0;
	uint32_t o = 0;
	signed char var;
	vt100_move(x_player_fire, y_player_fire - 1);
	serial_putchar('*');
	vt100_move(x_player_fire, y_player_fire - 1);
	serial_putchar('|');
	for (y_player_fire = 19; y_player_fire > 1; y_player_fire--) {
		for (i = 0; i < 0xF001; i++) {
			if (k == 0xFFF) {
				if (y_player_fire == 1) {
					vt100_move(x_player_fire, y_player_fire + 1);
					serial_putchar(' ');
					return;
				}
				k = 0;
				vt100_move(x_player_fire, y_player_fire + 1);
				serial_putchar(' ');
				for (j = 0; j < 0x2F; j++) {
					vt100_move(x_player_fire, y_player_fire);
					serial_putchar('*');
					vt100_move(x_player_fire, y_player_fire);
					serial_putchar('|');
				}
				if (y_enemy_fire == y_player_fire) {
					if (x_enemy_fire == x_player_fire) {
						vt100_move(x_player_fire, y_player_fire);
						serial_putchar('X');
						y_enemy_fire = 0;
						return;
					}
				}
				y_player_fire--;
			}
			if (o == 0x7800) {
				o = 0;
				enemy_fire();
				enemy_movement();
			}
			var = serial_get_last_char();
			player_movement(var);
			k++;
			o++;
			if (hitbox(x_player_fire, y_player_fire) == 1) {
				vt100_move(x_player_fire, y_player_fire + 1);
				serial_putchar(' ');
				return;
			}
		}
		vt100_move(x_player_fire, y_player_fire + 1);
		serial_putchar(' ');
	}
}

uint8_t hitbox(uint8_t x, uint8_t y) {
	uint8_t i;
	uint8_t j;
	if (y == enemy_pointer->l1[0][1]) {
		for (i = 0; i < 15; i++) {
			if (x == enemy_pointer->l1[i][0] || x == enemy_pointer->l1[i][0] + 1
					|| x == enemy_pointer->l1[i][0] + 2) {
				if (enemy_pointer->l1[i][2] == 1) {
					for (j = 0; j < 0x1F; j++) {
						vt100_move(enemy_pointer->l1[i][0], enemy_pointer->l1[i][1]);
						serial_puts("x  ");
						vt100_move(enemy_pointer->l1[i][0], enemy_pointer->l1[i][1]);
						serial_puts("  x");
						vt100_move(enemy_pointer->l1[i][0], enemy_pointer->l1[i][1]);
						serial_puts(" x ");
						vt100_move(enemy_pointer->l1[i][0], enemy_pointer->l1[i][1]);
						serial_puts("100");
					}
					enemy_destroyed(i, 1);
					enemy_pointer->l1[i][2] = 0;
					player_score();
					completed_condition();
					return 1;
				}
			}
		}
	}
	else if (y == enemy_pointer->l2[0][1]) {
		for (i = 0; i < 15; i++) {
			if (x == enemy_pointer->l2[i][0] || x == enemy_pointer->l2[i][0] + 1
					|| x == enemy_pointer->l2[i][0] + 2) {
				if (enemy_pointer->l2[i][2] == 1) {
					for (j = 0; j < 0x1F; j++) {
						vt100_move(enemy_pointer->l2[i][0], enemy_pointer->l2[i][1]);
						serial_puts("x  ");
						vt100_move(enemy_pointer->l2[i][0], enemy_pointer->l2[i][1]);
						serial_puts("  x");
						vt100_move(enemy_pointer->l2[i][0], enemy_pointer->l2[i][1]);
						serial_puts(" x ");
						vt100_move(enemy_pointer->l2[i][0], enemy_pointer->l2[i][1]);
						serial_puts("100");
					}
					enemy_destroyed(i, 2);
					enemy_pointer->l2[i][2] = 0;
					player_score();
					completed_condition();
					return 1;
				}
			}
		}
	}
	else if (y == player_pointer->y) {
		if (x == player_pointer->x || x == player_pointer->x + 1
				|| x == player_pointer->x + 2) {
			for (i = 0; i < 0x2F; i++) {
				vt100_move(player_pointer->x, player_pointer->y);
				serial_puts("   ");
			}
			for (i = 0; i < 0x2F; i++) {
				vt100_move(player_pointer->x, player_pointer->y);
				serial_puts("(0)");
			}
			player_pointer->LIFE--;
			life_Player_display();
			return 1;
		}
	}
	return 0;
}

/*	INITIALISATION SCORE JOUEUR	*/
void player_score(void) {
	vt100_move(78, 2);
	serial_puts("00");
	score_hundred++;
	if (score_hundred == 58) {
		score_hundred = 48;
		score_thousand++;
		if (score_thousand == 58) {
			score_thousand = 48;
		}
	}
	vt100_move(77, 2);
	serial_putchar(score_hundred);
	vt100_move(76, 2);
	serial_putchar(score_thousand);
	vt100_move(68, 2);
	serial_puts("Score :");

}

uint8_t enemy_destroyed_check(uint8_t column, uint8_t line) {
	if (line == 1 && enemy_pointer->l1[column][2] == 0) {
		return 1;
	} else if (line == 2 && enemy_pointer->l2[column][2] == 0) {
		return 1;
	} else {
		return 0;
	}
}

/*	VÉRIFICATION POSITION	*/
uint8_t position_check(uint8_t line, uint8_t direction) {
	uint8_t column;
	if (direction == 0) {
		for (column = 14; column > 0; column--) {
			if (enemy_destroyed_check(column, line) == 0) {
				return column;
			}
		}
	}
	if (direction == 1) {
		for (column = 0; column != 15; column++) {
			if (enemy_destroyed_check(column, line) == 0) {
				return column;
			}
		}
	}
	return 0;
}

/*	INITIALISATION DES DÉPLACEMENTS ENNEMIES	*/
void enemy_movement(void) {
	uint8_t column;
	for (column = 0; column != 15; column++) {
		enemy_destroyed(column, 1);
		enemy_destroyed(column, 2);
		if (counter == 0) {
			enemy_pointer->l1[column][0]++;
			enemy_pointer->l2[column][0]++;
		} else if (counter == 1) {
			enemy_pointer->l1[column][0]--;
			enemy_pointer->l2[column][0]--;
		}
		enemy_display(column, 1, enemy_pointer->l1[column][0],
				enemy_pointer->l1[column][1]);
		enemy_display(column, 2, enemy_pointer->l2[column][0],
				enemy_pointer->l2[column][1]);
	}
	if (enemy_pointer->l1[position_check(1, 0)][0] == 77
			|| enemy_pointer->l2[position_check(2, 0)][0] == 77) {
		for (column = 0; column != 15; column++) {
			enemy_destroyed(column, 1);
			enemy_destroyed(column, 2);
			enemy_pointer->l1[column][1]++;
			enemy_pointer->l2[column][1]++;
			enemy_display(column, 1, enemy_pointer->l1[column][0],
					enemy_pointer->l1[column][1]);
			enemy_display(column, 2, enemy_pointer->l2[column][0],
					enemy_pointer->l2[column][1]);
		}
		counter++;
	} else if (enemy_pointer->l1[position_check(1, 1)][0] == 2
			|| enemy_pointer->l2[position_check(2, 1)][0] == 2) {
		for (column = 0; column != 15; column++) {
			enemy_destroyed(column, 1);
			enemy_destroyed(column, 2);
			enemy_pointer->l1[column][1]++;
			enemy_pointer->l2[column][1]++;
			enemy_display(column, 1, enemy_pointer->l1[column][0],
					enemy_pointer->l1[column][1]);
			enemy_display(column, 2, enemy_pointer->l2[column][0],
					enemy_pointer->l2[column][1]);
		}
		counter--;
	}
	if (enemy_pointer->l2[0][1] == player_pointer->y) {
		vt100_clear_screen();
		vt100_move(35, 10);
		serial_puts("GAME OVER");
		score_hundred = 48;
		score_thousand = 48;
		restart();
	}
}

/*INITIALISATION DES DÉPLACEMENTS JOUEUR*/
void player_movement(signed char direction) {
	/*	DÉPLACEMENTS À GAUCHE	*/
	if (direction == 'q' && player_pointer->x > 2) {
		vt100_move(player_pointer->x, player_pointer->y);
		serial_puts("   ");
		player_pointer->x--;
		vt100_move(player_pointer->x, player_pointer->y);
		serial_puts("(0)");
	}
	/*	DÉPLACEMENTS À DROITE	*/
	else if (direction == 'd' && player_pointer->x < 77) {
		vt100_move(player_pointer->x, player_pointer->y);
		serial_puts("   ");
		player_pointer->x++;
		vt100_move(player_pointer->x, player_pointer->y);
		serial_puts("(0)");
	}
}

/*	INITIALISATION CONDITIONS POUR REMPORTER LA PARTIE	*/
void completed_condition(void) {
	win++;
	if (win == 30) {
		vt100_clear_screen();
		vt100_move(30, 10);
		serial_puts("LEVEL 1 COMPLETED");
		restart();
	}
}

/*	INITIALISATION POUR RELANCER LA PARTIE	*/
void restart(void) {
	vt100_move(30, 15);
	serial_puts("Press [SPACE] to relauch");
	game_launch();
	init_player();
	init_enemy();

}

void timer(void) {
	signed char var;
	uint32_t i;
	for (i = 0; i < 0xFFF30; i++) {
		var = serial_get_last_char();
		player_movement(var);
		if (var == ' ') {
			player_fire();
			life_Player_display();
			score_display();
			return;
		}
	}
	enemy_movement();
}
