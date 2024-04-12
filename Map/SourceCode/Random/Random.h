#pragma once
#include <Windows.h>
#include <random>

/************************************************
*	�����_���N���X.
**/
class Random final
{
public:
	Random();
	~Random();

	// �����̏�����.
	static HRESULT Init();

	// �������Œ肷�邩.
	static void IsRandLock( bool Flag ) { GetInstance()->m_RandLock = Flag; }

	// �����_���Ȑ��l( float�^ )���擾.
	//	Min�ȏ�AMax������Ԃ�.
	static float GetRand( float Min, float Max );
	// �����_���Ȑ��l( int�^ )���擾.
	//	Min�ȏ�AMax�ȉ���Ԃ�.
	static int	 GetRand( int	Min,  int  Max );
	// �����_���Ȑ��l( �񋓑� )���擾.
	//	Min�ȏ�AMax�ȉ���Ԃ�.
	template<class T>
	static T	 GetRand( T		Min, T	   Max );
	// �����_���ȗv�f��Ԃ�.
	template<class T>
	static T GetRand( const std::vector<T>& Vector );

	// �m��( ���� or �Z/�Z ).
	static bool Probability( int	Molecule, int	Denominator );
	static bool Probability( float	Molecule, float Denominator );
	static bool Probability( float	Ratio );

	// ���X�g���V���b�t������.
	template<class T>
	static std::vector<T> Shuffle( std::vector<T> List );
	static std::string Shuffle( std::string Text );

private:
	// �C���X�^���X�̎擾.
	static Random* GetInstance();

private:
	// �����_���̐ݒ�.
	std::random_device	m_rnd;
	std::mt19937		m_mt;

	// �������Œ肷�邩.
	bool				m_RandLock;

private:
	// �R�s�[�E���[�u�R���X�g���N�^, ������Z�q�̍폜.
	Random( const Random & )				= delete;
	Random& operator = ( const Random & )	= delete;
	Random( Random && )						= delete;
	Random& operator = ( Random && )		= delete;
};

//---------------------------.
// �����_���Ȑ��l( �񋓑� )���擾.
//	Min�ȏ�AMax�ȉ���Ԃ�.
//---------------------------.
template<class T>
inline T Random::GetRand( T Min, T Max )
{
	return static_cast<T>( GetRand( static_cast<int>( Min ), static_cast<int>( Max ) ) );
}

//---------------------------.
// �����_���ȗv�f��Ԃ�.
//---------------------------.
template<class T>
inline T Random::GetRand( const std::vector<T>& Vector ) {
	// �����_���ŕԂ��v�f��I��.
	const int Max = static_cast<int>( Vector.size() ) - 1;
	return Vector[GetRand( 0, Max )];
}

//---------------------------.
// ���X�g���V���b�t������.
//---------------------------.
template<class T>
inline std::vector<T> Random::Shuffle( std::vector<T> List )
{
	auto* pI = GetInstance();

	std::shuffle( List.begin(), List.end(), pI->m_mt );
	return List;
}