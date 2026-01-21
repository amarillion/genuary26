#pragma once

#include <allegro5/allegro.h>
#include "audio.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "point.h"
#include "graphicscontext.h"
#include "icomponent.h"

#define TWIST_START_EVENT 1024    /* value for event.type - originally in widget.h */

#ifdef USE_MONITORING
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
#endif

#define TICKS_FROM_MSEC(x) ((x) / MainLoop::getMainLoop()->getLogicIntervalMsec())
#define MSEC_FROM_TICKS(x) ((x) * MainLoop::getMainLoop()->getLogicIntervalMsec())

class MainLoop final
{
private:
	ALLEGRO_BITMAP *buffer;
	ALLEGRO_EVENT_QUEUE *equeue;
	ALLEGRO_TIMER *logicTimer;
	ALLEGRO_DISPLAY *display;

	std::unique_ptr<Audio> _audio = nullptr;
	
	ALLEGRO_PATH *localAppData;
	ALLEGRO_PATH *configPath;

	const char *configFilename;
	const char *title;
	const char *appname;
	
	int frame_count;
	int frame_counter;
	int last_fps;
	int lastUpdateMsec;

	enum ScreenMode { WINDOWED = 0, FULLSCREEN_WINDOW, FULLSCREEN };
	ScreenMode screenMode;

	/** Default display size, in situations where there is a choice */
	Point prefDisplaySize;

	// in smoke test mode: don't create display, just test loading resources.
	// smokeTest is in headless mode.
	bool smokeTest;
	
	void getFromConfig(ALLEGRO_CONFIG *config);
	void getFromArgs(int argc, const char *const *argv);

	int logicIntervalMsec;

	bool isResizableWindow = false;

	static MainLoop *instance;
	int initDisplay();

	std::vector<std::string> options;
	
	/** Only valid during call to run() */
	IComponent *app = nullptr;
#ifdef USE_MONITORING
	Clock::time_point t0; // time since start of program
	Clock::time_point t1; // time since start of update loop
	Clock::time_point t2; // time since start of phase
	void logStartTime(const std::string &phase);
	void logEndTime(const std::string &phase);
	std::map<std::string, long> sums;
	std::map<std::string, int> counts;
#endif
protected:
	ALLEGRO_CONFIG *config;
	bool fpsOn;

	void pumpMessages(IComponent *app);
public:
	bool isSmokeTest() { return smokeTest; }

	// randomly generated id used to identify recurring user
	std::string getUserId();

	/** return vector of unhandled command-line arguments */
	std::vector<std::string> &getOpts() { return options; }

	ALLEGRO_CONFIG *getConfig() { return config; }
	
	int getMsecCounter () { return al_get_timer_count(logicTimer) * logicIntervalMsec; }
	void setFpsOn (bool value) { fpsOn = value; }

	MainLoop();

	MainLoop &setTitle(const char *_title);
	MainLoop &setAppName(const char *_appname);
	MainLoop &setConfigFilename(const char *_configFilename);
	MainLoop &setLogicIntervalMsec (int value) { logicIntervalMsec = value; return *this; }
	
	/** Try to make a display of this size (might not be respected e.g. in full screen mode) */
	MainLoop &setPreferredDisplaySize (int w, int h) { prefDisplaySize = Point(w, h); return *this; }

	MainLoop &setResizableWindow (bool value) { isResizableWindow = value; return *this; }

	MainLoop &setAudioModule(std::unique_ptr<Audio> val) {
		_audio = move(val);
		return *this;
	}

	/**
	 * returns 0 on success, 1 on failure
	 */
	int init(int argc, const char *const *argv);
	void run(IComponent *app);
	virtual ~MainLoop();
	
	virtual void parseOpts(std::vector<std::string> &opts) {};
	int getLogicIntervalMsec () { return logicIntervalMsec; }

	void toggleWindowed();
	bool isWindowed();

	Audio *audio() { return _audio.get(); }
	static MainLoop *getMainLoop();
};
