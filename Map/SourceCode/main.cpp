#include "Random/Random.h"
#include "Map/Map.h"

int main() {
	bool dispIdMap = false;
	bool dispDebug = false;
	bool dispMap   = true;

	Random::Init();

	while ( 1 ) {
		system( "cls" );

		CMap m_map;
		m_map.Init();
		m_map.Create();
		m_map.Render( dispIdMap, dispDebug, dispMap );

		while ( ( GetAsyncKeyState( VK_SPACE ) & 0x0001 ) == false ) {
			if ( GetAsyncKeyState( VK_ESCAPE ) & 0x0001 ) return 0;
			if ( GetAsyncKeyState( VK_RSHIFT ) & 0x0001 ) {
				dispIdMap = !dispIdMap;

				system( "cls" );
				m_map.Render( dispIdMap, dispDebug, dispMap );
			}
			if ( GetAsyncKeyState( VK_RETURN ) & 0x0001 ) {
				dispDebug = !dispDebug;

				system( "cls" );
				m_map.Render( dispIdMap, dispDebug, dispMap );
			}
			if ( GetAsyncKeyState( VK_RCONTROL ) & 0x0001 ) {
				dispMap = !dispMap;

				system( "cls" );
				m_map.Render( dispIdMap, dispDebug, dispMap );
			}
		}
	}
	return 0;
}