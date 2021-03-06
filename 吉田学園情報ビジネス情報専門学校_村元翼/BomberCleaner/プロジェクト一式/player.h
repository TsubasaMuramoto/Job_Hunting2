//=============================================================
// プレイヤーの作成
// Author:村元翼
//=============================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_

//=============================================================
// インクルードファイル
//=============================================================
#include "scene.h"

//=============================================================
// マクロ定義
//=============================================================
#define MAX_SPEED			(6.0f)		// 最大速度
#define INERTIA				(0.1f)		// 慣性
#define ACCELERATION		(0.2f)		// 加速度
#define GRAVITY_SPEED		(0.8f)		// 重力の強さ
#define ROTATING_VELOCITY	(0.001f)	// プレイヤーの回転速度
#define JUMP				(13.0f)		// 重力

//=============================================================
// 前方宣言
//=============================================================
class CModel;
class CBomb;
class CShadow;
class CBillboard;

//=============================================================
// クラス定義
//=============================================================
class CPlayer : public CScene
{
public:
	//------------------------------------
	// 入力情報構造体
	//------------------------------------
	typedef enum
	{
		DIRECTION_FORWARD = 0,	// 前
		DIRECTION_BACK,			// 後
		DIRECTION_RIGHT,		// 右
		DIRECTION_LEFT,			// 左
		DIRECTION_MAX

	}MOVE_DIRECTION;

	//------------------------------------
	// プレイヤー状態構造体
	//------------------------------------
	typedef enum
	{
		STATE_NORMAL = 0,	// ノーマル
		STATE_HOLD,			// 何かを持っている
		STATE_MAX

	}PLAYER_STATE;

	CPlayer(OBJTYPE nPriority = CScene::OBJTYPE_PLAYER);		// コンストラクタ
	~CPlayer();													// デストラクタ

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot);				// 初期化
	void Uninit(void);											// 終了
	void Update(void);											// 更新
	void Draw(void);											// 描画

	// 生成
	static CPlayer *Create(D3DXVECTOR3 pos,D3DXVECTOR3 rot, int nType);

	// 移動系関数
	void Move(void);																				// 移動
	void Inertia(D3DXVECTOR3 &speed);																// 慣性
	void Gravity(D3DXVECTOR3& pos, float& fGravity, const float& fGravitySpeed, bool & bJump);		// 重力
	void Action(void);																				// アクション
	bool InputDirection(const MOVE_DIRECTION &moveDir);												// 移動方向入力関数
	void SpeedAndRotLimit(D3DXVECTOR3 &speed, D3DXVECTOR3 &rot, const float fMaxSpeed);				// 移動・回転限界
	bool Carry(void);																				// 爆弾持ち上げ関数
	void Quaternion(void);

	//---------------------------------------------------
	// 設定・取得関数
	//---------------------------------------------------
	D3DXVECTOR3 GetPos(void)					{ return m_pos; }									// 位置の取得
	D3DXVECTOR3 GetRot(void)					{ return m_rot; }									// 回転の取得
	D3DXVECTOR3 GetSpeed(void)					{ return m_Speed; }									// 速度の取得
	bool GetJump(void)							{ return m_bJump; }									// ジャンプ判定の取得
	void SetSpeed(D3DXVECTOR3 speed)			{ m_Speed = speed; }								// 速度設定
	void SetJump(bool bJump)					{ m_bJump = bJump; }								// ジャンプ判定の設定
	void SetGravity(float fGravity,bool bJump)	{ m_fGravity = fGravity , m_bJump = bJump; }		// 重力の設定

private:
	CBillboard		*m_pBillboard;					// ビルボードクラス
	CShadow			*m_pShadow;						// シャドウクラス
	CBomb			*m_pBomb;						// 爆弾クラス
	CModel			*m_pModel;						// モデルクラス
	D3DXVECTOR3		m_pos,m_Oldpos;					// 位置・最後の位置
	D3DXVECTOR3		m_posInit;						// 初期位置
	D3DXVECTOR3		m_rot;							// 回転
	D3DXVECTOR3		m_size;							// サイズ
	D3DXVECTOR3		m_Speed, m_Direction;			// 速度・向き			
	float			m_fGravitySpeed, m_fGravity;	// 重力加速度・重力
	float			m_fMaxSpeed;					// 最大速度
	float			m_fAngle;						// 回転する角度
	int				m_nType;						// タイプ
	int				m_nFrame;						// フレーム
	int				m_nPattern;						// アニメーションパターン
	bool			m_bJump;						// ジャンプ判定
	D3DXMATRIX		m_mtxWorld;						// ワールドマトリックス
	D3DXMATRIX		m_mtxRot;						// クォータニオンの計算に使うマトリックス
	PLAYER_STATE	m_state;						// プレイヤーの状態
};
#endif 
