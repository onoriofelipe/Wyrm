#include <sstream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SFML/Window.hpp>
#include "game.h"
#include "backdrop.h"
#include "events.h"

Game* Game::instance;

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

Game::Game() {
  // Create the main rendering window
  App = new sf::RenderWindow(sf::VideoMode(DEFAULT_WIDTH, DEFAULT_HEIGHT, 32),
                             "WYRM",
                             sf::Style::Titlebar | sf::Style::Close,
                             sf::ContextSettings(24,    // Depth buffer
                                                  8,    // Stencil buffer
                                                  8) ); // AA level

  srand( time(NULL) );

  // Set color and depth clear value
  glClearDepth(1.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Enable Z-buffer read and write
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);

  // Setup a perspective projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(90.f, 1.f, 1.f, 500.f);

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);

  glMatrixMode(GL_MODELVIEW);

  LoadKeymap();
  App->EnableKeyRepeat(true);

  gui = new GUI(*App);

  resourcemanager = new ResourceManager;

  objectmanager = new ObjectManager;

  networkhandler = new NetworkHandler;

  bRunning = true;

  player = 0;

  backdrop = 0;
}

Game::~Game() {
  bRunning = false;

  if(player) {
    delete player;
  }

  if( backdrop ) {
    delete backdrop;
  }

  if( networkhandler ) {
    delete networkhandler;
  }

  if( objectmanager ) {
    delete objectmanager;
  }

  if( resourcemanager ) {
    delete resourcemanager;
  }

  if( gui ) {
    delete gui;
  }

  if(App) {
    delete App;
  }
}

Game* Game::GetGame() {
  if( !instance ) {
    instance = new Game();
  }
  return instance;
}

void Game::Run() {
  static unsigned int counter = 0;
  static sf::Clock fps_timer;
  while ( bRunning && App->IsOpened() ) {
    float ElapsedTime = (float)(App->GetFrameTime()) / 1000.0f;
    Tick(ElapsedTime);

    counter++;

    if( fps_timer.GetElapsedTime() > 1000 ) {
      fps_timer.Reset();
      std::stringstream ss;
      ss << "WYRM - " << counter << " FPS";
      counter = 0;
      App->SetTitle( ss.str() );
    }

    App->SetFramerateLimit(200);
  }
}

void Game::Stop() {
  bRunning = false;
}

GUI* Game::GetGUI() {
  if( gui ) {
    return gui;
  } else {
    return NULL;
  }
}

Player* Game::GetPlayer() {
  if( player ) {
    return player;
  } else {
    return NULL;
  }
}

Player* Game::CreatePlayer(sf::Uint16 id_, std::string name_) {
  if( player ) {
    return player;
  } else {
    player = new Player(id_,name_);
    return player;
  }
}

NetworkHandler* Game::GetNetworkHandler() {
  if( networkhandler ) {
    return networkhandler;
  } else {
    return NULL;
  }
}

ObjectManager* Game::GetObjectManager() {
  if( objectmanager ) {
    return objectmanager;
  } else {
    return NULL;
  }
}

ResourceManager* Game::GetResourceManager() {
  if( resourcemanager ) {
    return resourcemanager;
  } else {
    return NULL;
  }
}

Backdrop* Game::GetBackdrop() {
  if( backdrop ) {
    return backdrop;
  } else {
    return NULL;
  }
}

void Game::Resize( float width, float height ) {
  sf::View view( sf::FloatRect(0, 0, width, height) );
  App->SetView( view );
}

sf::Vector2f Game::GetDefaultResolution() {
  return sf::Vector2f( DEFAULT_WIDTH, DEFAULT_HEIGHT );
}

sf::Vector2f Game::GetCurrentResolution() {
  sf::View view = App->GetView();
  return view.GetSize();
}

void Game::Tick(float t) {

  if( HandleEvents(*App) ) {
    bRunning = false;
  }

  networkhandler->Tick();

  // Clear the screen (fill it with black color)
  App->Clear(sf::Color(0, 0, 0));

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if( networkhandler->IsAuthenticated() ) {
    if( !backdrop ) {
      backdrop = new Backdrop(*App);
    }

    objectmanager->Tick(t);
    if( player ) {
      player->Tick(t);
    }
    //partSys.Update(t);

    backdrop->Draw(*App);

    objectmanager->DrawAll(*App);
    //App.Draw(partSys);
  }

  gui->Draw(*App);

  //sf::Shape rect = sf::Shape::Rectangle(10,10,100,100, sf::Color(255,255,255));

  //App->Draw(rect);

  // Display window contents on screen
  App->Display();
}
