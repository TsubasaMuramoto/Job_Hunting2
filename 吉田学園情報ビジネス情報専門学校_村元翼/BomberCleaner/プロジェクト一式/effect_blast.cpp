//========================================
// 爆風エフェクト(メッシュ球)の作成
// Author : 村元翼
//========================================
#include "effect_blast.h"
#include "model.h"
#include "manager.h"
#include "renderer.h"
#include "effect.h"
#include "LoadX.h"
#include "meshsphere.h"
#include "score.h"
#include "game.h"
#include "Bomb.h"
#include "sound.h"

//===========================================
// マクロ定義
//===========================================
#define BLAST_SCALE_SPEED		(1.6f)	// 爆風スケール速度
#define BLAST_ROLLING_SPEED		(0.5f)	// 爆風回転速度
#define EXPLOSION_SCORE			(100)	// 爆破スコア
#define START_TRANSPARENT_FRAME (60)	// 透明になり始めるフレーム数
#define ALPHA_SUB				(0.05f) // α値減少量

//=============================================================================
// コンストラクタ
//=============================================================================
CBlast::CBlast(OBJTYPE nPriority) : CScene(nPriority)
{
	memset(&m_pSphere, 0, sizeof(m_pSphere));			// 球クラス
	m_pos			= { 0.0f, 0.0f, 0.0f };				// 位置
	m_rot			= { 0.0f, 0.0f, 0.0f };				// 回転
	m_size			= { 0.0f, 0.0f, 0.0f };				// サイズ
	m_col			= { 0.0f, 0.0f, 0.0f, 1.0f };		// 色
	m_nFrame		= 0;								// フレーム数
	m_scale			= 0.0f;								// スケール
	m_fScalingSpeed = BLAST_SCALE_SPEED;				// スケール変更速度
}

//=============================================================================
// デストラクタ
//=============================================================================
CBlast::~CBlast()
{

}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CBlast::Init(D3DXVECTOR3 pos, D3DXVECTOR3 size)
{
	m_pos = pos;		// 位置
	m_size = size;		// サイズ

	// 爆発音
	CSound *pSound = CManager::GetInstance()->GetSound();
	pSound->PlaySound(CSound::SOUND_LABEL_SE_EXPLOSION);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void CBlast::Uninit(void)
{
	// メッシュ球破棄
	if (m_pSphere)
	{
		// 死亡判定
		if (!m_pSphere->GetDeath())
		{
			m_pSphere->Uninit();
			m_pSphere = nullptr;
		}
	}

	// オブジェクトの破棄
	Release();
}

//=============================================================================
// 更新処理
//=============================================================================
void CBlast::Update(void)
{
	// 爆発中の処理
	if (m_pSphere)
	{
		for (int nCnt = 0; nCnt < 2; nCnt++)
		{
			CScene *pScene = nullptr;
			switch (nCnt)
			{
			case 0:
				pScene = CScene::GetScene(OBJTYPE_MODEL);
				break;

			case 1:
				pScene = CScene::GetScene(OBJTYPE_BOMB);
				break;
			}

			// シーンがnullになるまで通る
			while (pScene)
			{
				// 次のシーンを取得
				CScene *pSceneNext = CScene::GetSceneNext(pScene);

				// モデルタイプの取得
				CScene::MODTYPE modtype = pScene->GetModelType();

				// タイプ別当たり判定処理
				switch (modtype)
				{
				case MODTYPE_TARGET:	// 木箱やゴミなどの壊す対象
					if (m_pSphere->SphereCollisionSphere((m_pSphere->GetSize().x * m_scale) / 2, pScene))
					{
						CGame *pGame = CManager::GetInstance()->GetGame();
						CScore *pScore = pGame->GetScore();		// ゲームクラスのスコアを取得
						pScore->AddScore(EXPLOSION_SCORE);		// スコアの加算
						pScene->Uninit();						// ターゲットの終了
					}
					break;

				case MODTYPE_BOMB:		// 爆弾
					if (m_pSphere->SphereCollisionSphere((m_pSphere->GetSize().y * m_scale) / 2, pScene))
					{
						CBomb *pBomb = (CBomb*)pScene;
						pBomb->Explosion();				// 爆発する
						pBomb->Uninit();				// 爆弾の終了
					}
					break;
				}

				// 次のシーンを現在のシーンにする
				pScene = pSceneNext;
			}
		}

		Spread();	// 爆風広がり

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void CBlast::Draw(void)
{

}

//=============================================================================
// 爆風の広がり方の処理
//=============================================================================
void CBlast::Spread(void)
{
	float Nextsub = m_fScalingSpeed;

	m_nFrame++;	// フレームカウント

	// フレームが一定を超えると徐々に透明になる
	if (m_nFrame >= START_TRANSPARENT_FRAME)
	{
		m_col.a -= ALPHA_SUB;
	}

	// 爆発が広がる
	else
	{
		m_scale += m_fScalingSpeed;
		m_fScalingSpeed -= Nextsub * 0.1f;
	}

	m_rot.y += BLAST_ROLLING_SPEED;						// 回転する

	// 各種変数の設定
	m_pSphere->SetRot(m_rot);
	m_pSphere->SetScale({ m_scale,m_scale ,m_scale });
	m_pSphere->SetCol(m_col);

	if (m_col.a <= 0.0f)	// 完全に透明になるまで終了しない
	{
		Uninit();
	}
}

//=============================================================================
// インスタンス生成処理
//=============================================================================
CBlast *CBlast::Create(D3DXVECTOR3 pos, D3DXVECTOR3 size, D3DXCOLOR col, int nLine, int nVertical)
{
	// インスタンス生成
	CBlast *pBlast = nullptr;
	if (!pBlast)
	{
		pBlast = new CBlast();
		if (pBlast)
		{
			pBlast->m_col = col;					// カラー
			pBlast->Init(pos, size);				// 初期化

			// スフィアの生成
			if (!pBlast->m_pSphere)
			{
				pBlast->m_pSphere = CMeshSphere::Create(pos, size, {0.0f,0.0f,0.0f}, nLine, nVertical, false, CScene::OBJTYPE_EFFECTMESH);
			}
		}
	}
	return pBlast;
}