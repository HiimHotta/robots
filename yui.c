#include "../robot_fight.h"/* Feito por Yan, O Monitor Mais Rápido do Oeste */

static Direction lookDir;
static char charging;
static int auxHP;
static int damage;
static char goodCondition;
static char shoot;
static char aux;
static char tmp;

int valid(Position p, int m, int n) {
	return p.x >= 0 && p.x < m && p.y >= 0 && p.y < n;
}

void prepareGame(Grid *g, Position p, int turnCount) {
	charging = 0;
	auxHP = 100;
	goodCondition = 1;
	damage = 0;
	shoot = 0;
	aux = 1;
	tmp = 0;
	setName("Yui");
}

Action bestTurn(Direction from, Direction to) {
	if(((6 + from - to) % 6) < 3) return TURN_LEFT;
	else return TURN_RIGHT;
}

Action shoot(Direction from, Direction to) {
	if(to == ((from + 1) % 6))
		return SHOOT_RIGHT;
	else if(to == from)
		return SHOOT_CENTER;
	else
		return SHOOT_LEFT;
}

int isEmptyControl(Tile *t) {
	return t->isControlPoint && t->type == NONE;
}

int isRobot(Tile *t) {
	return t->type == ROBOT;
}

char find(Grid *g, Position p, int (*check)(Tile*)) {
	int i, s_look = 0, s_size = 100000, dist;
	for(i = 0; i < 6; i++) {
		Position s = getNeighbor(p, i);
		dist = 1;
		while(valid(s, g->m, g->n)) {
			if(check(&g->map[s.x][s.y])) {
				s_look = i;
				s_size = dist;
			}
			s = getNeighbor(s, i);
			if(++dist > s_size)
				break;
		}
	}
	lookDir = s_look;
	return lookDir != 0;
}

Action processTurn(Grid *g, Position p, int turnsLeft) {
	Robot *r = &g->map[p.x][p.y].object.robot;
	if (r->hp == auxHP)
		damage = 0;
	if (r->hp <auxHP)
		damage++;
	if (r->hp < 30)
		goodCondition = 0;
	auxHP = r->hp;

	if((r->bullets == 0 || charging) && damage < 1) {
		charging = 1;
		if(g->map[p.x][p.y].isControlPoint) {
			if(r->bullets >= 20)
				charging = 0;
			return STAND;
		}
		if(find(g, p, isEmptyControl)) {
			if(lookDir == r->dir)
				return WALK;
			else
				return bestTurn(r->dir, lookDir);
		}
		else if(valid(getNeighbor(p, r->dir), g->m, g->n))
			return WALK;
		else
			return TURN_LEFT;
	}
	else if(find(g, p, isRobot) && goodCondition) {
		if(lookDir - r->dir == 0) {
			playSong("morra.ogg");
			return shoot(r->dir, lookDir);
		}
		else if (((lookDir - r->dir) % 6) >= -1 && ((lookDir - r->dir) % 6) <= 1)
			return bestTurn(r->dir, lookDir);
		else if (damaged < 2)
			return bestTurn (r->dir, lookDir);
		else
			return WALK;
	}
	else if (goodCondition == 0 && damage != 0) {
		if (lookDir - r->dir == 0)
		 return TURN_LEFT;
		return WALK; 
	}
	else if(valid(getNeighbor(p, r->dir), g->m, g->n) && aux) {
		aux = 0;
		return OBSTACLE_CENTER;
	}
	else {
		aux = 1;
		return WALK;
	}
}
