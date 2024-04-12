#include "Map.h"
#include "..\Random\Random.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace {
	constexpr int MAP_SIZE		= 30;	// マップサイズ
	constexpr int AREA_SIZE_MIN = 5;	// エリアの最小サイズ

	const std::string MAP_TYPE( const int type ) {
		switch ( type ) {
		case 0:	// エリア
			return "□";
		case 1:	// 部屋
			return "■";
		case 2:	// 分割線
			return "△";
		case 3:	// 通路
			return "▲";
		default:// その他
			return "×";
		}
	}
	const std::string COLOR_ID( const int mapId ) {
		const int id	= mapId / 10;
		const int type	= mapId % 10;

		std::stringstream ss;
		ss << "\x1b[38;5;" << ( id % 15 + 1 ) << "m" << "\x1b[1m" << std::setw( 2 ) << id << MAP_TYPE( type ) << "\x1b[0m" << "\x1b[39m";
		return ss.str();
	}
}

CMap::CMap()
	: m_Map		()
	, m_Area	()
	, m_Split	()
	, m_Room	()
	, m_Aisle	()
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
	for ( int i = 0; i < 10; i++ ) {
		CreateSplit();
	}
}

//---------------------------
// 表示
//---------------------------
void CMap::Render()
{
	MapUpdate();

	for ( int y = 0; y < MAP_SIZE; y++) {
		for ( int x = 0; x < MAP_SIZE; x++) {
			std::cout << COLOR_ID( m_Map[y][x] ) << ",";
		}
		std::cout << std::endl;
	}

	std::cout << "--------------------" << std::endl;
	const int aSize = m_Area.size();
	for ( int i = 0; i < aSize; i++ ) {
		std::cout << "- Area : " << COLOR_ID( i * 10 + 0 ) << std::endl;
		std::cout << "  sPos x : " << m_Area[i].Position.first.x  << ", y : " << m_Area[i].Position.first.y  << std::endl;
		std::cout << "  ePos x : " << m_Area[i].Position.second.x << ", y : " << m_Area[i].Position.second.y << std::endl;
		std::cout << "  Size w : " << m_Area[i].Size.x << ", h : " << m_Area[i].Size.y << std::endl;
		std::cout << "  Room Id : " << COLOR_ID( m_Area[i].Room_id * 10 + 1 ) << std::endl;
		for ( auto& [sId, aList] : m_Area[i].SplitData ) {
			std::cout << "  - Split Id : " << COLOR_ID( sId * 10 + 2 ) << std::endl;
			for ( auto& aId : aList ) {
				std::cout << "    Area Id : " << COLOR_ID( aId * 10 + 0 ) << std::endl;
			}
		}
	}
	std::cout << "--------------------" << std::endl;
	const int sSize = m_Split.size();
	for ( int i = 0; i < sSize; i++ ) {
		std::cout << "- Split : " << COLOR_ID( i * 10 + 2 ) << std::endl;
		std::cout << "  sPos x : " << m_Split[i].Position.first.x  << ", y : " << m_Split[i].Position.first.y  << std::endl;
		std::cout << "  ePos x : " << m_Split[i].Position.second.x << ", y : " << m_Split[i].Position.second.y << std::endl;
		std::cout << "  Size w : " << m_Split[i].Size.x << ", h : " << m_Split[i].Size.y << std::endl;
		std::cout << "  Type : " << ( m_Split[i].IsVertical() ? "Vertical" : "Horizontal" ) << std::endl;
	}
	std::cout << "--------------------" << std::endl;
	const int rSize = m_Room.size();
	for ( int i = 0; i < rSize; i++ ) {
		std::cout << "- Room : " << COLOR_ID( i * 10 + 1 ) << std::endl;
		std::cout << "  sPos x : " << m_Room[i].Position.first.x  << ", y : " << m_Room[i].Position.first.y  << std::endl;
		std::cout << "  ePos x : " << m_Room[i].Position.second.x << ", y : " << m_Room[i].Position.second.y << std::endl;
		std::cout << "  Size w : " << m_Room[i].Size.x << ", h : " << m_Room[i].Size.y << std::endl;
	}
	std::cout << "--------------------" << std::endl;
	const int iSize = m_Aisle.size();
	for ( int i = 0; i < iSize; i++ ) {
		std::cout << "- Aisle : " << COLOR_ID( i * 10 + 3 ) << std::endl;
		std::cout << "  sPos x : " << m_Aisle[i].Position.first.x  << ", y : " << m_Aisle[i].Position.first.y  << std::endl;
		std::cout << "  ePos x : " << m_Aisle[i].Position.second.x << ", y : " << m_Aisle[i].Position.second.y << std::endl;
		std::cout << "  Size w : " << m_Aisle[i].Size.x << ", h : " << m_Aisle[i].Size.y << std::endl;
	}
	std::cout << "--------------------" << std::endl;
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
	const int areaId = m_Area.size();
	for ( int i = 0; i < areaId; ++i ) {
		for ( int y = m_Area[i].Position.first.y; y <= m_Area[i].Position.second.y; ++y ) {
			for ( int x = m_Area[i].Position.first.x; x <= m_Area[i].Position.second.x; ++x ) {
				m_Map[y][x] = i * 10 + 0;
			}
		}
	}


	// 通路のマップの反映
	const int splitId = m_Split.size();
	for ( int i = 0; i < splitId; ++i ) {
		if ( m_Split[i].Type == SplitType::Vertical ) {
			for ( int y = m_Split[i].Position.first.y; y <= m_Split[i].Position.second.y; ++y ) {
				m_Map[y][m_Split[i].Position.first.x] = i * 10 + 2;
			}
		} else {
			for ( int x = m_Split[i].Position.first.x; x <= m_Split[i].Position.second.x; ++x ) {
				m_Map[m_Split[i].Position.first.y][x] = i * 10 + 2;
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
	const int newSplitId	= m_Split.size();
	int		  splitAreaId	= -1;

	// 現在のエリアの順番をシャッフルしたリストを作成
	const int areaSize = m_Area.size();
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


	// エリアの更新
	UpdateArea( splitAreaId, newSplitId );
}

//---------------------------
// エリアの更新
//---------------------------
void CMap::UpdateArea( const ulong splitAreaId, const ulong newSplitId )
{
	const int	newAreaId			= m_Area.size();
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
