////////////////////////////////////////////////
// File:	Entity.cpp
//
// Author:	Liam Russell (A.K.A. BitSlapper)
//
// Copyright:	2014-2015 Liam Russell
//
// License:	GNU GENERAL PUBLIC LICENSE V3
//////////////////////////////////////////////

#include "Entity.h"


Entity::Entity()
{
	this->m_enmEntityType = ENTITY_TYPE::NONE;
}


Entity::Entity(const Entity& entity)
{
	this->m_enmEntityType = entity.GetEntityType();
}


Entity& Entity::operator = (const Entity& entity)
{
	this->m_enmEntityType = entity.GetEntityType();
	return *this;
}

Entity::~Entity()
{

}