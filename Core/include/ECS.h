#ifndef SPRITE_H
#define SPRITE_H

template<typename T> 
void getNextId()
{
	static int m_ids = 0;
	return m_ids++;
}
class Component
{
};

class Entity
{
	std::vector<Component> m_Components;
public:
	template<typename T> 

};

#endif