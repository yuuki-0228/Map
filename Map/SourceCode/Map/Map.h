#pragma once
#include <vector>
#include <unordered_map>

/************************************************
*	マップクラス
**/
class CMap
{
public:
	using ulong = unsigned long;

	struct Vector2
	{
		int x;
		int y;

		Vector2()
			: Vector2( 0, 0 )
		{

		}
		Vector2( int X, int Y )
			: x	( X )
			, y	( Y )
		{
		}
		Vector2 operator-( const Vector2& v ) {
			return Vector2( this->x - v.x, this->y - v.y );
		}
		Vector2 operator+( const Vector2& v ) {
			return Vector2( this->x + v.x, this->y + v.y );
		}
	};
	
	// 分割線タイプ
	enum class SplitType {
		Vertical,	// 縦
		Horizontal	// 横
	};

	// 分割線タイプ
	enum class ObjectType {
		Wall,	// 壁
		Hole	// 穴
	};

	// エリア
	struct AreaData
	{
		std::pair<Vector2, Vector2>	Position;	// 座標<左上, 右上>
		Vector2						Size;		// サイズ
		ulong						RoomId;		// 部屋データ<Room_id>
		std::unordered_map<ulong, std::vector<ulong>>	SplitData;	// 分割線データ<Split_id, Vector(Area_id)>
	};

	// 分割線
	struct SplitData
	{
		std::pair<Vector2, Vector2> Position;	// 座標<開始, 終了>
		Vector2						Size;		// サイズ
		SplitType					Type;		// タイプ

		// 縦線かどうか
		bool IsVertical() const { return Type == SplitType::Vertical; }
	};

	// 部屋
	struct RoomData
	{
		std::pair<Vector2, Vector2>	Position;		// 座標<開始, 終了>
		Vector2						Size;			// サイズ
		bool						MonsterHouse;	// モンスターハウスかどうか
		std::vector<ulong>			ObjectData;		// オブジェクトデータ<Object_id>
		std::unordered_map<ulong, std::vector<ulong>>	AisleData;	// 通路データ<Split_id, Vector(Aisle_id)>
	};

	// オブジェクト
	struct ObjectData
	{
		std::pair<Vector2, Vector2> Position;	// 座標<開始, 終了>
		Vector2						Size;		// サイズ
		ObjectType					Type;		// タイプ
	};

	// 通路
	struct AisleData
	{
		std::pair<Vector2, Vector2>				Position;	// 座標<開始, 終了>
		Vector2									Size;		// サイズ
		std::pair<ulong, std::vector<ulong>>	Adjacent;	// 隣接しているID<Room_id/Aisle_id, Room_id/Aisle_id>
		bool									RoomAisle;	// 部屋につながる通路か(true:)
	};

public:
	CMap();
	~CMap();

	// 初期化
	void Init();

	// マップ生成
	void Create();

	// 表示
	void Render( const bool dispIdMap, const bool dispDebug, const bool dispMap );

private:
	void MapUpdate();							// マップの更新

	void CreateSplit();							// 分割線の作成
	void CreateRoom();							// 部屋の作成
	void CreateWallObject( const int roomId );	// オブジェクト(壁)の作成
	void CreateAreaAisle();						// 全てのエリアに通路の作成
	bool CreateAisle(		 const AreaData& nowArea, const AreaData& Area, const ulong splitId, ulong* id = nullptr, int* aisleCreateNum = nullptr );	// 通路の作成
	void CreateConnectAisle( const ulong splitId, const ulong newAisleId, const ulong aisleId );									// 通路を繋げる通路を作成
	void CreateNotUseSplitAisle();				// 使っていない分割線から通路を作成する
	void UpdateArea( const ulong splitAreaId, const ulong newSplitId );																// エリアの更新
	void UpdateSplitArea( const AreaData& oldAreaData, const ulong splitAreaId, const ulong newAreaId, const ulong newSplitId );	// エリアの分割線の更新

	void UseSplit( const ulong splitId );		// 分割線を使用したことを保存

	bool CheckSplitConnect(const std::unordered_map<ulong, bool>& passingRoomIds, const ulong checkRoomId, const ulong roomId );

private:
	std::vector<std::vector<int>>	m_Map;			// マップ
	std::vector<AreaData>			m_Area;			// エリア
	std::vector<SplitData>			m_Split;		// 分割線
	std::vector<RoomData>			m_Room;			// 部屋
	std::vector<ObjectData>			m_Object;		// オブジェクト
	std::vector<AisleData>			m_Aisle;		// 通路

	std::vector<ulong>				m_NotUseSplit;	// まだ使用していない分割線
};
