#include "Random/Random.h"
#include "Map/Map.h"

int main() {
	Random::Init();

	while ( 1 ) {
		system( "cls" );

		CMap m_map;
		m_map.Init();
		m_map.Create();
		m_map.Render();

		while ( ( GetAsyncKeyState( VK_SPACE ) & 0x0001 ) == false ) {
			if ( ( GetAsyncKeyState( VK_ESCAPE ) & 0x0001 ) ) return 0;
		}
	}
	return 0;
}