#pragma once
#include "Math/Matrix.h"
#include "Math/Transform.h"

class ACamera;

/**
 * 에디터 전용 카메라
 * ACamera는 게임 전용 액터로 남겨두고, 에디터 카메라는 훨씬 기초적인 동작만으로도 충분함.
 * FViewportClient가 에디터 카메라를 소유하고, 복잡한 기능이 필요 없으니 이곳에서 구조체로 구현.
 */
struct FEditorCamera
{
    FTransform Transform;
    ECameraProjectionMode ProjectionMode;
    float FOV; // Perspective
    float ScreenSize; // Orthogonal
    float NearClip;
    float FarClip;
    float Speed;
    float Sensitivity;
    float MaxPitch;
};

/**
 * FViewportClient는 하드웨어와 가장 가까운 객체로, 여기에서 렌더를 담당한다.
 */
class FViewportClient
{
public:
    FViewportClient();
    ~FViewportClient() = default;

    /**
     * TODO: FViewportClient는 카메라 소유
     *       렌더 담당
     *       입력 처리 및 카메라 제어
     */
    virtual void Draw();

    virtual void OnResize(int32 InWidth, int32 InHeight);

    virtual void HandleInput(const float DeltaTime);

protected:
    FEditorCamera EditorCamera;

private:
    FMatrix ViewMatrix;
    FMatrix ProjectionMatrix;

    int32 Width;
    int32 Height;
};

/**
 * FViewport는 FViewportClient가 위젯과 통신할 수 있게 한번 감싼 객체.
 */
class FViewport
{
public:
    FViewport();
    ~FViewport() = default;

    /**
     * TODO: FViewport는 인터페이스에 가까움
     *       입력을 FViewportClient에 전달
     *       렌더 타겟(출력 버퍼, 화면, 텍스처 등) 제공 
     */

    virtual void Draw();

    virtual void OnResize(int32 InWidth, int32 InHeight);

    virtual void HandleInput(const float DeltaTime);
    
private:
    std::unique_ptr<FViewportClient> ViewportClient;
};