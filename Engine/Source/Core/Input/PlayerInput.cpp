#include "pch.h" 
#include "PlayerInput.h"

#include "Editor/Windows/ConsoleWindow.h"

APlayerInput::APlayerInput()
{
    Keyboard = std::make_unique<DirectX::Keyboard>();
    Mouse = std::make_unique<DirectX::Mouse>();
    GamePad = std::make_unique<DirectX::GamePad>();
}

void APlayerInput::SetClientSize(uint32 InClientWidth, uint32 InClientHeight)
{
    ClientWidth = InClientWidth;
    ClientHeight = InClientHeight;
}

void APlayerInput::UpdateInput()
{
    auto Kb = Keyboard->GetState();
    KeyboardTracker.Update(Kb);

    // 마우스 상태 가져오기
    auto MouseState = Mouse->GetState();
    MouseTracker.Update(MouseState);

    PrevMouseState = CurrentMouseState;
    
    CurrentMouseState.LeftDown = MouseState.leftButton;
    CurrentMouseState.RightDown = MouseState.rightButton;
    CurrentMouseState.MiddleDown = MouseState.middleButton;
    CurrentMouseState.Wheel = MouseState.scrollWheelValue;
    CurrentMouseState.ClientPosition.X = MouseState.x; // 윈도우에 상대적
    CurrentMouseState.ClientPosition.Y = MouseState.y; // 윈도우에 상대적

    POINT p;
    GetCursorPos(&p);
    CurrentMouseState.ScreenPosition.X = p.x;
    CurrentMouseState.ScreenPosition.Y = p.y;
}

bool APlayerInput::IsKeyPressed(DirectX::Keyboard::Keys InKey) const
{
    return KeyboardTracker.pressed.IsKeyDown(InKey);
}

bool APlayerInput::IsKeyReleased(DirectX::Keyboard::Keys InKey) const
{
    return KeyboardTracker.released.IsKeyDown(InKey);
}

bool APlayerInput::IsKeyDown(DirectX::Keyboard::Keys InKey) const
{
    return Keyboard->GetState().IsKeyDown(InKey);
}

bool APlayerInput::IsMousePressed(bool isRight) const
{
    if (isRight)
    {
        return !PrevMouseState.RightDown && CurrentMouseState.RightDown;
    }
    return !PrevMouseState.LeftDown && CurrentMouseState.LeftDown;
}

bool APlayerInput::IsMouseReleased(bool isRight) const
{
    if (isRight)
    {
        return PrevMouseState.RightDown && !CurrentMouseState.RightDown;
    }
    return PrevMouseState.LeftDown && !CurrentMouseState.LeftDown;
}

bool APlayerInput::IsMouseDown(bool isRight) const
{
    if (isRight)
    {
        return CurrentMouseState.RightDown;
    }
    return CurrentMouseState.LeftDown;
}

FPoint APlayerInput::GetMouseDelta() const
{
    return {CurrentMouseState.ScreenPosition.X - PrevMouseState.ScreenPosition.X,
        CurrentMouseState.ScreenPosition.Y - PrevMouseState.ScreenPosition.Y};
}

FPoint APlayerInput::GetMousePositionClient() const
{
    return {CurrentMouseState.ClientPosition.X, CurrentMouseState.ClientPosition.Y};
}

void APlayerInput::GetMousePositionNDC(float& OutX, float& OutY) const
{
    float HalfWidth = static_cast<float>(ClientWidth) / 2.f;
    float HalfHeight = static_cast<float>(ClientHeight) / 2.f;
    OutX = (static_cast<float>(CurrentMouseState.ClientPosition.X) - HalfWidth) / HalfWidth;
    OutY = (static_cast<float>(CurrentMouseState.ClientPosition.Y) - HalfHeight) / HalfHeight * -1.f;
}

int32 APlayerInput::GetMouseWheel() const
{
    return CurrentMouseState.Wheel;
}

int32 APlayerInput::GetMouseWheelDelta() const
{
    return CurrentMouseState.Wheel - PrevMouseState.Wheel;
}

void APlayerInput::CacheCursorPosition()
{
    /**
     * PrevMouseState를 캐싱하는 이유:
     *   윈도우 메시지를 통한 마우스 커서 위치 업데이트는 커서가 윈도우 창을 벗어나면 업데이트 되지 않음.
     *   만약 커서가 빠르게 움직이고 있고, 캐싱해야할 시점에 커서가 윈도우를 벗어나면,
     *   캐싱 값은 스크린 포지션이므로 윈도우 바깥의 위치를 캐싱하고, 그 위치에 커서를 고정함.
     *   윈도우 바깥에 커서가 고정되니 포지션 델타값이 업데이트 되지 않아서 값이 유지되고, 시점에 지속적으로 회전하는 문제 발생.
     *   따라서 커서가 윈도우를 벗어나지 않았던 이전 시점의 위치를 캐싱함으로써 위와 같은 문제 방지.
     */
    CachedScreenPosition = PrevMouseState.ScreenPosition;
}

void APlayerInput::FixMouseCursor()
{
    CurrentMouseState.ScreenPosition = PrevMouseState.ScreenPosition;
    SetCursorPos(CachedScreenPosition.X, CachedScreenPosition.Y);
}
