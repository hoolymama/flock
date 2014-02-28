/***************************************************************************
agent.cpp  -  description
-------------------
    begin                : Wed Mar 29 2006
    copyright            : (C) 2006 by Julian Mann
    email                : julian.mann@gmail.com

	This is just a position and id to be stored
	id isn't really used at the moment - so it's effectively just a MVector
	however it will be useful in the future to have it qrapped up in our own data type
	because then we can store say RGB values or other data

	***************************************************************************/

#include <agent.h>

agent::agent(
			 const MVector &p,
			 unsigned id
			 )	:
	m_position(p),
	//m_cachedNeighbors(0),
	m_id(id)
{
}

agent::~agent() {}

const MVector & agent::position()  const {return m_position; } 

const double agent::position(mayaMath::axis a) const {return (m_position)[a];}

MVector  agent::getPosition()  const {return m_position; }

const unsigned int & agent::id() const {return m_id;}
