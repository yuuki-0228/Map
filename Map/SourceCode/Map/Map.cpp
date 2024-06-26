#include "Map.h"
#include "..\Random\Random.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace {
	constexpr int MAP_SIZE		= 40;					// マップサイズ
	constexpr int ROOM_SIZE_MIN = 5;					// 部屋の最小サイズ
	constexpr int AREA_SIZE_MIN = ROOM_SIZE_MIN + 2;	// エリアの最小サイズ
	constexpr int SPLIT_MIN		= 5;					// 分割線の最小数
	constexpr int SPLIT_MAX		= 10;					// 分割線の最大数

	constexpr int AREA_TYPE		= 0;					// エリア
	constexpr int ROOM_TYPE		= 1;					// 部屋
	constexpr int SPLIT_TYPE	= 2;					// 分割線
	constexpr int AISLE_TYPE	= 3;					// 通路
	constexpr int OBJECT_TYPE	= 4;					// オブジェクト

	const std::string MAP( const int mapId ) {
		switch ( mapId % 10 ) {
		case AREA_TYPE:
			return "□";
		case ROOM_TYPE:
			return "■";
		case SPLIT_TYPE:
			return "□";
		case AISLE_TYPE:
			return "■";
		case OBJECT_TYPE:
			return "□";
		default:
			return "□";
		}
	}
	const std::string MAP_TYPE( const int type ) {
		switch ( type ) {
		case AREA_TYPE:
			return "□";
		case ROOM_TYPE:
			return "■";
		case SPLIT_TYPE:
			return "△";
		case AISLE_TYPE:
			return "▲";
		case OBJECT_TYPE:
			return "☆";
		default:
			return "×";
		}
	}
	const std::string COLOR_ID( const int mapId, const bool isDispId = true ) {
		const int id	= mapId / 10;
		const int type	= mapId % 10;

		std::stringstream ss;
		isDispId ?
			ss << "\x1b[38;5;" << ( id % 15 + 1 ) << "m" << "\x1b[1m" << std::setw( 2 ) << id << MAP_TYPE( type ) << "\x1b[0m" << "\x1b[39m" :
			ss << "\x1b[38;5;" << ( id % 15 + 1 ) << "m" << "\x1b[1m" << MAP_TYPE( type ) << "\x1b[0m" << "\x1b[39m";
		return ss.str();
	}
}

CMap::CMap()
	: m_Map			()
	, m_Area		()
	, m_Split		()
	, m_Room		()
	, m_Object		()
	, m_Aisle		()
	, m_NotUseSplit	()
{
}

CMap::~CMap()
{
}

//---------------------------
// 初期化
//---------------------------
void CMap::Init()
{
	// マップ初期化
	m_Map.resize( MAP_SIZE );
	for ( auto& ml : m_Map ) {
		ml.resize( MAP_SIZE );
	}

	m_Area.emplace_back( AreaData
	{
		std::make_pair( Vector2( 0, 0 ), Vector2( MAP_SIZE -1, MAP_SIZE - 1 ) ),
		Vector2( MAP_SIZE, MAP_SIZE )
	} );
}

//---------------------------
// マップ生成
//---------------------------
void CMap::Create()
{
	// 分割線の作成
	const int num = Random::GetRand( SPLIT_MIN, SPLIT_MAX );
	for ( int i = 0; i < num; i++ ) {
		CreateSplit();
	}
	CreateRoom();				// 部屋の作成
	CreateAreaAisle();			// 通路の作成
	CreateNotUseSplitAisle();	// 使っていない分割線から通路を作成
}

//---------------------------
// 表示
//---------------------------
void CMap::Render( const bool dispIdMap, const bool dispDebug, const bool dispMap )
{
	MapUpdate();

	if ( dispMap ) {
		for ( int y = 0; y < MAP_SIZE; y++ ) {
			for ( int x = 0; x < MAP_SIZE; x++ ) {
				std::cout << MAP( m_Map[y][x] );
			}
			std::cout << std::endl;
		}
	}
	else {
		if ( dispIdMap ) {
			for ( int y = 0; y < MAP_SIZE; y++ ) {
				for ( int x = 0; x < MAP_SIZE; x++ ) {
					std::cout << COLOR_ID( m_Map[y][x] ) << ",";
				}
				std::cout << std::endl;
			}
		}
		else {
			for ( int y = 0; y < MAP_SIZE; y++ ) {
				for ( int x = 0; x < MAP_SIZE; x++ ) {
					std::cout << COLOR_ID( m_Map[y][x], false );
				}
				std::cout << std::endl;
			}
		}
	}
	std::cout << std::endl << "<< [\x1b[31mSpace\x1b[39m : \x1b[33mマップの再生成\x1b[39m] [\x1b[31mEnter\x1b[39m : \x1b[33mデバック情報の表示/非表示\x1b[39m] [\x1b[31mR_Shift\x1b[39m : \x1b[33mマップにIDの表示/非表示\x1b[39m] [\x1b[31mR_Ctrl\x1b[39m : \x1b[33m普通のマップ表示に切り替える\x1b[39m] >>" << std::endl;

	if ( dispDebug ){
		std::cout << "--------------------" << std::endl;
		std::cout << "    Area Datas" << std::endl;
		std::cout << "--------------------" << std::endl;
		const int aSize = static_cast<int>( m_Area.size() );
		for ( int i = 0; i < aSize; i++ ) {
			std::cout << "- Area    : " << COLOR_ID( i * 10 + AREA_TYPE ) << std::endl;
			std::cout << "  sPos    : x = " << m_Area[i].Position.first.x  << ", y = " << m_Area[i].Position.first.y  << std::endl;
			std::cout << "  ePos    : x = " << m_Area[i].Position.second.x << ", y = " << m_Area[i].Position.second.y << std::endl;
			std::cout << "  Size    : w = " << m_Area[i].Size.x << ", h = " << m_Area[i].Size.y << std::endl;
			std::cout << "  Room Id : " << COLOR_ID( m_Area[i].RoomId * 10 + ROOM_TYPE ) << std::endl;
			for ( auto& [sId, aList] : m_Area[i].SplitData ) {
				std::cout << "  - Split Id : " << COLOR_ID( sId * 10 + SPLIT_TYPE ) << std::endl;
				for ( auto& aId : aList ) {
					std::cout << "    Area Id  : " << COLOR_ID( aId * 10 + AREA_TYPE ) << std::endl;
				}
			}
		}
		std::cout << "--------------------" << std::endl;
		std::cout << "    Split Datas"		<< std::endl;
		std::cout << "--------------------" << std::endl;
		const int sSize = static_cast<int>( m_Split.size() );
		for ( int i = 0; i < sSize; i++ ) {
			std::cout << "- Split  : " << COLOR_ID( i * 10 + SPLIT_TYPE ) << std::endl;
			std::cout << "  sPos   : x = " << m_Split[i].Position.first.x  << ", y = " << m_Split[i].Position.first.y  << std::endl;
			std::cout << "  ePos   : x = " << m_Split[i].Position.second.x << ", y = " << m_Split[i].Position.second.y << std::endl;
			std::cout << "  Size   : w = " << m_Split[i].Size.x << ", h = " << m_Split[i].Size.y << std::endl;
			std::cout << "  Type   : " << ( m_Split[i].IsVertical() ? "Vertical" : "Horizontal" ) << std::endl;
		}
		std::cout << "--------------------" << std::endl;
		std::cout << "     Room Datas"		<< std::endl;
		std::cout << "--------------------" << std::endl;
		const int rSize = static_cast<int>( m_Room.size() );
		for ( int i = 0; i < rSize; i++ ) {
			std::cout << "- Room   : " << COLOR_ID( i * 10 + ROOM_TYPE ) << std::endl;
			std::cout << "  sPos   : x = " << m_Room[i].Position.first.x  << ", y = " << m_Room[i].Position.first.y  << std::endl;
			std::cout << "  ePos   : x = " << m_Room[i].Position.second.x << ", y = " << m_Room[i].Position.second.y << std::endl;
			std::cout << "  Size   : w = " << m_Room[i].Size.x << ", h = " << m_Room[i].Size.y << std::endl;
			std::cout << "  MonsterHouse : " << ( m_Room[i].MonsterHouse ? "true" : "false" ) << std::endl;
			std::cout << "  - ObjectData : " << std::endl;
			for ( auto& oId : m_Room[i].ObjectData ) {
				std::cout << "    Object Id  : " << COLOR_ID( oId * 10 + OBJECT_TYPE ) << std::endl;
			}
			for ( auto& [sId, aList] : m_Room[i].AisleData ) {
				std::cout << "  - Split Id : " << COLOR_ID( sId * 10 + ROOM_TYPE ) << std::endl;
				for ( auto& aId : aList ) {
					std::cout << "    Aisle Id : " << COLOR_ID( aId * 10 + AISLE_TYPE ) << std::endl;
				}
			}
		}
		std::cout << "--------------------" << std::endl;
		std::cout << "    Object Datas"		<< std::endl;
		std::cout << "--------------------" << std::endl;
		const int oSize = static_cast<int>( m_Object.size() );
		for ( int i = 0; i < oSize; i++ ) {
			std::cout << "- Object : " << COLOR_ID( i * 10 + OBJECT_TYPE ) << std::endl;
			std::cout << "  sPos   : x = " << m_Object[i].Position.first.x  << ", y = " << m_Object[i].Position.first.y  << std::endl;
			std::cout << "  ePos   : x = " << m_Object[i].Position.second.x << ", y = " << m_Object[i].Position.second.y << std::endl;
			std::cout << "  Size   : w = " << m_Object[i].Size.x << ", h = " << m_Object[i].Size.y << std::endl;
			std::cout << "  Type   : " << ( m_Object[i].Type == ObjectType::Wall ? "Wall" : "Hole" ) << std::endl;
		}
		std::cout << "--------------------" << std::endl;
		std::cout << "    Aisle Datas"		<< std::endl;
		std::cout << "--------------------" << std::endl;
		const int iSize = static_cast<int>( m_Aisle.size() );
		for ( int i = 0; i < iSize; i++ ) {
			std::cout << "- Aisle     : " << COLOR_ID( i * 10 + AISLE_TYPE ) << std::endl;
			std::cout << "  sPos      : x = " << m_Aisle[i].Position.first.x  << ", y = " << m_Aisle[i].Position.first.y  << std::endl;
			std::cout << "  ePos      : x = " << m_Aisle[i].Position.second.x << ", y = " << m_Aisle[i].Position.second.y << std::endl;
			std::cout << "  Size      : w = " << m_Aisle[i].Size.x << ", h = " << m_Aisle[i].Size.y << std::endl;
			std::cout << "  RoomAisle : " << ( m_Aisle[i].RoomAisle ? "true" : "false" ) << std::endl;
			std::cout << "  - Adjacent : " << std::endl;
			std::cout << "    - first : " << std::endl;
			std::cout << "      " << ( m_Aisle[i].RoomAisle ? "Room Id  : " : "Aisle Id : " ) << COLOR_ID( m_Aisle[i].Adjacent.first  * 10 + ( m_Aisle[i].RoomAisle ? ROOM_TYPE : AISLE_TYPE ) ) << std::endl;
			std::cout << "    - second : " << std::endl;
			for ( auto& rId : m_Aisle[i].Adjacent.second ) {
				std::cout << "      " << ( m_Aisle[i].RoomAisle ? "Room Id  : " : "Aisle Id : " ) << COLOR_ID( rId * 10 + ( m_Aisle[i].RoomAisle ? ROOM_TYPE : AISLE_TYPE ) ) << std::endl;
			}
		}
		std::cout << "--------------------" << std::endl;
		std::cout << "    NotUseSplit" << std::endl;
		std::cout << "--------------------" << std::endl;
		const int nSize = static_cast<int>( m_NotUseSplit.size() );
		for ( int i = 0; i < nSize; i++ ) {
			std::cout << "- Split     : " << COLOR_ID( m_NotUseSplit[i] * 10 + SPLIT_TYPE ) << std::endl;
		}
		if ( nSize == 0 ) std::cout << "  none" << std::endl;
		std::cout << "--------------------" << std::endl;
	}
}

//---------------------------
// マップの更新
//---------------------------
void CMap::MapUpdate()
{
	// マップ初期化
	for ( auto& ml : m_Map ) {
		for ( auto& m : ml ) {
			m = 0;
		}
	}

	// エリアのマップの反映
	const int areaSize = static_cast<int>( m_Area.size() );
	for ( int i = 0; i < areaSize; ++i ) {
		for ( int y = m_Area[i].Position.first.y; y <= m_Area[i].Position.second.y; ++y ) {
			for ( int x = m_Area[i].Position.first.x; x <= m_Area[i].Position.second.x; ++x ) {
				m_Map[y][x] = i * 10 + AREA_TYPE;
			}
		}
	}

	// 分割線のマップの反映
	const int splitSize = static_cast<int>( m_Split.size() );
	for ( int i = 0; i < splitSize; ++i ) {
		if ( m_Split[i].Type == SplitType::Vertical ) {
			for ( int y = m_Split[i].Position.first.y; y <= m_Split[i].Position.second.y; ++y ) {
				m_Map[y][m_Split[i].Position.first.x] = i * 10 + SPLIT_TYPE;
			}
		} else {
			for ( int x = m_Split[i].Position.first.x; x <= m_Split[i].Position.second.x; ++x ) {
				m_Map[m_Split[i].Position.first.y][x] = i * 10 + SPLIT_TYPE;
			}
		}
	}

	// 部屋のマップの反映
	const int roomSize = static_cast<int>( m_Room.size() );
	for ( int i = 0; i < roomSize; ++i ) {
		for ( int y = m_Room[i].Position.first.y; y <= m_Room[i].Position.second.y; ++y ) {
			for ( int x = m_Room[i].Position.first.x; x <= m_Room[i].Position.second.x; ++x ) {
				m_Map[y][x] = i * 10 + ROOM_TYPE;
			}
		}
	}

	// 通路のマップの反映
	const int aisleSize = static_cast<int>( m_Aisle.size() );
	for ( int i = 0; i < aisleSize; ++i ) {
		for ( int y = m_Aisle[i].Position.first.y; y <= m_Aisle[i].Position.second.y; ++y ) {
			for ( int x = m_Aisle[i].Position.first.x; x <= m_Aisle[i].Position.second.x; ++x ) {
				m_Map[y][x] = i * 10 + AISLE_TYPE;
			}
		}
	}

	// オブジェクトのマップの反映
	const int objectSize = static_cast<int>( m_Object.size() );
	for ( int i = 0; i < objectSize; ++i ) {
		for ( int y = m_Object[i].Position.first.y; y <= m_Object[i].Position.second.y; ++y ) {
			for ( int x = m_Object[i].Position.first.x; x <= m_Object[i].Position.second.x; ++x ) {
				m_Map[y][x] = i * 10 + OBJECT_TYPE;
			}
		}
	}
}

//---------------------------
// 分割線の作成
//---------------------------
void CMap::CreateSplit()
{
	SplitData newSplit;
	const int newSplitId	= static_cast<int>( m_Split.size() );
	int		  splitAreaId	= -1;

	// 現在のエリアの順番をシャッフルしたリストを作成
	const int areaSize = static_cast<int>( m_Area.size() );
	std::vector<std::pair<int, AreaData>> shuffleArea( areaSize );
	for ( int i = 0; i < areaSize; i++ ) {
		shuffleArea[i] = std::make_pair( i, m_Area[i] );
	}
	shuffleArea = Random::Shuffle( shuffleArea );

	// 新しい分割線を作成
	for ( int i = 0; i < areaSize; i++ ) {
		auto& [id, area] = shuffleArea[i];
		if (	  area.Size.x >  AREA_SIZE_MIN * 2 && area.Size.y >  AREA_SIZE_MIN * 2 ) newSplit.Type = Random::GetRand( SplitType::Vertical, SplitType::Horizontal );
		else if ( area.Size.x <= AREA_SIZE_MIN * 2 && area.Size.y >  AREA_SIZE_MIN * 2 ) newSplit.Type = SplitType::Horizontal;
		else if ( area.Size.x >  AREA_SIZE_MIN * 2 && area.Size.y <= AREA_SIZE_MIN * 2 ) newSplit.Type = SplitType::Vertical;
		else continue;

		newSplit.Position.first		= newSplit.IsVertical() ?
			Vector2( Random::GetRand( area.Position.first.x + AREA_SIZE_MIN, area.Position.second.x - AREA_SIZE_MIN ), area.Position.first.y ) :
			Vector2( area.Position.first.x, Random::GetRand( area.Position.first.y + AREA_SIZE_MIN, area.Position.second.y - AREA_SIZE_MIN ) );
		newSplit.Position.second	= newSplit.IsVertical() ?
			Vector2( newSplit.Position.first.x, area.Position.second.y ) :
			Vector2( area.Position.second.x, newSplit.Position.first.y );
		newSplit.Size = newSplit.Position.second - newSplit.Position.first + Vector2( 1, 1 );
		splitAreaId	  = id;
		break;
	}
	if ( splitAreaId == -1 ) return;
	m_Split.emplace_back( newSplit );
	m_NotUseSplit.emplace_back( newSplitId );

	// エリアの更新
	UpdateArea( splitAreaId, newSplitId );
}

//---------------------------
// 部屋の作成
//---------------------------
void CMap::CreateRoom()
{
	for ( auto& area : m_Area ) {
		const int newRoomId = static_cast<int>( m_Room.size() );

		// 部屋の作成
		RoomData newRoom;
		const int isBegRoom	 = Random::GetRand( 0, 1 );
		const int firstMinX  = area.Position.first.x + 1;
		const int firstMaxX  = area.Position.second.x - ROOM_SIZE_MIN;
		const int firstMinY  = area.Position.first.y + 1;
		const int firstMaxY  = area.Position.second.y - ROOM_SIZE_MIN;
		newRoom.Position.first.x	= Random::GetRand( firstMinX, firstMaxX - ( ( firstMaxX - firstMinX ) / 2 ) * isBegRoom );
		newRoom.Position.first.y	= Random::GetRand( firstMinY, firstMaxY - ( ( firstMaxY - firstMinY ) / 2 ) * isBegRoom );

		const int secondMinX = newRoom.Position.first.x + ROOM_SIZE_MIN - 1;
		const int secondMaxX = area.Position.second.x - 1;
		const int secondMinY = newRoom.Position.first.y + ROOM_SIZE_MIN - 1;
		const int secondMaxY = area.Position.second.y - 1;
		newRoom.Position.second.x	= Random::GetRand( secondMinX + ( ( secondMaxX - secondMinX ) / 2 ) * isBegRoom, secondMaxX );
		newRoom.Position.second.y	= Random::GetRand( secondMinY + ( ( secondMaxY - secondMinY ) / 2 ) * isBegRoom, secondMaxY );
		newRoom.Size = newRoom.Position.second - newRoom.Position.first + Vector2( 1, 1 );
		m_Room.emplace_back( newRoom );

		// エリアに部屋を登録
		area.RoomId = newRoomId;

		// オブジェクト生成.
		CreateWallObject( newRoomId );
	}
}

//---------------------------
// オブジェクト(壁)の作成
//---------------------------
void CMap::CreateWallObject( const int roomId )
{
	// 4つ角にオブジェクト(壁)を生成
	std::vector<Vector2> cornerPos {
		m_Room[roomId].Position.first,
		{ m_Room[roomId].Position.second.x, m_Room[roomId].Position.first.y },
		{ m_Room[roomId].Position.first.x,  m_Room[roomId].Position.second.y },
		m_Room[roomId].Position.second,
	};
	std::vector<Vector2> addSize {
		{  ( m_Room[roomId].Size.x / 2 - 2 ),  ( m_Room[roomId].Size.y / 2 - 2 ) },
		{ -( m_Room[roomId].Size.x / 2 - 2 ),  ( m_Room[roomId].Size.y / 2 - 2 ) },
		{  ( m_Room[roomId].Size.x / 2 - 2 ), -( m_Room[roomId].Size.y / 2 - 2 ) },
		{ -( m_Room[roomId].Size.x / 2 - 2 ), -( m_Room[roomId].Size.y / 2 - 2 ) },
	};

	const int size = static_cast<int>( cornerPos.size() );
	for ( int i = 0; i < size; i++ ) {
		if ( Random::Probability( 1, 3 ) ) continue;
		const int newObjectId = static_cast<int>( m_Object.size() );
		ObjectData newObject;

		// 縦か横をランダムで決める
		if ( Random::Probability( 1, 2 ) ) {
			newObject.Position.first = i % 2 == 0 ?
				cornerPos[i] :
				cornerPos[i] + Vector2( addSize[i].x, 0 );
			newObject.Position.second = 1 % 2 == 0 ?
				cornerPos[i] + Vector2( addSize[i].x, 0 ) :
				cornerPos[i];
		} else {
			newObject.Position.first = i / 2 == 0 ?
				cornerPos[i]:
				cornerPos[i] + Vector2( 0, addSize[i].y );
			newObject.Position.second = i / 2 == 0 ?
				cornerPos[i] + Vector2( 0, addSize[i].y ) :
				cornerPos[i];
		}
		newObject.Size = newObject.Position.second - newObject.Position.first + Vector2( 1, 1 );
		newObject.Type = ObjectType::Wall;

		m_Object.emplace_back( newObject );
		m_Room[roomId].ObjectData.emplace_back( newObjectId );
	}
}

//---------------------------
// 通路の作成
//---------------------------
void CMap::CreateAreaAisle()
{
	std::vector<AreaData> sortAreaData = m_Area;
	std::sort( sortAreaData.begin(), sortAreaData.end(), []( const AreaData& a, const AreaData& b ) { return a.SplitData.size() < b.SplitData.size(); } );
	for ( auto& nowArea : sortAreaData ) {
		std::vector<std::pair<ulong, std::vector<ulong>>> shuffleSplitData;

		const int splitDataSize = static_cast<int>( nowArea.SplitData.size() );
		shuffleSplitData.reserve( splitDataSize );
		for ( auto& [sId, areaList] : nowArea.SplitData ) {
			shuffleSplitData.emplace_back( std::make_pair( sId, areaList ) );
		}
		shuffleSplitData = Random::Shuffle( shuffleSplitData );

		for ( auto& [sId, areaList] : shuffleSplitData ) {
			bool		splitSkip			= false;
			auto		shuffleAreaList		= Random::Shuffle( areaList );
			const int	shuffleAreaListSize = static_cast<int>( shuffleAreaList.size() );
			for ( int i = 0; i < shuffleAreaListSize; i++ ) {
				int aisleCreateNum	= 0;

				// 通路を作成できるか
				if ( m_Room[nowArea.RoomId].AisleData.count( sId ) != 0 )
				{
					for ( auto& aId : m_Room[nowArea.RoomId].AisleData[sId] ) {
						for ( auto& rId : m_Aisle[aId].Adjacent.second ) {
							if ( rId == m_Area[shuffleAreaList[i]].RoomId ) {
								splitSkip = true;
								break;
							}
						}
					}
					if ( splitSkip ) break;
				}

				// 通路の作成
				ulong newAisleId = 0;
				if ( CreateAisle( nowArea, m_Area[shuffleAreaList[i]], sId, &newAisleId, &aisleCreateNum ) == false ) {
					splitSkip = true;
					break;
				}

				// 繋げる部屋にまだ通路がない場合作成する
				if ( m_Room[m_Area[shuffleAreaList[i]].RoomId].AisleData.count( sId ) == 0 ) {
					CreateAisle( m_Area[shuffleAreaList[i]], nowArea, sId );
				}

				// 通路通しを繋げる通路を作成
				const auto aisleId = Random::Shuffle( m_Room[m_Area[shuffleAreaList[i]].RoomId].AisleData[sId] )[0];
				CreateConnectAisle( sId, newAisleId, aisleId );
				
				// 繋げる通路に部屋Idが登録されていない場合追加する
				if ( std::find( m_Aisle[aisleId].Adjacent.second.begin(), m_Aisle[aisleId].Adjacent.second.end(), nowArea.RoomId )
					== m_Aisle[aisleId].Adjacent.second.end() )
				{
					m_Aisle[aisleId].Adjacent.second.emplace_back( nowArea.RoomId );
				}

				// 確率か通路が作れなくなったら他のエリアは繋げず終了する
				if ( i + 1 >= aisleCreateNum || Random::Probability( 1, 2 ) ) break;
			}

			// 確率で他の分割線は繋げず終了する
			if ( splitSkip == false ) UseSplit( sId );
			if ( splitSkip == false && Random::Probability( 1, 5 ) == false ) break;
		}
	}
}

//---------------------------
// 通路の作成
//---------------------------
bool CMap::CreateAisle( const AreaData& nowArea, const AreaData& Area, const ulong splitId, ulong* id, int* aisleCreateNum )
{
	const int newAisleId = static_cast<int>( m_Aisle.size() );

	AisleData newAisle;
	newAisle.RoomAisle			= true;
	newAisle.Adjacent.first		= nowArea.RoomId;
	newAisle.Adjacent.second.emplace_back( Area.RoomId );

	if ( m_Split[splitId].IsVertical() ) {
		// 左側
		if ( m_Split[splitId].Position.first.x < m_Room[nowArea.RoomId].Position.first.x ) {
			auto aisleMinPos = m_Room[nowArea.RoomId].Position.first.y;
			auto aisleMaxPos = m_Room[nowArea.RoomId].Position.second.y;
			for ( auto& oId : m_Room[nowArea.RoomId].ObjectData ) {
				if ( m_Object[oId].Position.first.x != m_Room[nowArea.RoomId].Position.first.x ) continue;

				if ( m_Object[oId].Position.first.y == m_Room[nowArea.RoomId].Position.first.y ) {
					aisleMinPos = m_Object[oId].Position.second.y + 1;
				}
				else {
					aisleMaxPos = m_Object[oId].Position.first.y - 1;
				}
			}
			if ( aisleCreateNum != nullptr ) *aisleCreateNum = ( aisleMaxPos - aisleMinPos ) / 2;

			// 部屋に繋がる通路を作成
			newAisle.Position.first	 = Vector2( m_Split[splitId].Position.first.x, Random::GetRand( aisleMinPos, aisleMaxPos ) );
			newAisle.Position.second = Vector2( m_Room[nowArea.RoomId].Position.first.x - 1, newAisle.Position.first.y );

			// 他の通路とかぶる場合作成しない
			for ( auto& aId : m_Room[nowArea.RoomId].AisleData[splitId] ) {
				if ( m_Aisle[aId].Position.first.y - 3 <= newAisle.Position.first.y && newAisle.Position.first.y <= m_Aisle[aId].Position.first.y + 3 ) {
					return false;
				}
			}
		}
		// 右側
		else {
			auto aisleMinPos = m_Room[nowArea.RoomId].Position.first.y;
			auto aisleMaxPos = m_Room[nowArea.RoomId].Position.second.y;
			for ( auto& oId : m_Room[nowArea.RoomId].ObjectData ) {
				if ( m_Object[oId].Position.second.x  != m_Room[nowArea.RoomId].Position.second.x ) continue;

				if ( m_Object[oId].Position.first.y == m_Room[nowArea.RoomId].Position.first.y )
				{
					aisleMinPos = m_Object[oId].Position.second.y + 1;
				} 
				else {
					aisleMaxPos = m_Object[oId].Position.first.y - 1;
				}
			}
			if ( aisleCreateNum != nullptr ) *aisleCreateNum = ( aisleMaxPos - aisleMinPos ) / 2;

			// 部屋に繋がる通路を作成
			newAisle.Position.first  = Vector2( m_Room[nowArea.RoomId].Position.second.x + 1, Random::GetRand( aisleMinPos, aisleMaxPos ) );
			newAisle.Position.second = Vector2( m_Split[splitId].Position.first.x, newAisle.Position.first.y );

			// 他の通路とかぶる場合作成しない
			for ( auto& aId : m_Room[nowArea.RoomId].AisleData[splitId] ) {
				if ( m_Aisle[aId].Position.first.y - 3 <= newAisle.Position.first.y && newAisle.Position.first.y <= m_Aisle[aId].Position.first.y + 3 ) {
					return false;
				}
			}
		}
	}
	else {
		// 上側
		if ( m_Split[splitId].Position.first.y < m_Area[nowArea.RoomId].Position.first.y ) {
			auto aisleMinPos = m_Room[nowArea.RoomId].Position.first.x;
			auto aisleMaxPos = m_Room[nowArea.RoomId].Position.second.x;
			for ( auto& oId : m_Room[nowArea.RoomId].ObjectData ) {
				if ( m_Object[oId].Position.first.y != m_Room[nowArea.RoomId].Position.first.y ) continue;

				if ( m_Object[oId].Position.first.x == m_Room[nowArea.RoomId].Position.first.x ) {
					aisleMinPos = m_Object[oId].Position.second.x + 1;
				}
				else {
					aisleMaxPos = m_Object[oId].Position.first.x - 1;
				}
			}
			if ( aisleCreateNum != nullptr ) *aisleCreateNum = ( aisleMaxPos - aisleMinPos ) / 2;

			// 部屋に繋がる通路を作成
			newAisle.Position.first	 = Vector2( Random::GetRand( aisleMinPos, aisleMaxPos ), m_Split[splitId].Position.first.y );
			newAisle.Position.second = Vector2( newAisle.Position.first.x, m_Room[nowArea.RoomId].Position.first.y - 1 );

			// 他の通路とかぶる場合作成しない
			for ( auto& aId : m_Room[nowArea.RoomId].AisleData[splitId] ) {
				if ( m_Aisle[aId].Position.first.x - 3 <= newAisle.Position.first.x && newAisle.Position.first.x <= m_Aisle[aId].Position.first.x + 3 ) {
					return false;
				}
			}
		}
		// 下側
		else{
			auto aisleMinPos = m_Room[nowArea.RoomId].Position.first.x;
			auto aisleMaxPos = m_Room[nowArea.RoomId].Position.second.x;
			for ( auto& oId : m_Room[nowArea.RoomId].ObjectData ) {
				if ( m_Object[oId].Position.second.y  != m_Room[nowArea.RoomId].Position.second.y ) continue;

				if ( m_Object[oId].Position.first.x == m_Room[nowArea.RoomId].Position.first.x ) {
					aisleMinPos = m_Object[oId].Position.second.x + 1;
				}
				else {
					aisleMaxPos = m_Object[oId].Position.first.x - 1;
				}
			}
			if ( aisleCreateNum != nullptr ) *aisleCreateNum = ( aisleMaxPos - aisleMinPos ) / 2;

			// 部屋に繋がる通路を作成
			newAisle.Position.first	 = Vector2( Random::GetRand( aisleMinPos, aisleMaxPos ), m_Room[nowArea.RoomId].Position.second.y + 1 );
			newAisle.Position.second = Vector2( newAisle.Position.first.x, m_Split[splitId].Position.first.y );

			// 他の通路とかぶる場合作成しない
			for ( auto& aId : m_Room[nowArea.RoomId].AisleData[splitId] ) {
				if ( m_Aisle[aId].Position.first.x - 3 <= newAisle.Position.first.x && newAisle.Position.first.x <= m_Aisle[aId].Position.first.x + 3 ) {
					return false;
				}
			}
		}
	}
	newAisle.Size = newAisle.Position.second - newAisle.Position.first + Vector2( 1, 1 );
	m_Aisle.emplace_back( newAisle );
	m_Room[nowArea.RoomId].AisleData[splitId].emplace_back( newAisleId );
	if ( id != nullptr ) *id = newAisleId;

	return true;
}

//---------------------------
// 通路を繋げる通路を作成
//---------------------------
void CMap::CreateConnectAisle( const ulong splitId, const ulong newAisleId, const ulong aisleId )
{
	const int connectAisleId	= static_cast<int>( m_Aisle.size() );

	AisleData newAisle;
	newAisle.RoomAisle			= false;
	newAisle.Adjacent.first		= newAisleId;
	newAisle.Adjacent.second.emplace_back( aisleId );

	Vector2 startPos;
	Vector2 endPos;
	if ( m_Split[splitId].IsVertical() ) {
		startPos = m_Aisle[newAisleId].Position.first.y <= m_Aisle[aisleId].Position.first.y ?
			m_Aisle[newAisleId].Position.first.x < m_Split[splitId].Position.first.x ? m_Aisle[newAisleId].Position.second	: m_Aisle[newAisleId].Position.first :
			m_Aisle[aisleId].Position.first.x	 < m_Split[splitId].Position.first.x ? m_Aisle[aisleId].Position.second		: m_Aisle[aisleId].Position.first;
		endPos	 = m_Aisle[newAisleId].Position.first.y <= m_Aisle[aisleId].Position.first.y ?
			m_Aisle[aisleId].Position.first.x	 < m_Split[splitId].Position.first.x ? m_Aisle[aisleId].Position.second		: m_Aisle[aisleId].Position.first :
			m_Aisle[newAisleId].Position.first.x < m_Split[splitId].Position.first.x ? m_Aisle[newAisleId].Position.second	: m_Aisle[newAisleId].Position.first;
	}
	else {
		startPos = m_Aisle[newAisleId].Position.first.x <= m_Aisle[aisleId].Position.first.x ?
			m_Aisle[newAisleId].Position.first.y < m_Split[splitId].Position.first.y ? m_Aisle[newAisleId].Position.second	: m_Aisle[newAisleId].Position.first :
			m_Aisle[aisleId].Position.first.y	 < m_Split[splitId].Position.first.y ? m_Aisle[aisleId].Position.second		: m_Aisle[aisleId].Position.first;
		endPos = m_Aisle[newAisleId].Position.first.x <= m_Aisle[aisleId].Position.first.x ?
			m_Aisle[aisleId].Position.first.y	 < m_Split[splitId].Position.first.y ? m_Aisle[aisleId].Position.second		: m_Aisle[aisleId].Position.first :
			m_Aisle[newAisleId].Position.first.y < m_Split[splitId].Position.first.y ? m_Aisle[newAisleId].Position.second	: m_Aisle[newAisleId].Position.first;
	}
	newAisle.Position.first  = startPos;
	newAisle.Position.second = endPos;
	newAisle.Size = newAisle.Position.second - newAisle.Position.first + Vector2( 1, 1 );

	m_Aisle.emplace_back( newAisle );
}

//---------------------------
// 使っていない分割線から通路を作成する
//---------------------------
void CMap::CreateNotUseSplitAisle()
{
	for ( auto& splitId : m_NotUseSplit ) {
		std::vector<std::pair<ulong, std::vector<ulong>>> areaSplitList;

		// 繋がっている部屋を取得
		const int areaSize = static_cast<int>( m_Area.size() );
		for ( int aId = 0; aId < areaSize; aId++ ) {
			if ( m_Area[aId].SplitData.count( splitId ) == 0 ) continue;
			areaSplitList.emplace_back( std::make_pair( aId, m_Area[aId].SplitData[splitId] ) );
		}
		areaSplitList = Random::Shuffle( areaSplitList );

		// この分割線に隣接している部屋がどことも繋がっていないか
		bool ConnectSplit = false;
		for ( auto& [aId, areaList] : areaSplitList ) {
			for ( auto& areaId : areaList ) {
				std::unordered_map<ulong, bool> passingRoomIds;
				passingRoomIds[m_Area[aId].RoomId] = true;
				if ( CheckSplitConnect( passingRoomIds, areaId, m_Area[aId].RoomId ) ) {
					ConnectSplit = true;
					break;
				}
			}
			if ( ConnectSplit ) break;
		}
		if ( ConnectSplit ) continue;

		// 孤立しているため部屋を繋げる
		ulong newAisleId = 0;
		CreateAisle( m_Area[areaSplitList[0].first], m_Area[areaSplitList[0].second[0]], splitId, &newAisleId );

		// 繋げる部屋にまだ通路がない場合作成する
		if ( m_Room[m_Area[areaSplitList[0].second[0]].RoomId].AisleData.count( splitId ) == 0 ) {
			CreateAisle( m_Area[areaSplitList[0].second[0]], m_Area[areaSplitList[0].first], splitId );
		}

		// 通路通しを繋げる通路を作成
		const auto aisleId = Random::Shuffle( m_Room[m_Area[areaSplitList[0].second[0]].RoomId].AisleData[splitId] )[0];
		CreateConnectAisle( splitId, newAisleId, aisleId );

		// 繋げる通路に部屋Idが登録されていない場合追加する
		if ( std::find( m_Aisle[aisleId].Adjacent.second.begin(), m_Aisle[aisleId].Adjacent.second.end(), m_Area[areaSplitList[0].first].RoomId )
			== m_Aisle[aisleId].Adjacent.second.end() )
		{
			m_Aisle[aisleId].Adjacent.second.emplace_back( m_Area[areaSplitList[0].first].RoomId );
		}
	}
}

//---------------------------
// エリアの更新
//---------------------------
void CMap::UpdateArea( const ulong splitAreaId, const ulong newSplitId )
{
	const int	newAreaId			= static_cast<int>( m_Area.size() );
	const auto  oldSplitAreaData	= m_Area[splitAreaId];

	// 新しいエリアの作成
	AreaData newArea;
	newArea.Position = std::make_pair( m_Split[newSplitId].IsVertical() ?
		Vector2( m_Split[newSplitId].Position.first.x + 1, m_Split[newSplitId].Position.first.y ) :
		Vector2( m_Split[newSplitId].Position.first.x, m_Split[newSplitId].Position.first.y + 1 ), m_Area[splitAreaId].Position.second );
	newArea.Size = newArea.Position.second - newArea.Position.first + Vector2( 1, 1 );
	m_Area.emplace_back( newArea );

	// エリアの大きさの更新
	m_Area[splitAreaId].Position.second = m_Split[newSplitId].IsVertical() ?
		Vector2( m_Split[newSplitId].Position.second.x - 1, m_Split[newSplitId].Position.second.y ) :
		Vector2( m_Split[newSplitId].Position.second.x, m_Split[newSplitId].Position.second.y - 1 );
	m_Area[splitAreaId].Size = m_Area[splitAreaId].Position.second - m_Area[splitAreaId].Position.first + Vector2( 1, 1 );


	// エリアの分割線の更新
	UpdateSplitArea( oldSplitAreaData, splitAreaId, newAreaId, newSplitId );
}

//---------------------------
// エリアの分割線の更新
//---------------------------
void CMap::UpdateSplitArea( const AreaData& oldAreaData, const ulong splitAreaId, const ulong newAreaId, const ulong newSplitId )
{
	for ( auto& [splitId, areaList] : oldAreaData.SplitData ) {
		// 分割したエリアの隣接している分割線とタイプが同じ
		if ( m_Split[splitId].Type == m_Split[newSplitId].Type ) {
			const auto splitPos		= m_Split[newSplitId].IsVertical() ? m_Split[splitId].Position.first.x		: m_Split[splitId].Position.first.y;
			const auto newSplitPos	= m_Split[newSplitId].IsVertical() ? m_Split[newSplitId].Position.first.x	: m_Split[newSplitId].Position.first.y;
			const auto splitAreaPos	= m_Split[newSplitId].IsVertical() ? m_Area[splitAreaId].Position.first.x	: m_Area[splitAreaId].Position.first.y;
			if ( splitPos >= newSplitPos != splitAreaPos >= newSplitPos )
			{
				// 要素を新しいエリアに変更する
				m_Area[newAreaId].SplitData[splitId] = m_Area[splitAreaId].SplitData[splitId];
				for ( auto& aId : areaList ) {
					std::replace( m_Area[aId].SplitData[splitId].begin(), m_Area[aId].SplitData[splitId].end(), splitAreaId, newAreaId );
				}
				m_Area[splitAreaId].SplitData.erase( splitId );
			}
		}
		// 分割したエリアの隣接している分割線とタイプが違う
		else {
			const auto splitAreaFirstPos	= m_Split[newSplitId].IsVertical() ? m_Area[splitAreaId].Position.first.x	: m_Area[splitAreaId].Position.first.y;
			const auto splitAreaSecondPos	= m_Split[newSplitId].IsVertical() ? m_Area[splitAreaId].Position.second.x	: m_Area[splitAreaId].Position.second.y;
			const auto newAreaFirstPos		= m_Split[newSplitId].IsVertical() ? m_Area[newAreaId].Position.first.x		: m_Area[newAreaId].Position.first.y;
			const auto newAreaSecondPos		= m_Split[newSplitId].IsVertical() ? m_Area[newAreaId].Position.second.x	: m_Area[newAreaId].Position.second.y;

			for ( auto& aId : areaList ) {
				const auto areaFirstPos		= m_Split[newSplitId].IsVertical() ? m_Area[aId].Position.first.x	: m_Area[aId].Position.first.y;
				const auto areaSecondPos	= m_Split[newSplitId].IsVertical() ? m_Area[aId].Position.second.x	: m_Area[aId].Position.second.y;
				if ( ( newAreaFirstPos <= areaFirstPos  && areaFirstPos  <= newAreaSecondPos ) ||
					 ( newAreaFirstPos <= areaSecondPos && areaSecondPos <= newAreaSecondPos ) || 
					 ( areaFirstPos <= newAreaFirstPos  && newAreaSecondPos <= areaSecondPos ) )
				{
					// 要素を追加
					m_Area[newAreaId].SplitData[splitId].emplace_back( aId );
					m_Area[aId].SplitData[splitId].emplace_back( newAreaId );
				}
				if ( !( ( splitAreaFirstPos <= areaFirstPos  && areaFirstPos  <= splitAreaSecondPos ) ||
						( splitAreaFirstPos <= areaSecondPos && areaSecondPos <= splitAreaSecondPos ) ||
						( areaFirstPos <= splitAreaFirstPos  && splitAreaSecondPos <= areaSecondPos ) ) )
				{
					// 要素を取り除く
					auto newEnd = std::remove( m_Area[splitAreaId].SplitData[splitId].begin(), m_Area[splitAreaId].SplitData[splitId].end(), aId );
					m_Area[splitAreaId].SplitData[splitId].erase( newEnd, m_Area[splitAreaId].SplitData[splitId].end() );
					
					if ( m_Area[splitAreaId].SplitData[splitId].empty() ) {
						m_Area[splitAreaId].SplitData.erase( splitId );
					}

					newEnd = std::remove( m_Area[aId].SplitData[splitId].begin(), m_Area[aId].SplitData[splitId].end(), splitAreaId );
					m_Area[aId].SplitData[splitId].erase( newEnd, m_Area[aId].SplitData[splitId].end() );
					
					if ( m_Area[aId].SplitData[splitId].empty() ) {
						m_Area[aId].SplitData.erase( splitId );
					}
				}
			}
		}
	}

	// 要素を追加
	m_Area[splitAreaId].SplitData[newSplitId].emplace_back( newAreaId );
	m_Area[newAreaId].SplitData[newSplitId].emplace_back( splitAreaId );
}

//---------------------------
// 分割線を使用したことを保存
//---------------------------
void CMap::UseSplit( const ulong splitId )
{
	// 保存するIDが常に使用している場合終了する
	if ( std::find( m_NotUseSplit.begin(), m_NotUseSplit.end(), splitId ) == m_NotUseSplit.end() ) {
		return;
	}

	// 使用していないリストから削除
	auto newEnd = std::remove( m_NotUseSplit.begin(), m_NotUseSplit.end(), splitId );
	m_NotUseSplit.erase( newEnd, m_NotUseSplit.end() );
}

bool CMap::CheckSplitConnect(const std::unordered_map<ulong, bool>& passingRoomIds, const ulong checkRoomId, const ulong roomId )
{
	bool result = false;
	for ( auto& [sId, AisleList] : m_Room[roomId].AisleData ) {
		for ( auto& aId : AisleList ) {
			for ( auto& rId : m_Aisle[aId].Adjacent.second ) {
				if ( checkRoomId == rId  ) return true;

				auto passing = passingRoomIds;
				if ( passing[rId] ) continue;

				passing[rId] = true;
				result = CheckSplitConnect( passing, checkRoomId, rId );
				if ( result ) return result;
			}
		}
	}
	return result;
}
