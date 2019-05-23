#define _USE_MATH_DEFINES


#include <iostream>
#include <fstream>
#include <string>

#include <Windows.h>


#include <SDL.h>
#include <SDL_ttf.h>

#include "winUtils.h"
#define _WIN32_WINNT 0x050

const SDL_Color BACKGROUND_DEFAULT_COLOR = { 12, 10, 12, 255 };
const SDL_Color BACKGROUND_HIGHLIGHT_COLOR = { 245, 245, 250, 255 };
const SDL_Color TEXT_DEFAULT_COLOR = { 245, 245, 250, 255 };
const SDL_Color TEXT_HIGHLIGHT_COLOR = { 12, 10, 12, 255 };
const SDL_Color TEXT_SOLUTION_COLOR = { 100, 160, 255, 255 };






bool init(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font);

void close(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);


void loop(SDL_Window * window, SDL_Renderer *renderer, TTF_Font* font);


std::pair<bool, std::pair<int, int>> drawTextAt(SDL_Renderer* renderer, TTF_Font* font,
	const  std::string& text, const int& x, const int& y, const SDL_Color& color);

void draw(SDL_Renderer *renderer, TTF_Font* font, int passwordLength);

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);




int main(int argc, char* argv[]) {
	HHOOK hhkLowLevelKybd = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, 0, 0);
	int ret = 0;
	std::ofstream log_file("log.txt");
	std::cerr.rdbuf(log_file.rdbuf());

	SDL_Window * window = nullptr;
	SDL_Renderer * renderer = nullptr;
	TTF_Font * font = nullptr;

	std::cerr << "Initializing..." << std::endl;
	if (init(&window, &renderer, &font)) {
		std::cerr << "Loop..." << std::endl;
		loop(window, renderer, font);

		std::cerr << "Closing..." << std::endl;
		close(window, renderer, font);
	}
	else
		ret = 1;
	

	log_file.close();
	UnhookWindowsHookEx(hhkLowLevelKybd);
	return ret;
}


bool init(SDL_Window** window, SDL_Renderer** renderer, TTF_Font** font) {
	
	std::cerr << "Init video system..." << std::endl;
	//Init video system
	if (SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "Error in init <init video>: " << SDL_GetError() << std::endl;
		return false;
	}

	std::cerr << "Init font system..." << std::endl;
	if (TTF_Init() == -1) {
		std::cerr << "Error in init <init TTF>: " << TTF_GetError() << std::endl;
		SDL_Quit();
		return false;
	}
	
	std::cerr << "Init window..." << std::endl;

	//Get desktop size
	SDL_DisplayMode dm;
	int error = 0;
	int width = 0;
	int height = 0;
	int i = 0;
	while (error >= 0)
	{
		error = SDL_GetDesktopDisplayMode(i, &dm);
		width += dm.w;
		height += dm.h;
		i++;
	}
	
	

	//Create window
	*window = SDL_CreateWindow("NoEscape", 0, 0,
		width, height, SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP);
	if (*window == nullptr) {
		std::cerr << "Error in init <create window>: " << SDL_GetError() << std::endl;
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	std::cerr << "Init renderer..." << std::endl;
	//Create renderer
	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
	if (*renderer == nullptr) {
		std::cerr << "Error in init <create renderer>: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(*window);
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	
	std::cerr << "Opening font..." << std::endl;
	*font = TTF_OpenFont("Inconsolata.ttf", 28);
	if (*font == NULL)
	{
		std::cerr << "Error in init <open font>: " << TTF_GetError() << std::endl;
		SDL_DestroyRenderer(*renderer);
		SDL_DestroyWindow(*window);
		TTF_Quit();
		SDL_Quit();
		return false;
	}

	
	return true;
}

void close(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {

	TTF_CloseFont(font);

	SDL_DestroyRenderer(renderer);

	SDL_DestroyWindow(window);

	TTF_Quit();
	SDL_Quit();
}



void loop(SDL_Window * window, SDL_Renderer *renderer, TTF_Font* font) {
	bool quit = false;
	std::string password = "";
	std::string currentInput = "";
	std::string storedPassword = "";
	std::ifstream passwordFile("password.dat");
	if (passwordFile.is_open()) {
		std::getline(passwordFile, storedPassword);
		passwordFile.close();
	}
	else {
		storedPassword = "sorry";
		std::ofstream oPasswordFile("password.dat");
		oPasswordFile << storedPassword;
		oPasswordFile.close();
	}
	SDL_StartTextInput();

	SDL_Event e;
	std::cerr << "First draw..." << std::endl;
	draw(renderer, font, currentInput.length());
	std::cerr << "Start of loop..." << std::endl;
	//Loop
	while (!quit) {
		//Event processing
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				//quit = true; //no quitting;
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
				SDL_StopTextInput();
				password = currentInput;
				currentInput = "";
				if (password == storedPassword) {
					quit = true;
				}
				SDL_StartTextInput();
			}
			else if (e.type == SDL_TEXTINPUT) {
				currentInput.append(e.text.text);
			}
			else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_BACKSPACE) {
				if (currentInput.length() > 0)
					currentInput.erase(currentInput.length()-1, 1);
			}
		}
		draw(renderer, font, currentInput.length());
		SDL_RaiseWindow(window);
		SDL_SetWindowInputFocus(window);
	}
	std::cerr << "End loop..." << std::endl;
}





void draw(SDL_Renderer *renderer, TTF_Font* font, int passwordLength) {

	//Paint BG
	SDL_SetRenderDrawColor(renderer, BACKGROUND_DEFAULT_COLOR.r, 
				BACKGROUND_DEFAULT_COLOR.g, BACKGROUND_DEFAULT_COLOR.b, 
				BACKGROUND_DEFAULT_COLOR.a);
	SDL_RenderClear(renderer);
	
	//Paint BG
	SDL_SetRenderDrawColor(renderer, BACKGROUND_HIGHLIGHT_COLOR.r,
				BACKGROUND_HIGHLIGHT_COLOR.g, BACKGROUND_HIGHLIGHT_COLOR.b,
				BACKGROUND_HIGHLIGHT_COLOR.a);

	//Create password string
	std::string shownPassword = "";
	for (int i = 0; i < passwordLength; i++)
		shownPassword.append("*");
	shownPassword.append("_");

	//Paint TEXT	
	auto result = drawTextAt(renderer, font, shownPassword, 0 , 0, TEXT_DEFAULT_COLOR);
	
	//DRAW
	SDL_RenderPresent(renderer);
}

std::pair<bool, std::pair<int,int>> drawTextAt(SDL_Renderer* renderer, TTF_Font* font, 
			const  std::string& text, const int& x, const int& y, const SDL_Color& color)
{
	SDL_Surface* text_surface = TTF_RenderText_Blended(font, text.c_str(), color);
	if (text_surface == NULL) {
		std::cerr << "Error in drawTextAt <render surface>: " << TTF_GetError() << std::endl;
		return { false, {0, 0} };
	}

	SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (text_texture == NULL) {
		std::cerr << "Error in drawTextAt <render texture>: " << TTF_GetError() << std::endl;
		SDL_FreeSurface(text_surface);
		return { false, {0, 0} };
	}

	std::pair<int, int> size { text_surface->w, text_surface->h };

	SDL_Rect dst_rect{x, y, text_surface->w, text_surface->h};

	
	SDL_RenderCopy(renderer, text_texture, NULL, &dst_rect);
	SDL_FreeSurface(text_surface);
	SDL_DestroyTexture(text_texture);
	
	return { true, size };
}

DWORD forbiddenKeys[]{
	VK_LWIN,
	VK_RWIN,
	VK_LMENU,
	VK_RMENU,
	VK_TAB,
	VK_LCONTROL,
	VK_RCONTROL
};
const int NUM_FORBIDDEN_KEYS = 5;

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	bool eatKeystroke = false;

	if (nCode == HC_ACTION)
	{		
		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		for (int i = 0; i < NUM_FORBIDDEN_KEYS && !eatKeystroke; i++) {
			eatKeystroke = eatKeystroke || (p->vkCode == forbiddenKeys [i]);
		}
		
	}
	return(eatKeystroke ? 1 : CallNextHookEx(NULL, nCode, wParam, lParam));
}


