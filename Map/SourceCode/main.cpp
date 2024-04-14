#include "Random/Random.h"
#include "Map/Map.h"

int main() {
	bool dispIdMap = true;
	bool dispDebug = true;

	Random::Init();

	while ( 1 ) {
		system( "cls" );

		CMap m_map;
		m_map.Init();
		m_map.Create();
		m_map.Render( dispIdMap, dispDebug );

		while ( ( GetAsyncKeyState( VK_SPACE ) & 0x0001 ) == false ) {
			if ( GetAsyncKeyState( VK_ESCAPE ) & 0x0001 ) return 0;
			if ( GetAsyncKeyState( VK_RSHIFT ) & 0x0001 ) {
				dispIdMap = !dispIdMap;

				system( "cls" );
				m_map.Render( dispIdMap, dispDebug );
			}
			if ( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) {
				dispDebug = !dispDebug;

				system( "cls" );
				m_map.Render( dispIdMap, dispDebug );
			}
		}
	}
	return 0;
}