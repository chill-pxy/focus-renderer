import engine;

int main(int argc, char** argv)
{
//    std::filesystem::path executable_path(argv[0]);
//    std::filesystem::path config_file_path = executable_path.parent_path().parent_path() / "engine/configs/deployment/SmoothEditor.ini";
//
//    Smooth::SmoothEngine* engine=new Smooth::SmoothEngine();
//
//    engine->startEngine(config_file_path.generic_string());
//
//    Smooth::SmoothEditor* editor=new Smooth::SmoothEditor();
//
//    editor->initialize(engine);
//    editor->run();
//
//    editor->clear();
//    engine->shutdownEngine();

    EAT::Engine* engine = new EAT::Engine();
    engine->startEngine();

    return 0;
} 