#pragma once

class ACamera;

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

protected:
    ACamera* Camera;

private:
    // TODO: 뷰 및 프로젝션 매트릭스 캐시는 여기에서?
};

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
    
private:
    std::unique_ptr<FViewportClient> ViewportClient;
};