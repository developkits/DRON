/**
 *  GameState/Menu.hpp
 *  (c) Jonathan Capps
 *  Created 08 Sept. 2011
 */

#ifndef MENU_HPP
#define MENU_HPP

#include "GameState.hpp"
#include <deque>

typedef unsigned int Entity;
class EntitySystem;
class D3D11Renderer;
class Menu : public GameState
{
	public:
		Menu( EntitySystem& es, D3D11Renderer& r );
		virtual ~Menu() { }

		virtual void Update( float dt );
		virtual void HandleKeypress( const WPARAM key );

	private:
		void ProcessInput();

		EntitySystem&	_entity_system;
		D3D11Renderer&	_renderer;
		Entity			_camera;
		Entity			_test_entity;

		typedef std::deque< ACTION > ActionDeque;
		ActionDeque	_actions;
};

#endif  MENU_HPP
