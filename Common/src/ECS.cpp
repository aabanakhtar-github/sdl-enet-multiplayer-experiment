#include "ECS.h"

std::size_t ECS::Internal::next_system_ID = 0;
// first bit is reserved for whether the entity is active or not
std::size_t ECS::Internal::next_component_ID = 1;