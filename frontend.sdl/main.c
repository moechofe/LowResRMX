//
// Copyright 2017-2020 Timo Kloss
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//

#include "config.h"

#include "main.h"
#include "core.h"
#include "runner.h"
#include "dev_menu.h"
#include "settings.h"
#include "system_paths.h"
#include "utils.h"
#include "boot_intro.h"
#include "sdl_include.h"

#if SCREENSHOTS
#include "screenshot.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <math.h>
#include <string.h>

const char *defaultDisk = "Disk.rmx";
// XXX: Change the default scale on linux
// const int defaultWindowScale = 4;
const int defaultWindowScale = 1;
const int joyAxisThreshold = 16384;

const int keyboardControls[2][2][8] = {
		// mapping 0
		{
				// up, down, left, right, button A, button B, alt. button A, alt. button B
				{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
				 SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_N, SDL_SCANCODE_M},
				{SDL_SCANCODE_E, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_F,
				 SDL_SCANCODE_TAB, SDL_SCANCODE_Q, SDL_SCANCODE_LSHIFT, SDL_SCANCODE_A}},
		// mapping 1
		{
				// up, down, left, right, button A, button B, alt. button A, alt. button B
				{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
				 SDL_SCANCODE_J, SDL_SCANCODE_K, SDL_SCANCODE_I, SDL_SCANCODE_U},
				{SDL_SCANCODE_UP, SDL_SCANCODE_DOWN, SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT,
				 SDL_SCANCODE_H, SDL_SCANCODE_L, SDL_SCANCODE_O, SDL_SCANCODE_Y}}};

void update(void *arg);
void updateScreenRect(int winW, int winH);
void configureJoysticks(void);
void closeJoysticks(void);
void setTouchPosition(int windowX, int windowY);
void toggleZoom(void);
void changeVolume(int delta);
void audioCallback(void *userdata, Uint8 *stream, int len);
void saveScreenshot(void *pixels, int scale);

#ifdef __EMSCRIPTEN__
void onloaded(const char *filename);
void onerror(const char *filename);
#endif

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_AudioDeviceID audioDevice = 0;
SDL_AudioSpec audioSpec;

struct Runner runner;
#if DEV_MENU
struct DevMenu devMenu;
#endif
struct Settings settings;
struct CoreInput coreInput;

enum MainState mainState = MainStateUndefined;
char mainProgramFilename[FILENAME_MAX] = "";

int numJoysticks = 0;
SDL_Joystick *joysticks[2] = {NULL, NULL};
SDL_Rect screenRect;
bool quit = false;
bool releasedTouch = false;
bool audioStarted = false;
bool mouseEnabled = false;
int messageNumber = 0;
bool hasUsedInputLastUpdate = false;
int screenshotRequestedWithScale = 0;
int volume = 0; // 0 = max, it's a bit shift

int main(int argc, const char *argv[])
{
	memset(&coreInput, 0, sizeof(struct CoreInput));

	coreInput.width = 216;	//-32;
	coreInput.height = 384; //-16;
	// TEMP:
	// coreInput.safe.left=1;
	// coreInput.safe.top=1;
	// coreInput.safe.right=1;
	// coreInput.safe.bottom=1;

	settings_init(&settings, mainProgramFilename, argc, argv);
	runner_init(&runner);
#if DEV_MENU
	dev_init(&devMenu, &runner, &settings);
#endif

	if (runner_isOkay(&runner))
	{
		SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);

		SDL_EventState(SDL_DROPFILE, SDL_ENABLE);
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_DROPFILE:
			{
				strncpy(mainProgramFilename, event.drop.file, FILENAME_MAX - 1);
				SDL_free(event.drop.file);
				break;
			}
			}
		}

		Uint32 windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
		if (settings.session.fullscreen)
		{
			windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		const char *windowTitle = "LowResRMX";

		window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * defaultWindowScale, SCREEN_HEIGHT * defaultWindowScale, windowFlags);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

		SDL_AudioSpec desiredAudioSpec = {
				.freq = 44100,
				.format = AUDIO_S16,
				.channels = NUM_CHANNELS,
#ifdef __EMSCRIPTEN__
				.samples = 2048, // sample FRAMES
#else
				.samples = 1470, // sample FRAMES
#endif
				.userdata = runner.core,
				.callback = audioCallback};

		audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredAudioSpec, &audioSpec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

		configureJoysticks();

		bootNX();
		if (hasProgram())
		{
			machine_poke(runner.core, bootIntroStateAddress, BootIntroStateProgramAvailable);
		}

		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		updateScreenRect(width, height);

#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop_arg(update, NULL, -1, true);
#else
		while (!quit)
		{
			Uint32 ticks = SDL_GetTicks();

			update(NULL);

			if (!settings.session.disabledelay || runner.core->machineInternals->isEnergySaving)
			{
				// limit to 60 FPS
				Uint32 ticksDelta = SDL_GetTicks() - ticks;
				if (ticksDelta < 16)
				{
					SDL_Delay(16 - ticksDelta);
				}
			}
		}

		core_willSuspendProgram(runner.core);
#endif
	}

	closeJoysticks();

	SDL_CloseAudioDevice(audioDevice);

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();

	runner_deinit(&runner);

	return 0;
}

void bootNX()
{
	mainState = MainStateBootIntro;

	struct CoreError error = core_compileProgram(runner.core, bootIntroSourceCode, true);
	if (error.code != ErrorNone)
	{
		core_traceError(runner.core, error);
	}

	runner.core->interpreter->debug = false;
	core_willRunProgram(runner.core, SDL_GetTicks() / 1000);
}

void rebootNX()
{
	core_willSuspendProgram(runner.core);

	mainProgramFilename[0] = 0;
	bootNX();
}

bool hasProgram()
{
	return mainProgramFilename[0] != 0;
}

const char *getMainProgramFilename()
{
	return mainProgramFilename;
}

void selectProgram(const char *filename)
{
	strncpy(mainProgramFilename, filename, FILENAME_MAX - 1);
	if (mainState == MainStateBootIntro)
	{
		machine_poke(runner.core, bootIntroStateAddress, BootIntroStateProgramAvailable);
	}
	else
	{
		runMainProgram();
	}
}

void runMainProgram()
{
	core_willSuspendProgram(runner.core);

	struct CoreError error = runner_loadProgram(&runner, mainProgramFilename);
#if DEV_MENU
	devMenu.lastError = error;
#endif
	if (error.code != ErrorNone)
	{
#if DEV_MENU
		core_setDebug(runner.core, true);
		showDevMenu();
#else
		core_traceError(runner.core, error);
#endif
	}
	else
	{
		core_willRunProgram(runner.core, SDL_GetTicks() / 1000);
		mainState = MainStateRunningProgram;
	}
}

void runToolProgram(const char *filename)
{
	core_willSuspendProgram(runner.core);

	struct CoreError error = runner_loadProgram(&runner, filename);
	if (error.code == ErrorNone)
	{
		mainState = MainStateRunningTool;
		runner.core->interpreter->debug = false;
		core_willRunProgram(runner.core, SDL_GetTicks() / 1000);
	}
	else
	{
		core_traceError(runner.core, error);
	}
}

void showDevMenu()
{
#if DEV_MENU
	core_willSuspendProgram(runner.core);

	bool reload = (mainState == MainStateRunningTool);
	mainState = MainStateDevMenu;
	dev_show(&devMenu, reload);
#endif
}

bool usesMainProgramAsDisk()
{
	return (mainState == MainStateRunningTool);
}

void getDiskFilename(char *outputString)
{
	if (usesMainProgramAsDisk())
	{
		strncpy(outputString, mainProgramFilename, FILENAME_MAX - 1);
	}
	else
	{
		strncpy(outputString, mainProgramFilename, FILENAME_MAX - 1);
		char *separator = strrchr(outputString, PATH_SEPARATOR_CHAR);
		if (separator)
		{
			separator++;
			*separator = 0;
			strncat(outputString, defaultDisk, FILENAME_MAX - 1);
		}
		else
		{
			strncpy(outputString, defaultDisk, FILENAME_MAX - 1);
		}
	}
}

void getRamFilename(char *outputString)
{
	char *prefPath = SDL_GetPrefPath("martin_mauchauffee", "LowResRMX");
	if (prefPath)
	{
		strncpy(outputString, prefPath, FILENAME_MAX - 1);

		char *separator = strrchr(mainProgramFilename, PATH_SEPARATOR_CHAR);
		if (separator)
		{
			separator++;
			strncat(outputString, separator, FILENAME_MAX - 1);
		}
		else
		{
			strncat(outputString, mainProgramFilename, FILENAME_MAX - 1);
		}

		char *postfix = strrchr(outputString, '.');
		if (postfix)
		{
			*postfix = 0;
		}
		strncat(outputString, ".dat", FILENAME_MAX - 1);
	}
	else
	{
		outputString[0] = 0;
	}
}

void updateMouseMode()
{
	if (!mouseEnabled && (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP))
	{
		SDL_ShowCursor(SDL_DISABLE);
	}
	else
	{
		SDL_ShowCursor(SDL_ENABLE);
	}
}

void setMouseEnabled(bool enabled)
{
	mouseEnabled = enabled;
	updateMouseMode();
}

void update(void *arg)
{
	SDL_Event event;
	bool hasInput = false;
	bool forceRender = false;

	if (releasedTouch)
	{
		coreInput.touch = false;
		releasedTouch = false;
	}

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
			{
				updateScreenRect(event.window.data1, event.window.data2);
				forceRender = true;
				break;
			}
			}
			break;

		case SDL_DROPFILE:
		{
			if (hasPostfix(event.drop.file, ".rmx") || hasPostfix(event.drop.file, ".RMX"))
			{
#if DEV_MENU
				bool handled = (mainState == MainStateDevMenu && dev_handleDropFile(&devMenu, event.drop.file));
				if (!handled)
				{
					selectProgram(event.drop.file);
				}
#else
				selectProgram(event.drop.file);
#endif
				forceRender = true;
			}
			else
			{
				overlay_message(runner.core, "NOT NX FORMAT");
			}
			SDL_free(event.drop.file);
			break;
		}

		case SDL_KEYDOWN:
		{
			SDL_Keycode keycode = event.key.keysym.sym;
			SDL_Scancode scancode = event.key.keysym.scancode;

			if (event.key.keysym.mod == 0)
			{
				hasInput = true;
			}

			// text input
			if (keycode == SDLK_RETURN)
			{
				coreInput.key = CoreInputKeyReturn;
			}
			else if (keycode == SDLK_BACKSPACE)
			{
				coreInput.key = CoreInputKeyBackspace;
			}
			else if (scancode == SDL_SCANCODE_UP)
			{
				coreInput.key = CoreInputKeyUp;
			}
			else if (scancode == SDL_SCANCODE_DOWN)
			{
				coreInput.key = CoreInputKeyDown;
			}
			else if (scancode == SDL_SCANCODE_LEFT)
			{
				coreInput.key = CoreInputKeyLeft;
			}
			else if (scancode == SDL_SCANCODE_RIGHT)
			{
				coreInput.key = CoreInputKeyRight;
			}
			else if (scancode == SDL_SCANCODE_DELETE)
			{
				coreInput.key = CoreInputKeyDelete;
			}
			else if (keycode >= SDLK_0 && keycode <= SDLK_9)
			{
				coreInput.key = keycode;
			}

#if HOT_KEYS
			// system
			if (event.key.keysym.mod & KMOD_CTRL)
			{
				if (keycode == SDLK_d)
				{
					core_setDebug(runner.core, !core_getDebug(runner.core));
					if (core_getDebug(runner.core))
					{
						overlay_message(runner.core, "DEBUG ON");
					}
					else
					{
						overlay_message(runner.core, "DEBUG OFF");
					}
				}
				else if (keycode == SDLK_f)
				{
					if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN_DESKTOP)
					{
						SDL_SetWindowFullscreen(window, 0);
					}
					else
					{
						SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
					}
					updateMouseMode();
					forceRender = true;
				}
				else if (keycode == SDLK_r)
				{
					if (hasProgram())
					{
						runMainProgram();
						overlay_message(runner.core, "RELOADED");
					}
				}
				else if (keycode == SDLK_e)
				{
					rebootNX();
				}
				else if (keycode == SDLK_s)
				{
					screenshotRequestedWithScale = (event.key.keysym.mod & KMOD_SHIFT) ? 1 : 4;
					forceRender = true;
				}
				else if (keycode == SDLK_z)
				{
					toggleZoom();
					forceRender = true;
				}
				else if (keycode == SDLK_PLUS)
				{
					changeVolume(-1);
				}
				else if (keycode == SDLK_MINUS)
				{
					changeVolume(+1);
				}
			}
			else if (keycode == SDLK_ESCAPE)
			{
				// XXX: Always quit when press ESCAPE
				// quit = true;
				if (settings.session.disabledev)
				{
					quit = true;
				}
#if DEV_MENU
				else if (hasProgram())
				{
					if (mainState != MainStateDevMenu)
					{
						showDevMenu();
					}
				}
#endif
			}
			else if (settings.session.mapping == 1 && !core_isKeyboardEnabled(runner.core))
			{
				if (keycode == SDLK_SPACE)
				{
					toggleZoom();
					forceRender = true;
					hasInput = false;
				}
				else if (scancode == SDL_SCANCODE_KP_PLUS)
				{
					changeVolume(-1);
				}
				else if (scancode == SDL_SCANCODE_KP_MINUS)
				{
					changeVolume(+1);
				}
			}
#endif
			break;
		}

		case SDL_TEXTINPUT:
		{
			char key = event.text.text[0];
			hasInput = true;
			if (key >= ' ' && key <= '_')
			{
				coreInput.key = key;
			}
			else if (key >= 'a' && key <= 'z')
			{
				coreInput.key = key - 32;
			}
			break;
		}

		case SDL_MOUSEBUTTONDOWN:
		{
			hasInput = true;
			setTouchPosition(event.button.x, event.button.y);
			coreInput.touch = true;
			break;
		}

		case SDL_MOUSEBUTTONUP:
		{
			releasedTouch = true;
			break;
		}

		case SDL_MOUSEMOTION:
		{
			setTouchPosition(event.motion.x, event.motion.y);
			break;
		}

		case SDL_JOYDEVICEADDED:
		case SDL_JOYDEVICEREMOVED:
		{
			configureJoysticks();
			break;
		}

		case SDL_JOYBUTTONDOWN:
		{
			hasInput = true;
			if (event.jbutton.button == 2)
			{
				coreInput.pause = true;
			}
			break;
		}
		}
	}

	const Uint8 *state = SDL_GetKeyboardState(NULL);

	switch (mainState)
	{
	case MainStateUndefined:
		break;

	case MainStateBootIntro:
		if (hasInput && !hasProgram())
		{
			// user hint
			overlay_message(runner.core, "DRAG .RMX INTO WINDOW");
		}
		core_update(runner.core, &coreInput);
		if (machine_peek(runner.core, bootIntroStateAddress) == BootIntroStateReadyToRun)
		{
			machine_poke(runner.core, bootIntroStateAddress, BootIntroStateDone);
#ifdef __EMSCRIPTEN__
			emscripten_async_wget(mainProgramFilename, mainProgramFilename, onloaded, onerror);
#else
			runMainProgram();
#endif
		}
		break;

	case MainStateRunningProgram:
	case MainStateRunningTool:
		core_update(runner.core, &coreInput);
		if (hasInput)
		{
			if (runner.core->interpreter->state == StateEnd)
			{
				overlay_message(runner.core, "END OF PROGRAM");
			}
			else if (!coreInput.out_hasUsedInput && !hasUsedInputLastUpdate)
			{
				// user hints for controls
				// union IOAttributes attr = runner.core->machine->ioRegisters.attr;
				if (runner.core->machine->ioRegisters.status.keyboardVisible)
				// if (attr.keyboardEnabled)
				{
					overlay_message(runner.core, "KEYBOARD");
				}
			}
		}
		break;

	case MainStateDevMenu:
#if DEV_MENU
		dev_update(&devMenu, &coreInput);
#endif
		break;
	}

	hasUsedInputLastUpdate = coreInput.out_hasUsedInput;

	if (!audioStarted && audioDevice)
	{
		audioStarted = true;
		SDL_PauseAudioDevice(audioDevice, 0);
	}

	if (core_shouldRender(runner.core) || forceRender)
	{
		SDL_RenderClear(renderer);

		void *pixels = NULL;
		int pitch = 0;
		SDL_LockTexture(texture, NULL, &pixels, &pitch);

		video_renderScreen(runner.core, pixels);

		if (screenshotRequestedWithScale > 0)
		{
			saveScreenshot(pixels, screenshotRequestedWithScale);
			screenshotRequestedWithScale = 0;
		}

		SDL_UnlockTexture(texture);
		SDL_RenderCopy(renderer, texture, NULL, &screenRect);

		SDL_RenderPresent(renderer);
	}
}

void updateScreenRect(int winW, int winH)
{
	switch (settings.session.zoom)
	{
	case ZoomPixelPerfect:
	{
		int factor = fmax(1, fmin(winW / SCREEN_WIDTH, winH / SCREEN_HEIGHT));

		int nxScreenW = SCREEN_WIDTH * factor;
		int nxScreenH = SCREEN_HEIGHT * factor;

		screenRect.w = nxScreenW;
		screenRect.h = nxScreenH;
		screenRect.x = (winW - nxScreenW) / 2;
		screenRect.y = (winH - nxScreenH) / 2;
		break;
	}
	case ZoomLarge:
	{
		float factor = fmax(1, fmin(winW / (float)SCREEN_WIDTH, winH / (float)SCREEN_HEIGHT));

		int nxScreenW = SCREEN_WIDTH * factor;
		int nxScreenH = SCREEN_HEIGHT * factor;

		screenRect.w = nxScreenW;
		screenRect.h = nxScreenH;
		screenRect.x = (winW - nxScreenW) / 2;
		screenRect.y = (winH - nxScreenH) / 2;
		break;
	}
	case ZoomOverscan:
	{
		float factor = fmax(winW / (float)SCREEN_WIDTH, winH / (float)SCREEN_HEIGHT);

		int nxScreenW = SCREEN_WIDTH * factor;
		int nxScreenH = SCREEN_HEIGHT * factor;

		screenRect.w = nxScreenW;
		screenRect.h = nxScreenH;
		screenRect.x = (winW - nxScreenW) / 2;
		screenRect.y = (winH - nxScreenH) / 2;
		break;
	}
	case ZoomSqueeze:
		screenRect.w = winW;
		screenRect.h = winH;
		screenRect.x = 0;
		screenRect.y = 0;
		break;
	}

	SDL_SetTextInputRect(&screenRect);
}

void configureJoysticks()
{
	closeJoysticks();
	numJoysticks = SDL_NumJoysticks();
	if (numJoysticks > 2)
	{
		numJoysticks = 2;
	}
	for (int i = 0; i < numJoysticks; i++)
	{
		joysticks[i] = SDL_JoystickOpen(i);
	}
}

void closeJoysticks()
{
	for (int i = 0; i < numJoysticks; i++)
	{
		SDL_JoystickClose(joysticks[i]);
		joysticks[i] = NULL;
	}
	numJoysticks = 0;
}

void setTouchPosition(int windowX, int windowY)
{
	coreInput.touchX = (float)(windowX - screenRect.x) * SCREEN_WIDTH / screenRect.w;
	coreInput.touchY = (float)(windowY - screenRect.y) * SCREEN_HEIGHT / screenRect.h;
}

void toggleZoom()
{
#ifdef __EMSCRIPTEN__
	settings.session.zoom = 1;
#else
	settings.session.zoom = (settings.session.zoom + 1) % 4;
#endif
	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	updateScreenRect(width, height);
}

void changeVolume(int delta)
{
	volume += delta;
	if (volume < 0)
		volume = 0;
	if (volume > 6)
		volume = 6;
	char message[16];
	sprintf(message, "VOLUME %d%%", 100 >> volume);
	overlay_message(runner.core, message);
}

void audioCallback(void *userdata, Uint8 *stream, int len)
{
	int16_t *samples = (int16_t *)stream;
	int numSamples = len / NUM_CHANNELS;
	audio_renderAudio(userdata, samples, numSamples, audioSpec.freq, volume);
}

void saveScreenshot(void *pixels, int scale)
{
#if SCREENSHOTS
	bool succeeded = screenshot_save(pixels, scale);
	if (succeeded)
	{
		overlay_message(runner.core, "SCREENSHOT SAVED");
	}
	else
	{
		overlay_message(runner.core, "SCREENSHOT ERROR");
	}
#endif
}

#ifdef __EMSCRIPTEN__

EM_JS(void, ready, (), {
	document.querySelector('body').classList.add("app-ready");
});

void onloaded(const char *filename)
{
	ready();
	// int width, height;
	// SDL_GetWindowSize(window, &width, &height);
	// updateScreenRect(width, height);
	runMainProgram();
}

void onerror(const char *filename)
{
}

#endif
