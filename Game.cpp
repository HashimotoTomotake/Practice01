//
// Game.cpp
//

#include <random>	// C++11の乱数

#include "pch.h"
#include "Game.h"

extern void ExitGame();

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
	// 乱数用の時間
	//time_t t;
	//t = time(&t);
	//srand((unsigned int)t);

	// キーボードの作成 
	m_keyboard = std::make_unique<Keyboard>();

    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

	// まだ手を出さない
	IsAppear = false;

	// 乱数の値
	EnemyNumber = ran() % 3;

	// 1度のみ実行しない
	OnceUpdate = 0;

	// 引き分けになった回数
	DrawCount = 0;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;

	static Keyboard::KeyboardStateTracker tracker;
	Keyboard::State kb = m_keyboard->GetState();
	tracker.Update(kb);

	// 手を出す
	if (!IsAppear)
	{
		     if (tracker.pressed.Z) { PlayerNumber = 0; IsAppear = true; }
		else if (tracker.pressed.X) { PlayerNumber = 1; IsAppear = true;}
		else if (tracker.pressed.C) { PlayerNumber = 2; IsAppear = true;}
	}

	// 敵がわざと負けさせるか判断する
	if ((IsAppear)&&(OnceUpdate == 0))
	{
		int Percent= ran() % 100;
		if (Percent < 15 + 10 * DrawCount)
		{
			switch (PlayerNumber)
			{
				case 0:EnemyNumber=1;break;
				case 1:EnemyNumber=2;break;
				case 2:EnemyNumber=0;break;
			}
		}

		// 2度と実行させない
		OnceUpdate = 1;
	}

	// あいこになった場合は継続する
	if (PlayerNumber == EnemyNumber)
	{
		// 引き分けになった回数が増える
		// DrawCount++;
	}

	// リセットする
	if (tracker.pressed.Space)
	{
		IsAppear = false;
		EnemyNumber = ran() % 3;
		OnceUpdate = 0;
		DrawCount = 0;
	}
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

	// スプライトの描画
	Vector2 m_PlayerPosition = Vector2(100,300);
	Vector2 m_EnemyPosition = Vector2(500, 50);

	m_sprites->Begin();
	
	// じゃんけんで手を出す
	if (IsAppear)
	{
		m_sprites->Draw(m_texture[PlayerNumber].Get(), m_PlayerPosition, nullptr);
		m_sprites->Draw(m_texture[EnemyNumber].Get(), m_EnemyPosition, nullptr);

	}
	m_sprites->End();


    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
	auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

	// スプライトパッチの作成
	m_sprites = std::make_unique<SpriteBatch>(context);

	// テクスチャーのロード
	m_texture =  new Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>[3];
	CreateWICTextureFromFile(device, L"Resources\\gu.png", nullptr, m_texture[0].GetAddressOf());
	CreateWICTextureFromFile(device, L"Resources\\tyoki.png", nullptr, m_texture[1].GetAddressOf());
	CreateWICTextureFromFile(device, L"Resources\\pa.png", nullptr, m_texture[2].GetAddressOf());



}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
