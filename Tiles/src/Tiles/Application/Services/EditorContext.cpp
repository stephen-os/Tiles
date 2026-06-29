#include "EditorContext.h"

namespace Tiles::Services
{
    EditorContext::EditorContext(
        std::shared_ptr<Domain::IProjectRepository> projectRepository,
        std::shared_ptr<ICamera> viewportCamera)
        : m_CommandHistory(100)
        , m_ProjectService(std::move(projectRepository))
        , m_PaintingService(m_CommandHistory)
        , m_ViewportCamera(std::move(viewportCamera))
    {
    }

    std::shared_ptr<EditorContext> EditorContext::Create(
        std::shared_ptr<Domain::IProjectRepository> projectRepository,
        std::shared_ptr<ICamera> viewportCamera)
    {
        return std::make_shared<EditorContext>(
            std::move(projectRepository),
            std::move(viewportCamera)
        );
    }
}
