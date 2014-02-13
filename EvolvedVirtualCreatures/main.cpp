
#include "stdafx.h"

#include "foundation/PxAssert.h"
#include "RendererConfig.h"
#include "SampleCommandLine.h"
#include "RendererMemoryMacros.h"
#include "SampleAllocator.h"
#include "PhysXSampleApplication.h"
#include "PsFile.h"
#include "EvolvedVirtualCreatures.h"
extern SDbgConfig *g_pDbgConfig;

//#include "vld/vld.h"
MEMORYMONITOR_INNER_PROCESS();



using namespace SampleFramework;

static PhysXSampleApplication* gApp = NULL;
static SampleSetup gSettings;
static SampleCommandLine* gSampleCommandLine = NULL;

#define SEPARATE_EVENT_LOOP 1


void mainInitialize()
{
	PX_ASSERT(gSampleCommandLine);
	const SampleCommandLine& cmdline = *gSampleCommandLine;
	initSampleAllocator();
	gApp = SAMPLE_NEW(PhysXSampleApplication)(cmdline);

	gApp->customizeSample(gSettings);
	gSettings.mName = "Evolved Virtual Creature";

	if (gApp->isOpen())
		gApp->close();

	gApp->open(gSettings.mWidth, gSettings.mHeight, gSettings.mName, gSettings.mFullscreen);

#if SEPARATE_EVENT_LOOP
	gApp->start(Ps::Thread::getDefaultStackSize());
#else
	if(gApp->isOpen()) gApp->onOpen();
#endif

	class PxAssertHandler2 : public PxAssertHandler
	{
		virtual void operator()(const char* exp, const char* file, int line, bool& ignore) {
			assert(exp);
		}
	};
	static PxAssertHandler2 assertHandler;
	physx::PxSetAssertHandler( assertHandler );

#ifdef _DEBUG
	string configFileName = "config_evc_debug.json";
#else
	string configFileName = "config_evc_checked.json";
#endif

	if (!memmonitor::Init(memmonitor::INNER_PROCESS,(HINSTANCE)::GetModuleHandle(0), configFileName))
	{
	//	MessageBoxA(NULL, memmonitor::GetLastError().c_str(), "ERROR", MB_OK);
		//assert(0);
	}

}

void mainTerminate()
{
	memmonitor::Cleanup();
	DELETESINGLE(gApp);
	DELETESINGLE(gSampleCommandLine);
	releaseSampleAllocator();	
}

bool mainContinue()
{
	if (gApp->isOpen() && !gApp->isCloseRequested())
	{
		if(gApp->getInputMutex().trylock())
		{
			gApp->handleMouseVisualization();
			gApp->doInput();
			gApp->update();
#if !SEPARATE_EVENT_LOOP
			gApp->updateEngine();
#endif
			gApp->getInputMutex().unlock();
		}
		Thread::yield();
		return true;
	}

#if SEPARATE_EVENT_LOOP
	gApp->signalQuit();
	gApp->waitForQuit();
#else
	if (gApp->isOpen() || gApp->isCloseRequested())
		gApp->close();
#endif

	return false;
}

void mainLoop()
{
	while(mainContinue());
}

int main()
{
	gSampleCommandLine = new SampleCommandLine(GetCommandLineA());
	mainInitialize();
	mainLoop();
	mainTerminate();

	SAFE_DELETE(g_pDbgConfig);
	return 0;
}
