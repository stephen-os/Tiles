#include "Core/Application.h"
#include "Core/EntryPoint.h"

#include "Tiles/Editor.h"

Tiles::Application* Tiles::CreateApplication(int argc, char** argv)
{
    Tiles::ApplicationSpecification spec;
    spec.Name = "Tiles";
	spec.Icon = "res/assets/bucket.png";
    spec.Width = 1920;
    spec.Height = 1080;
	spec.Use2DRenderer = true;
	spec.Maximized = true;

    Tiles::Application* app = new Tiles::Application(spec);
    app->PushLayer<Tiles::Editor>();

    return app;
}