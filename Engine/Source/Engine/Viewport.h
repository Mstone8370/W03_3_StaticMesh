#pragma once
#include "Math/Matrix.h"
#include "Math/Transform.h"

class FViewport;

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
 * 어떻게 렌더할지, 무엇을 렌더할지를 결정한다.
 */
class FViewportClient
{
public:
    FViewportClient();
    virtual ~FViewportClient() = default;

    /**
     * TODO: FViewportClient는 카메라 소유
     *       렌더 담당
     *       입력 처리 및 카메라 제어
     */
    virtual void Draw(const std::weak_ptr<FViewport>& InViewport);

    virtual void OnResize(int32 InWidth, int32 InHeight);

    virtual void HandleInput(const float DeltaTime);

protected:
    std::unique_ptr<FEditorCamera> EditorCamera;

private:
    FMatrix ViewMatrix;
    FMatrix ProjectionMatrix;

    std::weak_ptr<FViewport> Viewport;
};

/**
 * FViewport는 FViewportClient가 위젯과 통신할 수 있게 한번 감싼 객체.
 * SViewport에 렌더 결과를 전달하기 위해 렌더 타겟 뷰 및 프레임 버퍼, 뷰포트 등을 관리하는
 * 객체를 소유하게 된다. (하지만 이번에는 생략하여 FViewport에서 관리)
 */
class FViewport : public std::enable_shared_from_this<FViewport>
{
public:
    FViewport();
    virtual ~FViewport() = default;

    void Init(int32 InWidth, int32 InHeight);

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

public:
    int32 TopLeftX;
    int32 TopLeftY;
    int32 Width;
    int32 Height;

    D3D11_VIEWPORT D3DViewport;
    
    ID3D11Texture2D* RenderTarget;
    ID3D11RenderTargetView* RenderTargetView;
    
    ID3D11Texture2D* DepthStencilBuffer;
    ID3D11DepthStencilView* DepthStencilView;
    
};