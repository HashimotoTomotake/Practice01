//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"

#include "pch.h"

#include <random>	// C++11の乱数


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

    Game();

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    virtual void OnDeviceLost() override;
    virtual void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>    m_deviceResources;

	//------------------------------------------------------------------------------------

	std::unique_ptr<DirectX::Keyboard> m_keyboard;					// キーボード
	std::unique_ptr<DirectX::SpriteBatch> m_sprites;				// スプライトバッチ																	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> *m_texture;	// テクスチャー
	std::random_device ran;		// 乱数
	bool IsAppear;				// じゃんけんを表示するか
	int EnemyNumber;			// じゃんけんで何を出すか(0->グー,1->チョキ,2->パー)
	int PlayerNumber;			// プレイヤーがじゃんけんを出す				
	int OnceUpdate;				// 一度のみ実行する
	int DrawCount;				// 引き分けになった回数

    // Rendering loop timer.
    DX::StepTimer                           m_timer;
};