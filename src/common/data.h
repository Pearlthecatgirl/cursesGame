#pragma once
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../common/definitions.h"

struct attr{
	char *name;
	int namec; // length of name
	char *code; // identifier
	int _value_index; // Value given in scientific notation
	double _value_specific; // Value given in scientific notation
	enum type {INT, FLOAT} type_of_value;
};

struct base {
	struct data *dat;
};

struct data {
	// meta data
	char *dataName; // in game content name
	char dataId[MAX_FILE_NAME_SIZE];
	
	// Actual data
	char *tileset;
	int tilesetc; // tileset count
	struct entity *prototype_entity_list;
	int prototype_entityc;
};
/* Entity attributes:
 *	base_hp: base hp of entity, prior to level scaling, attribute modifiers, etc. This is stored in the data file
 *	total_hp: is derived from the base_hp, as well as scaling and modifiers.
 *	hp: the current hp, can be higher than total_hp from temporary buffs.
 *	base_r: base range of the entity, prior to scaling or attribute modifiers
 *	r: the current range
 *	t: angle currently facing. Just determine this on spawn
 *  drops: the loot table. Save this into the data 
 * */
struct entity {
	int ex, ey; // X and Y position
	int base_r, r, t; // Range and Angle (theta) for direction facing and reach
	short level;
	long int hp, base_hp, total_hp;
	struct item **entity_inv; // Inventory of entities.
	struct loot_table *drops; // Loot table
	int inv_itemc; // Inventory item count
	char *name;
	char **tagv; // Tag vector
	int tagc; // Tag count

	/*static will always spawn. nonStatic is generated completely*/
	// entities are spawned as void *. typecast to these structs
	enum entity_type {
		furniture_static, furniture_nonStatic,
		passive_static, passive_nonStatic, 
		aggresive_static, aggresive_nonStatic,
	} entity_type; // Enemies, decor, etc
};

struct item{
	char *item_name;
	char *item_id;
	struct attr **attrs_array; // Array of attributes
	int attrc; // Number of Attributes
};

struct loot_table{
	char *group_name;
	struct item **itemv;
	short *rate;
	int itemc;
};

// Use this function in the game. Loads the binary data
// TODO: add obfuscation
void util_loadData(char *path, char *dataId, struct data *contentBlob);
void _loadData_txt(char *path, char *dataId, struct data *contentBlob);

