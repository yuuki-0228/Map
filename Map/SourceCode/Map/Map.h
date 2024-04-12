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
	
	// エリア
	struct AreaData
	{
		std::pair<Vector2, Vector2>						Position;	// 座標<左上, 右上>
		Vector2											Size;		// サイズ
		ulong											Room_id;	// Room_Id
		std::pair<ulong, std::vector<ulong>>			AisleData;	// 通路データ<Room_Id, vector(Aisle_Id)>
		std::unordered_map<ulong, std::vector<ulong>>	SplitData;	// 分割線データ<Split_id, Vector(Area_Id)>
	};

	// 分割線タイプ
	enum class SplitType {
		Vertical,	// 縦
		Horizontal	// 横
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
		std::pair<Vector2, Vector2> Position;	// 座標<開始, 終了>
		Vector2						Size;		// サイズ
	};

	// 通路
	struct AisleData
	{
		std::pair<Vector2, Vector2> Position;	// 座標<開始, 終了>
		Vector2						Size;		// サイズ
	};

public:
	CMap();
	~CMap();

	// 初期化
	void Init();

	// マップ生成
	void Create();

	// 表示
	void Render();

private:
	void MapUpdate(); // マップの更新
	void CreateSplit();	// 分割線の作成
	void UpdateArea( const ulong splitAreaId, const ulong newSplitId ); // エリアの更新
	void UpdateSplitArea( const AreaData& oldAreaData, const ulong splitAreaId, const ulong newAreaId, const ulong newSplitId ); // エリアの分割線の更新

private:
	std::vector<std::vector<int>>	m_Map;		// マップ
	std::vector<AreaData>			m_Area;		// エリア
	std::vector<SplitData>			m_Split;	// 分割線
	std::vector<RoomData>			m_Room;		// 部屋
	std::vector<AisleData>			m_Aisle;	// 通路
};
