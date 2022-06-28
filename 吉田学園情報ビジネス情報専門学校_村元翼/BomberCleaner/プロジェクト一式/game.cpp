//=========================================================
//�Q�[����ʂ̏���
//=========================================================
#include "game.h"
#include "fade.h"
#include "manager.h"
#include "meshfield.h"
#include "meshcylinder.h"
#include "meshsphere.h"
#include "timercount.h"
#include "stage.h"
#include "score.h"
#include "Scene3D.h"
#include "keyboard.h"
#include "XInput.h"
#include "DirectInput.h"
#include "Bomb.h"
#include "BombSpawner.h"
#include "LoadX.h"
#include "Ui.h"
#include "sound.h"

//=========================================================
// �}�N����`
//=========================================================
#define COUNT_DOWN_FRAME	(60)							// �J�E���g�_�E���̃t���[��
#define UI_SIZE				(D3DXVECTOR3(1.0f,1.0f,0.0f))	// UI�T�C�Y
#define FADE_FRAME			(90)							// �I�����Ă��烊�U���g�Ƀt�F�[�h����܂ł̃t���[��

//=========================================================
// �ÓI�����o�ϐ��̏�����
//=========================================================
CScore *CGame::m_pScore = nullptr;

//=========================================================
// �R���X�g���N�^
//=========================================================
CGame::CGame()
{
	m_nFrame			= 0;		// �t���[��
	m_pTimer			= nullptr;	// �^�C�}�[�N���X
	m_pMeshCylinder		= nullptr;	// ���b�V����
	m_pMeshField		= nullptr;	// ���b�V����
	m_pMeshSphere		= nullptr;	// ���b�V����
	m_pMeshWall			= nullptr;	// ���b�V����
	m_pStage			= nullptr;	// �X�e�[�W�N���X
	m_pUI				= nullptr;	// UI�N���X
	m_bOnce				= false;	// ��x�̂ݒʂ�ϐ�
	m_bStart			= false;	// �X�^�[�g����ϐ�
}

//=========================================================
// �f�X�g���N�^
//=========================================================
CGame::~CGame()
{

}

//=========================================================
// ������
//=========================================================
HRESULT CGame::Init(void)
{

#ifdef _DEBUG
	CBombSpawner::Create({ 800.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f }, 5);

	CBomb::Create({ 0.0f,	10.0f, -300.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
	CBomb::Create({ 100.0f, 10.0f, -300.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
	CBomb::Create({ -100.0f,10.0f, -300.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
	CBomb::Create({ 0.0f,	10.0f, -400.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
	CBomb::Create({ 100.0f, 10.0f, -400.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
	CBomb::Create({ -100.0f,10.0f, -400.0f	},	{ 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, CManager::GetInstance()->GetLoadX()->GetNum("MODTYPE_BOMB"));
#endif

	// �X�e�[�W����
	if (!m_pStage)
	{
		m_pStage = new CStage;
		if (m_pStage)
		{
			m_pStage->Init();									// ������
			m_pStage->SetStage("data/TEXT/StageSet000.txt");	// ���f���ݒ�̃t�@�C����
		}
	}

// �^�C�}�[�J�E���g����
#ifdef _DEBUG
	m_pTimer = CTimerCount::Create(D3DXVECTOR3(SCREEN_WIDTH - 250.0f, 50.0f, 0.0f), D3DXVECTOR3(50.0f, 30.0f, 0.0f), 10, false);

	m_pMeshField = CMeshField::Create({ 100.0f,100.0f,100.0f }, { 300.0f,0.0f,300.0f }, { 40.0f,0.0f,0.0f }, 6, 6);
	m_pMeshField->BindTexture(CManager::GetInstance()->GetTexture()->GetTexture("TEX_TYPE_CAUTION"));
#else
	m_pTimer = CTimerCount::Create(D3DXVECTOR3(SCREEN_WIDTH - 250.0f, 50.0f, 0.0f), D3DXVECTOR3(50.0f, 30.0f, 0.0f), 60, false);
#endif

	// �X�R�A����
	m_pScore = CScore::Create(D3DXVECTOR3(SCREEN_WIDTH - 300.0f, SCREEN_HEIGHT - 50.0f, 0.0f), D3DXVECTOR3(50.0f, 30.0f, 0.0f));

	return S_OK;
}

//=========================================================
// �I��
//=========================================================
void CGame::Uninit(void)
{
	// �X�g�b�v����
	if (CManager::GetInstance()->GetStop())
	{
		CManager::GetInstance()->SetStop(false);
	}
}

//=========================================================
// �X�V
//=========================================================
void CGame::Update(void)
{
	CTexture *pTex = CManager::GetInstance()->GetTexture();
	CSound *pSound = CManager::GetInstance()->GetSound();

	//----------------------------------------------------
	// �J�n�J�E���g�_�E��
	//----------------------------------------------------
#ifndef _DEBUG
	if (!m_bStart)
	{
		// �t���[��0�̎�
		if (m_nFrame == 0)
		{
			CManager::GetInstance()->SetStop(true);
		}

		m_nFrame++;		// �t���[���J�E���g

		// �J�E���g�R
		if (m_nFrame == COUNT_DOWN_FRAME) 
		{
			m_pUI = CUi::Create(SCREEN_CENTER, UI_SIZE, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(pTex->GetTexture("TEX_TYPE_NUMBER_THREE"));

			pSound->PlaySound(CSound::SOUND_LABEL_SE_COUNT);	// �J�E���g��
		}

		// �J�E���g�Q
		else if (m_nFrame == COUNT_DOWN_FRAME * 2) 
		{
			SetUiDelete();
			m_pUI = CUi::Create(SCREEN_CENTER, UI_SIZE, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(pTex->GetTexture("TEX_TYPE_NUMBER_TWO"));

			pSound->PlaySound(CSound::SOUND_LABEL_SE_COUNT);	// �J�E���g��
		}

		// �J�E���g�P
		else if (m_nFrame == COUNT_DOWN_FRAME * 3) 
		{
			SetUiDelete();
			m_pUI = CUi::Create(SCREEN_CENTER, UI_SIZE, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(pTex->GetTexture("TEX_TYPE_NUMBER_ONE"));

			pSound->PlaySound(CSound::SOUND_LABEL_SE_COUNT);	// �J�E���g��

		}

		// �J�E���g�O
		else if (m_nFrame == COUNT_DOWN_FRAME * 4) 
		{
			SetUiDelete();
			m_pUI = CUi::Create(SCREEN_CENTER, {UI_SIZE.x * 3,UI_SIZE.y,0.0f}, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(pTex->GetTexture("TEX_TYPE_GAMESTART"));

			pSound->PlaySound(CSound::SOUND_LABEL_SE_GAMESTART);	// �X�^�[�g��
		}

		// �Q�[���X�^�[�g
		else if (m_nFrame == COUNT_DOWN_FRAME * 5)
		{
			SetUiDelete();
			m_nFrame = 0;
			m_bStart = true;
			CManager::GetInstance()->SetStop(false);
		}
	}
#endif

	//----------------------------------------------------
	// �I���T�C��
	//----------------------------------------------------
	if (m_pTimer->GetLimit())
	{
		if (!m_bOnce)
		{
			m_pUI = CUi::Create(SCREEN_CENTER, { UI_SIZE.x * 3,UI_SIZE.y,0.0f }, CUi::UI_TYPE_ZOOM);
			m_pUI->BindTexture(pTex->GetTexture("TEX_TYPE_END"));
			CManager::GetInstance()->SetStop(true);
			m_bOnce = true;

			pSound->PlaySound(CSound::SOUND_LABEL_SE_TIMEUP);	// �I����
		}

		m_nFrame++;	// �t���[���J�E���g

		if (m_nFrame == FADE_FRAME)
		{
			SetUiDelete();
			CManager::GetInstance()->GetFade()->SetFade(CManager::MODE::RESULT);
		}
	}

}
//=========================================================
// �`��
//=========================================================
void CGame::Draw(void)
{

}

//=========================================================
// UI�폜�֐�
//=========================================================
void CGame::SetUiDelete(void)
{
	if (m_pUI)
	{
		m_pUI->Uninit();
		delete m_pUI;
		m_pUI = nullptr;
	}
}





