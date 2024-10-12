#include "Game.h"

#include "../config.h"

#include <iostream>

Game::Game( SDL_Window *window, SDL_Renderer *renderer )
    : mWindow( window ), mRenderer( renderer ), mQuit( false ),
      mPlayer( 25, 5, mPlatforms, mSolidPlatforms, mWalls )
{
  std::cout << "Game constructor called" << std::endl;

/* if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0)
  {
    std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
  }
}

void Game::loadMusic(const char* path) {
  mMusic = Mix_LoadMUS( path );
  if (mMusic == nullptr) {
    std::cerr << "Failed to load music: " << Mix_GetError() << std::endl;
  }
}

void Game::playMusic() {
  Mix_PlayMusic( mMusic, -1 );
}*/
}

void Game::handleEvents()
{
  std::cout << "Handling events" << std::endl;
  SDL_Event e;
  while( SDL_PollEvent( &e ) != 0 )
  {
    std::cout << "Event polled: " << e.type << std::endl;
    if( e.type == SDL_QUIT )
    {
      std::cout << "SDL_QUIT event received" << std::endl;
      mQuit = true;   
    }
    mPlayer.handleEvent( e );
    std::cout << "Adding platforms" << std::endl;
    //mPlatforms.push_back( Platform( 10, SCREEN_HEIGHT - 150, 200, 20 ) );
    mSolidPlatforms.push_back(SolidPlatform( 10, SCREEN_HEIGHT - 150, 570, 20 ) );
    mSolidPlatforms.push_back(SolidPlatform( 800, SCREEN_HEIGHT - 150, 570, 20 ) );
    //mPlatforms.push_back( Platform( 395, SCREEN_HEIGHT - 270, 50, 20 ) );
    mPlatforms.push_back( Platform( 500, SCREEN_HEIGHT - 340, 50, 20 ) );
    mPlatforms.push_back( Platform( 500, SCREEN_HEIGHT - 540, 50, 20 ) );
    mSolidPlatforms.push_back(SolidPlatform( 570, SCREEN_HEIGHT - 540, 220, 20 ) );
    mSolidPlatforms.push_back(SolidPlatform( 1200, SCREEN_HEIGHT - 540, 220, 20 ) );
   // mPlatforms.push_back( Platform( 395, SCREEN_HEIGHT - 240, 50, 20 ) );
   // mPlatforms.push_back( Platform( 395, SCREEN_HEIGHT - 270, 50, 20 ) );
   // mWalls.push_back( Wall( 200, SCREEN_HEIGHT - 250, 20, 100 ) );
    //mSolidPlatforms.push_back(SolidPlatform( 200, SCREEN_HEIGHT - 260, 20, 10 ) );

    mWalls.push_back( Wall( 0, 0, 20, 800 ) );
    mWalls.push_back( Wall( 1260, 0, 20, 800 ) );
  }
}

void Game::update()
{
  std::cout << "Updating game state" << std::endl;
  mPlayer.move();
}

void Game::render()
{
  std::cout << "Rendering game" << std::endl;
  SDL_SetRenderDrawColor( mRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
  SDL_RenderClear( mRenderer );

  std::cout << "Rendering platforms" << std::endl;
  for( auto &platform : mPlatforms )
    platform.render( mRenderer );

  std::cout << "Rendering solid platforms" << std::endl;
  for( auto &solidPlatform : mSolidPlatforms )
    solidPlatform.render( mRenderer );

  std::cout << "Rendering walls" << std::endl;
  for( auto &wall : mWalls )
    wall.render( mRenderer );

  std::cout << "Rendering player" << std::endl;
  mPlayer.render( mRenderer );

  SDL_RenderPresent( mRenderer );
  std::cout << "Render complete" << std::endl;
}

bool Game::isQuit() const
{
  return mQuit;
}
