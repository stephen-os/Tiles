#include "Lumina/Application.h"
#include "Lumina/EntryPoint.h"

#include "Tiles/Tiles.h"

Lumina::Application* Lumina::CreateApplication(int argc, char** argv)
{
    Lumina::ApplicationSpecification spec;
    spec.Name = "Tiles";
    spec.Width = 1920;
    spec.Height = 1080;
    spec.Fullscreen = false;

    Lumina::Application* app = new Lumina::Application(spec);
    app->PushLayer<Tiles>();
    
    return app;
}