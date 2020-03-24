#ifndef GAME_H_
#define GAME_H_

void init_enemy(void);

void init_player(void);

void life_Player_display(void);
void score_display(void);
void enemy_display(uint8_t, uint8_t, uint8_t, uint8_t);
void enemy_destroyed(uint8_t, uint8_t);
void enemy_fire(void);
void player_fire(void);
uint8_t enemy_destroyed_check(uint8_t, uint8_t);
uint8_t position_check(uint8_t, uint8_t);
uint8_t hitbox(uint8_t, uint8_t);
void player_score(void);
void enemy_movement(void);
void player_movement(signed char);
void completed_condition(void);
void restart(void);
void timer(void);

#endif /* GAME_H_ */
