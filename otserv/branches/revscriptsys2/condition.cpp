//////////////////////////////////////////////////////////////////////
// OpenTibia - an opensource roleplaying game
//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//////////////////////////////////////////////////////////////////////
#include "otpch.h"

#include "condition.h"
#include "game.h"
#include "combat.h"
#include "player.h"
#include "configmanager.h"

extern Game g_game;
extern ConfigManager g_config;

Condition* Condition::createPeriodDamageCondition(ConditionType type,
	uint32_t interval, int32_t value, int32_t total /*= 0*/, ConditionSource source /*= CONDITION_SOURCE_NONE*/)
{
	Condition* condition = Condition::createCondition(type, 0, source);
	if(condition){
		value = std::abs(value);
		total = std::abs(total);
		Condition::Effect* effect = NULL;
		if(total > 0){
			effect = new Condition::Effect(EFFECT_PERIODIC_DAMAGE, condition->getCombatType().value(), value, total, total / value, interval);
		}
		else{
			effect = new Condition::Effect(EFFECT_PERIODIC_DAMAGE, condition->getCombatType().value(), value, 0, 0, interval);
		}
		
		condition->addEffect(effect);
		return condition;
	}

	return NULL;
}

Condition* Condition::createCondition(ConditionType type, uint32_t ticks,
	ConditionSource source /*= CONDITION_SOURCE_NONE*/)
{
	switch(type.value()){
		case enums::CONDITION_PHYSICAL:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_PHYSICALDAMAGE, source, ticks, type.value());
		case enums::CONDITION_ENERGY:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_ENERGYDAMAGE, source, ticks, type.value());
		case enums::CONDITION_EARTH:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_EARTHDAMAGE, source, ticks, type.value());
		case enums::CONDITION_FIRE:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_FIREDAMAGE, source, ticks, type.value());
		case enums::CONDITION_LIFEDRAIN:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_LIFEDRAIN, source, ticks, type.value());
		case enums::CONDITION_MANADRAIN:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_MANADRAIN, source, ticks, type.value());
		case enums::CONDITION_DROWN:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_DROWNDAMAGE, source, ticks, type.value());
		case enums::CONDITION_ICE:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_ICEDAMAGE, source, ticks, type.value());
		case enums::CONDITION_HOLY:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_HOLYDAMAGE, source, ticks, type.value());
		case enums::CONDITION_DEATH:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_DEATHDAMAGE, source, ticks, type.value());

		case enums::CONDITION_INVISIBLE:
		case enums::CONDITION_LIGHT:
		case enums::CONDITION_REGENERATION:
		case enums::CONDITION_SOULREGEN:
		case enums::CONDITION_EXHAUST_DAMAGE:
		case enums::CONDITION_EXHAUST_HEAL:
		case enums::CONDITION_EXHAUST_YELL:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_NONE, source, ticks, type.value());

		case enums::CONDITION_HUNTING:
		{
			Condition* condition = Condition::createCondition(MECHANIC_NONE, COMBAT_NONE, source, ticks, type.value());
			if(condition){
				Condition::Effect* effect = new Condition::Effect(EFFECT_PERIODIC_MOD_STAMINA, 0, -g_config.getNumber(ConfigManager::RATE_STAMINA_LOSS), 0, 0, 1000);
				condition->addEffect(effect);
			}
			return condition;
		}

		case enums::CONDITION_INFIGHT:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_NONE, source, ticks, type.value(), ICON_SWORDS);
		case enums::CONDITION_HASTE:
			return Condition::createCondition(MECHANIC_NONE, COMBAT_NONE, source, ticks, type.value(), ICON_HASTE);
		case enums::CONDITION_PARALYZE:
			return Condition::createCondition(MECHANIC_PARALYZED, COMBAT_NONE, source, ticks, type.value(), ICON_PARALYZE);
		case enums::CONDITION_DRUNK:
			return Condition::createCondition(MECHANIC_DRUNK, COMBAT_NONE, source, ticks, type.value(), ICON_DRUNK);
		case enums::CONDITION_MANASHIELD:
			return createCondition(MECHANIC_NONE, COMBAT_NONE, source, ticks, type.value(), ICON_MANASHIELD);

		case enums::CONDITION_SILENCED:
		case enums::CONDITION_MUTED_CHAT:
		case enums::CONDITION_MUTED_TRADECHAT:
			return Condition::createCondition(MECHANIC_SILENCED, COMBAT_NONE, source, ticks, type.value());

		case enums::CONDITION_SHAPESHIFT:
			return Condition::createCondition(MECHANIC_SHAPESHIFT, COMBAT_NONE, source, ticks, type.value());
		case enums::CONDITION_PACIFIED:
			return Condition::createCondition(MECHANIC_PACIFIED, COMBAT_NONE, source, ticks, type.value());
		case enums::CONDITION_DISARMED:
			return Condition::createCondition(MECHANIC_DISARMED, COMBAT_NONE, source, ticks, type.value());

		default:
			break;
	}

	return NULL;
}

Condition* Condition::createCondition(MechanicType mechanicType, CombatType combatType, ConditionSource source,
	uint32_t ticks, uint32_t categoryId, uint32_t flags /*= 0*/)
{
	return new Condition(mechanicType, combatType, source, ticks, categoryId, flags);
}

Condition* Condition::createCondition(PropStream& propStream)
{
	if(propStream.size() == 0){
		return NULL;
	}

	return createCondition(MECHANIC_NONE, COMBAT_NONE, CONDITION_SOURCE_NONE, 0, 0);
}

Condition::Condition(const Condition& rhs)
{
	mechanicType = rhs.mechanicType;
	combatType = rhs.combatType;
	source = rhs.source;
	ticks = rhs.ticks;
	id = rhs.id;
	flags = rhs.flags;
	ownerId = rhs.ownerId;

	for(std::list<Effect*>::const_iterator it = rhs.effectList.begin(); it != rhs.effectList.end(); ++it){
		addEffect(new Condition::Effect(*(*it)));
	}
}

uint16_t Condition::getIcon() const
{
	uint16_t icons = 0;

	if(hasBitSet(CONDITION_FLAG_PARTYBUFF, flags)){
		icons |= ICON_PARTY_BUFF;
	}

	switch(getId()){
		case enums::CONDITION_PHYSICAL: icons |= ICON_BURN; break;
		case enums::CONDITION_ENERGY: icons |= ICON_ENERGY; break;
		case enums::CONDITION_EARTH: icons |= ICON_POISON; break;
		case enums::CONDITION_FIRE: icons |= ICON_BURN; break;
		case enums::CONDITION_DROWN: icons |= ICON_DROWNING; break;
		case enums::CONDITION_ICE: icons |= ICON_FREEZING; break;
		case enums::CONDITION_HOLY: icons |= ICON_DAZZLED; break;
		case enums::CONDITION_DEATH: icons |= ICON_CURSED; break;

		case enums::CONDITION_DRUNK: icons |= ICON_DRUNK; break;
		case enums::CONDITION_MANASHIELD: icons |= ICON_MANASHIELD; break;
		case enums::CONDITION_PARALYZE: icons |= ICON_PARALYZE; break;
		case enums::CONDITION_HASTE: icons |= ICON_HASTE; break;
	}

	/*
	switch(combatType.value()){
		case enums::COMBAT_FIREDAMAGE: icons |= ICON_BURN; break;
		case enums::COMBAT_ENERGYDAMAGE: icons |= ICON_ENERGY; break;
		case enums::COMBAT_DROWNDAMAGE: icons |= ICON_DROWNING; break;
		case enums::COMBAT_ICEDAMAGE: icons |= ICON_FREEZING; break;
		case enums::COMBAT_HOLYDAMAGE: icons |= ICON_DAZZLED; break;
		case enums::COMBAT_DEATHDAMAGE: icons |= ICON_CURSED; break;

		default:
			break;
	}
	*/

	return icons;
}

bool Condition::onBegin(Creature* creature)
{
	for(std::list<Effect*>::iterator it = effectList.begin(); it != effectList.end(); ++it){
		if(!(*it)->onBegin(creature)){
			return false;
		}
	}

	return true;
}

void Condition::onEnd(Creature* creature, ConditionEnd reason)
{
	for(std::list<Effect*>::iterator it = effectList.begin(); it != effectList.end(); ++it){
		(*it)->onEnd(creature, reason);
	}
}

bool Condition::onUpdate(Creature* creature, const Condition* addCondition)
{
	if(getId() != addCondition->getId()){
		//different condition
		return false;
	}

	if(getSource() != addCondition->getSource()){
		//different source (SlotType)
		return false;
	}

	if(getTicks() > addCondition->getTicks()){
		return false;
	}

	mechanicType = addCondition->mechanicType;
	combatType = addCondition->combatType;
	source = addCondition->source;
	ticks = addCondition->ticks;
	id = addCondition->id;
	flags = addCondition->flags;
	ownerId = ownerId;

	bool fullUpdate = false;
	if(effectList.size() == addCondition->effectList.size()){
		std::list<Effect*>::iterator curIt = effectList.begin();
		for(std::list<Effect*>::const_iterator it = addCondition->effectList.begin(); it != addCondition->effectList.end(); ++it){
			if(!(*curIt)->onUpdate(creature, *it)){
				fullUpdate = true;
				break;
			}
		}
	}

	if(fullUpdate){
		//Condition has been changed, maybe from a script reload, doing a full update
		for(std::list<Effect*>::iterator it = effectList.begin(); it != effectList.end(); ++it){
			(*it)->onEnd(creature, CONDITIONEND_REMOVED);
			delete (*it);
		}
		effectList.clear();

		for(std::list<Effect*>::const_iterator it = addCondition->effectList.begin(); it != addCondition->effectList.end(); ++it){
			addEffect(new Effect(*(*it)) );
		}
	}

	return true;
}

bool Condition::onTick(Creature* creature, uint32_t interval)
{
	for(std::list<Effect*>::iterator it = effectList.begin(); it != effectList.end(); ++it){
		if(!(*it)->onTick(creature, interval)){
			return false;
		}
	}

	if(ticks != 0){
		if(ticks - interval <= 0){
			return false;
		}

		ticks -= interval;
	}

	return true;
}

void Condition::addEffect(Condition::Effect* effect)
{
	effect->setOwner(this);
	effectList.push_back(effect);
}

bool Condition::isPersistent() const
{
	if(ticks == 0){
		return false;
	}

	//Other sources should not be saved
	return (source == CONDITION_SOURCE_NONE);
}

bool Condition::unserialize(PropStream& propStream)
{
	uint8_t attr_type;
	while(propStream.GET_UCHAR(attr_type) && attr_type != CONDITIONATTR_END.value()){
		if(!unserializeProp(ConditionAttribute(attr_type), propStream)){
			return false;
			break;
		}
	}

	return true;
}

bool Condition::unserializeProp(ConditionAttribute attr, PropStream& propStream)
{
	if(attr == CONDITIONATTRIBUTE_MECHANIC)
	{
		int32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		mechanicType = (MechanicType)value;
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_COMBAT)
	{
		int32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		combatType = (CombatType)value;
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_TICKS)
	{
		uint32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		ticks = value;
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_ID)
	{
		uint32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		id = value;
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_FLAGS)
	{
		uint32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		flags = value;
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_TYPE)
	{
		uint32_t value = 0;
		if(!propStream.GET_VALUE(value)){
			return false;
		}

		Condition::Effect* effect = new Condition::Effect(EffectType(value), 0, 0, 0, 0, 0);
		effectList.push_back(effect);
		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODTYPE)
	{
		Condition::Effect* currEffect = effectList.back();
		if(!propStream.GET_VALUE(currEffect->mod_type)){
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODVALUE)
	{
		Condition::Effect* currEffect = effectList.back();
		if(!propStream.GET_VALUE(currEffect->mod_value)){
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODTOTAL)
	{
		Condition::Effect* currEffect = effectList.back();
		if(!propStream.GET_VALUE(currEffect->mod_total)){
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODPERCENT)
	{
		Condition::Effect* currEffect = effectList.back();
		if(!propStream.GET_VALUE(currEffect->mod_percent)){
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODTICKS)
	{
		Condition::Effect* currEffect = effectList.back();
		if(!propStream.GET_VALUE(currEffect->mod_percent)){
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTRIBUTE_EFFECT_MODPOD)
	{
		Condition::Effect* currEffect = effectList.back();
		if(currEffect->effectType == EFFECT_SHAPESHIFT){
			OutfitType outfit;
			if(!propStream.GET_VALUE(outfit)){
				return false;
			}
			currEffect->mod_pod = outfit;
		}
		else if(currEffect->effectType == EFFECT_LIGHT){
			LightInfo lightInfo;
			if(!propStream.GET_VALUE(lightInfo)){
				return false;
			}
			currEffect->mod_pod = lightInfo;
		}
		else{
			return false;
		}

		return true;
	}

	if(attr == CONDITIONATTR_END)
		return true;

	return false;
}

bool Condition::serialize(PropWriteStream& propWriteStream)
{
	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_MECHANIC);
	propWriteStream.ADD_VALUE(mechanicType.value());

	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_COMBAT);
	propWriteStream.ADD_VALUE(combatType.value());

	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_SOURCE);
	propWriteStream.ADD_VALUE(source.value());

	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_TICKS);
	propWriteStream.ADD_VALUE(ticks);

	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_ID);
	propWriteStream.ADD_VALUE(id);

	propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_FLAGS);
	propWriteStream.ADD_VALUE(flags);

	for(std::list<Effect*>::iterator it = effectList.begin(); it != effectList.end(); ++it){
		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_TYPE);
		propWriteStream.ADD_ULONG((*it)->effectType);

		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODTYPE);
		propWriteStream.ADD_ULONG((*it)->mod_type);

		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODVALUE);
		propWriteStream.ADD_ULONG((*it)->mod_value);

		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODTOTAL);
		propWriteStream.ADD_ULONG((*it)->mod_total);

		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODPERCENT);
		propWriteStream.ADD_ULONG((*it)->mod_percent);

		propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODTICKS);
		propWriteStream.ADD_ULONG((*it)->mod_ticks);

		if((*it)->mod_pod.type() == typeid(OutfitType)){
			propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODPOD);
			propWriteStream.ADD_VALUE(boost::any_cast<const OutfitType>((*it)->mod_pod));
		}
		else if((*it)->mod_pod.type() == typeid(LightInfo)){
			propWriteStream.ADD_UCHAR(*CONDITIONATTRIBUTE_EFFECT_MODPOD);
			propWriteStream.ADD_VALUE(boost::any_cast<const LightInfo>((*it)->mod_pod));
		}
	}

	return true;
}

int32_t Condition::Effect::getStatValue(Creature* creature)
{
	if(mod_percent != 0){
		switch(mod_type){
			case enums::STAT_MAXHITPOINTS:
				return (int32_t)(creature->getMaxMana() * (mod_percent / 100.f));
			
				case enums::STAT_MAXMANAPOINTS:
					return (int32_t)(creature->getMaxMana() * (mod_percent / 100.f));

			default:
				break;
		}

		if(Player* player = creature->getPlayer()){
			switch(mod_type){
				case enums::STAT_SOULPOINTS:
					return (int32_t)(player->getPlayerInfo(PLAYERINFO_SOUL) * (mod_percent / 100.f));
				case enums::STAT_MAGICPOINTS:
					return (int32_t)(player->getMagicLevel() * (mod_percent / 100.f));
				default:
					break;
			}
		}
	}
	else{
		return mod_value;
	}

	return 0;
}

int32_t Condition::Effect::getSkillValue(Creature* creature)
{
	if(mod_percent != 0){
		if(Player* player = creature->getPlayer()){
			SkillType skillType = SkillType::fromInteger(i);
			int32_t currSkill = player->getSkill(skillType, SKILL_LEVEL);
			return (int32_t)(currSkill * (mod_percent / 100.f));
		}
	}
	else{
		return mod_value;
	}

	return 0;
}

bool Condition::Effect::onBegin(Creature* creature)
{
	switch(effectType){
		case EFFECT_MOD_SPEED:
		{
			g_game.changeSpeed(creature, n);
			break;
		}

		case EFFECT_LIGHT:
		{
			//mod_ticks = getTicks()/lightInfo.level;
			const LightInfo& lightInfo = boost::any_cast<const LightInfo>(mod_pod);
			creature->setCreatureLight(lightInfo);
			g_game.changeLight(creature);
			break;
		}

		case EFFECT_SHAPESHIFT:
		{
			const OutfitType& outfit = boost::any_cast<const OutfitType>(mod_pod);
			g_game.internalCreatureChangeOutfit(creature, outfit);
			break;
		}

		case EFFECT_SCRIPT:
		{
			//TODO:
			break;
		}
		default:
			break;
	}

	if(Player* player = creature->getPlayer()){
		switch(effectType){
			case EFFECT_MOD_STAT:
			{
				//we store the change in n so we can restore it when the effect ends
				PlayerStatType statType = PlayerStatType::fromInteger(i);
				n = getStatValue(creature);
				player->setVarStats(statType, n);
				player->sendStats();
				break;
			}
			
			case EFFECT_MOD_SKILL:
			{
				//we store the change in n so we can restore it when the effect ends
				SkillType skillType = SkillType::fromInteger(i);
				n = getSkillValue(creature);
				player->setVarSkill(skillType, -n);
				player->sendSkills();
				break;
			}

			default:
				break;
		}
	}

	return true;
}

bool Condition::Effect::onEnd(Creature* creature, ConditionEnd reason)
{
	switch(effectType){
		case EFFECT_MOD_SPEED:
		{
			g_game.changeSpeed(creature, -n);
			break;
		}

		case EFFECT_LIGHT:
		{
			creature->setNormalCreatureLight();
			g_game.changeLight(creature);
			break;
		}

		case EFFECT_SHAPESHIFT:
		{
			g_game.internalCreatureChangeOutfit(creature, creature->getDefaultOutfit());
			break;
		}

		case EFFECT_SCRIPT:
		{
			//TODO:
			break;
		}

		default:
			break;
	}

	if(Player* player = creature->getPlayer()){
		switch(effectType){
			case EFFECT_MOD_STAT:
			{
				//revert our changes
				PlayerStatType statType = PlayerStatType::fromInteger(i);
				player->setVarStats(statType, -n);
				player->sendStats();
				break;
			}
			
			case EFFECT_MOD_SKILL:
			{
				//revert our changes
				SkillType skillType = SkillType::fromInteger(i);
				player->setVarSkill(skillType, -n);
				player->sendSkills();
				break;
			}

			default:
				break;
		}
	}

	return true;
}

bool Condition::Effect::onUpdate(Creature* creature, const Condition::Effect* addEffect)
{
	if(effectType != addEffect->effectType){
		return false;
	}

	onEnd(creature, CONDITIONEND_UPDATE);

	mod_type = addEffect->mod_type;
	mod_value = addEffect->mod_value;
	mod_total = addEffect->mod_total;
	mod_percent = addEffect->mod_percent;
	mod_ticks = addEffect->mod_ticks;
	mod_pod = addEffect->mod_pod;

	//misc values
	//n is used as an internal tick counter
	//n = 0;
	i = 0;
	j = 0;

	return true;
}

bool Condition::Effect::onTick(Creature* creature, uint32_t ticks)
{
	switch(effectType){
		case EFFECT_PERIODIC_HEAL:
		{
			break;
		}

		case EFFECT_PERIODIC_DAMAGE:
		{
			n += ticks;
			if(n >= mod_ticks){
				n = 0;

				CombatType combatType = CombatType::fromInteger(mod_type);
				int32_t damage = -mod_value;
				Creature* attacker = g_game.getCreatureByID(owner_condition->ownerId);
				CombatSource combatSource(attacker, NULL, true);
				//std::cout << "Dealing " << mod_value << " "  << combatType.toString() << " to " << creature->getName() << std::endl;
				if(!g_game.combatBlockHit(combatType, combatSource, creature, damage, false, false)){
					g_game.combatChangeHealth(combatType, combatSource, creature, damage);
				}

				if(mod_total != 0){
					bool skip = false;
					if(const MagicField* field = creature->getTile()->getFieldItem()){
						if(field->getCombatType() == combatType){
							//The creature is still standing in the field so the damage should
							//not be counted towards the total damage.
							skip = true;
						}
					}

					if(!skip){
						//total damage done
						j += mod_value;

						++i;
						int32_t rounds = std::ceil(((float)mod_percent) / mod_value);
						if(i >= rounds){
							i = 0;
							--mod_value;
						}

						if(j >= mod_total || mod_value == 0){
							return false;
						}
					}
				}
			}
			break;
		}

		case EFFECT_REGEN_HEALTH:
		case EFFECT_REGEN_MANA:
		case EFFECT_REGEN_SOUL:
		{
			n += ticks;
			if(creature->getZone() != ZONE_PROTECTION){
				if(n >= mod_ticks){
					n = 0;
					if(effectType == EFFECT_REGEN_HEALTH){
						creature->changeHealth(getStatValue(creature));
					}
					else if(effectType == EFFECT_REGEN_MANA){
						creature->changeMana(getStatValue(creature));
					}
					else if(Player* player = creature->getPlayer()){
						player->changeSoul(getStatValue(creature));
					}
				}
			}
			break;
		}

		case EFFECT_LIGHT:
		{
			n += ticks;
			if(n >= mod_ticks){
				n = 0;
				LightInfo creatureLight;
				creature->getCreatureLight(creatureLight);
				if(creatureLight.level > 0){
					--creatureLight.level;
					creature->setCreatureLight(creatureLight);
					g_game.changeLight(creature);
				}
			}

			break;
		}

		case EFFECT_PERIODIC_MOD_STAMINA:
		{
			n += ticks;
			if(n >= mod_ticks){
				n = 0;
				if(Player* player = creature->getPlayer()){
					if(mod_value < 0){
						player->removeStamina(ticks * std::abs(mod_value));
					}
					else{
						player->addStamina(ticks * mod_value);
					}
				}
			}
		}

		case EFFECT_SCRIPT:
		{
			//TODO:
			break;
		}

		default:
			break;
	}

	return true;
}

bool Condition::Effect::onCombat(const CombatSource& combatSource, Creature* creature, CombatType type, int32_t& amount)
{
	switch(effectType){
		case EFFECT_MANASHIELD:
		{
			//TODO:
			break;
		}

		case EFFECT_SHIELD:
		{
			//TODO:
			break;
		}

		case EFFECT_MOD_HEAL:
		case EFFECT_MOD_DAMAGE:
		{
			if(hasBitSet(type.value(), mod_type)){
				if(mod_percent != 0){
					amount = amount + (int32_t)(amount * mod_percent / 100.f);
				}
				else{
					amount = amount + mod_value;
				}
			}

			break;
		}

		case EFFECT_SCRIPT:
		{
			//TODO:
			break;
		}

		default:
			break;
	}

	return true;
}
