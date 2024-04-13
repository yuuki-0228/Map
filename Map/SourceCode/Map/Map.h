#pragma once
#include <vector>
#include <unordered_map>

/************************************************
*	�}�b�v�N���X
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
	
	// �������^�C�v
	enum class SplitType {
		Vertical,	// �c
		Horizontal	// ��
	};

	// �������^�C�v
	enum class ObjectType {
		Wall,	// ��
		Hole	// ��
	};

	// �G���A
	struct AreaData
	{
		std::pair<Vector2, Vector2>						Position;	// ���W<����, �E��>
		Vector2											Size;		// �T�C�Y
		ulong											RoomId;		// �����f�[�^<Room_id>
		std::vector<ulong>								AisleData;	// �ʘH�f�[�^<Aisle_id>
		std::unordered_map<ulong, std::vector<ulong>>	SplitData;	// �������f�[�^<Split_id, Vector(Area_id)>
	};

	// ������
	struct SplitData
	{
		std::pair<Vector2, Vector2> Position;	// ���W<�J�n, �I��>
		Vector2						Size;		// �T�C�Y
		SplitType					Type;		// �^�C�v

		// �c�����ǂ���
		bool IsVertical() const { return Type == SplitType::Vertical; }
	};

	// ����
	struct RoomData
	{
		std::pair<Vector2, Vector2> Position;		// ���W<�J�n, �I��>
		Vector2						Size;			// �T�C�Y
		std::vector<ulong>			ObjectData;		// �I�u�W�F�N�g�f�[�^<Object_id>
		bool						MonsterHouse;	// �����X�^�[�n�E�X���ǂ���
	};

	// �I�u�W�F�N�g
	struct ObjectData
	{
		std::pair<Vector2, Vector2> Position;	// ���W<�J�n, �I��>
		Vector2						Size;		// �T�C�Y
		ObjectType					Type;		// �^�C�v
	};

	// �ʘH
	struct AisleData
	{
		std::pair<Vector2, Vector2> Position;	// ���W<�J�n, �I��>
		Vector2						Size;		// �T�C�Y
		std::pair<ulong, ulong>		Adjacent;	// �אڂ��Ă���ID<Room_id/Aisle_id, Room_id/Aisle_id>
		bool						RoomAisle;	// �����ɂȂ���ʘH��(true:)
	};

public:
	CMap();
	~CMap();

	// ������
	void Init();

	// �}�b�v����
	void Create();

	// �\��
	void Render();

private:
	void MapUpdate();						// �}�b�v�̍X�V

	void CreateSplit();						// �������̍쐬
	void CreateRoom();						// �����̍쐬
	void CreateObject( const int roomId );	// �I�u�W�F�N�g�̍쐬
	void CreateAisle();						// �ʘH�̍쐬


	void UpdateArea( const ulong splitAreaId, const ulong newSplitId ); // �G���A�̍X�V
	void UpdateSplitArea( const AreaData& oldAreaData, const ulong splitAreaId, const ulong newAreaId, const ulong newSplitId ); // �G���A�̕������̍X�V

private:
	std::vector<std::vector<int>>	m_Map;		// �}�b�v
	std::vector<AreaData>			m_Area;		// �G���A
	std::vector<SplitData>			m_Split;	// ������
	std::vector<RoomData>			m_Room;		// ����
	std::vector<ObjectData>			m_Object;	// �I�u�W�F�N�g
	std::vector<AisleData>			m_Aisle;	// �ʘH
};
