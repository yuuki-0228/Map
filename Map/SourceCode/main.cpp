#include "Random/Random.h"
#include "Map/Map.h"

int main() {
	Random::Init();
	CMap m_map;

	m_map.Init();

	m_map.Create();

	m_map.Render();
	return 0;
}