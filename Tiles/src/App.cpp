#include "Lumina/Core/Application.h"
#include "Lumina/Core/EntryPoint.h"

#include "Tiles/Editor.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Tiles";
    spec.Width = 1920;
    spec.Height = 1080;
    spec.Fullscreen = false;
    spec.Dock = false;
	spec.Api = Lumina::API::OPENGL; // OpenGL is the only supported API at the moment

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<Tiles::Editor>();
    
    return app;
}