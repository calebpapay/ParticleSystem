//----------------------------------------------------------------------------
// Copyright 2022, Ed Keenan, all rights reserved.
//----------------------------------------------------------------------------
//
// Thread Framework - for Keenan's C++ classes
//
//    One header to rule them all, 
//        one header to find them, 
//        one header to bring them all 
//        and in the Linker bind them.   <thank you Tolkien>
//
//    Don't try to understand it... its very complicated
//
//    Systems:
//        Dictionary - hold info {name,threadID,tabs} per thread
//        Banner - thread banner (begin/end) of thread
//        ThreadHelper - Front door
//    
// ---------------------------------------------------------------------------
//    v.1.00  Merge files / start
//    v.1.01 - Start Banner
//    v.1.02 - Fix banner printing
//    v.1.03 - Version control
//    v.1.04 - Remove entry from map when thread ended
//    v.1.05 - Deep copy of name in Banner
//    v.1.06 - new name -async defer- label
//    v.1.07 - problem with names on uninitialized thread names
//    v.1.08 - Fixed async/setName bug
//    v.1.09 - Cleanup
//    v.1.10 - remove exception trick
//    v.1.11 - ThreadID, MainBanner(),Create(), Destroy()
//    v.1.12 - THREAD_NAME_SIZE, ChangeCurrentName(), detection of async vs deferred
//    v.1.13 - Unified START_BANNER macro for derived or non-derived, poor rtti
//    v.1.14 - ChangeName() by std::thread
//    v.1.15 - helper privGetID(), privPrintTabs()
//    v.1.16 - Support calling numTabs, #THREAD_FRAMEWORK_H disable prints
//    v.1.17 - watermark 2021
//    v.1.18 - bugs with new compiler
//    v.1.19 - namespace scoping on defines START_MAIN_BANNER, START_BANNER
//    v.1.20 - SimpleBanner
//    v.1.21 - const fixes
//----------------------------------------------------------------------------- 

#ifndef THREAD_FRAMEWORK_VER_H
#define THREAD_FRAMEWORK_VER_H

#define THREAD_FRAMEWORK_VER "1.20"

#endif

#ifndef THREAD_FRAMEWORK_H
#define THREAD_FRAMEWORK_H

// Comment this out to turn off all thread-debugging i.e. prints
#define THREAD_FRAMEWORK_ENABLED

// -----------------------------------------------
//
// To Enable:
//      define THREAD_FRAMEWORK_ENABLED 
//
// In Compiler:
//      preprocessor define:
//           USE_THREAD_FRAMEWORK
//      forced include:
//           ThreadFramework.h
// 
// -----------------------------------------------
//  SUMMARY
// -----------------------------------------------
//
//   class Debug
//   {
//   public:
//		given threadName
//			static bool GetID(ThreadID &outID, const char *const threadName)
//		given std::Thread
//			static const char *const GetName(const std::thread &t0)
//			static ThreadID GetID(const std::thread &t0)
//			static bool SetName(const std::thread &t0, const char *threadName, int numTabs = 1)
//			static bool ChangeName(const std::thread &t0, const char *threadName)
//		given ThreadID
//			static const char *const GetName(const DWORD threadId)
//		inside Current thread
//			static const char *const GetCurrentName()
//			static ThreadID GetCurrentID()
//			static int GetCurrentNumTabs()
//			static bool SetCurrentName(const char *const threadName, int numTabs = 1)
//			static bool ChangeCurrentName(const char *const threadName)
//		same as Trace::out() but honor's tabs for specific threads
//			static void out(const char *const fmt, ...)
//   };
//
//   class BannerBase  <--- Functor Base for Thread Names and calling threads num tabs
//   {
//        BannerBase(const char *const _pName)
//   };
//
//   class Banner  <--- RAII for ThreadBanner
//   {
//   public:
//       Banner(void *_pBannerBase)  <-- used for all non-main banner
//       Banner(class BannerName *pBannerName)  <-- used for main banner
//       ~Banner();
//   }
//
//   class MainBanner : public Banner
//   {
//	 public:
//       MainBanner(const char *const pName)  <-- used for main banner
//   }
//
// Useful Macros
//
//   #define START_BANNER_MAIN(name) 
//   #define START_BANNER		
//


namespace ThreadFramework
{
#define THREAD_UNUSED_VAR(x) (void(x))

#ifdef THREAD_FRAMEWORK_ENABLED
	#define START_BANNER_MAIN(x)    ThreadFramework::MainBanner mainbanner(x);  
	#define START_BANNER			ThreadFramework::Banner banner(this);
#else
	#define START_BANNER_MAIN(x)    ThreadFramework::MainBanner mainbanner(x);
	#define START_BANNER			ThreadFramework::Banner banner(this);
#endif
}


#pragma warning( push )
#pragma warning( disable : 4625 )
#pragma warning( disable : 4626 )
#pragma warning( disable : 4774 )
#include <iostream>
#include <thread>
#include <sstream> 
#include <map>
#include <mutex>
#include <future>
#include <assert.h>
#pragma warning( pop ) 

#include <codecvt>
#include <string>

// --------------------------------------------------------------
// DICTIONARY
// --------------------------------------------------------------

namespace ThreadFramework
{
	typedef unsigned int ThreadID;

	class Dictionary
	{
	public:
		static const unsigned int THREAD_NAME_SIZE = 64;

		struct Data
		{
			char name[THREAD_NAME_SIZE];  // maximum name length 63+null
			int  numTabs;         // number of tabs for printing
			bool async;           // does thread have an active active deferred
			bool finalized;       // thread started with name
			char pad[2];          // alignment
		};

	public:
		Dictionary()
			: poThreadNames( new std::map<ThreadID, Data>() ),
			mtx()
		{
		}
		Dictionary(const Dictionary &) = delete;
		Dictionary &operator = (const Dictionary &) = delete;
		~Dictionary()
		{
			const std::lock_guard<std::mutex> lock(this->mtx);

			delete this->poThreadNames;
		}

		void Clear()
		{
			std::lock_guard<std::mutex> lock(this->mtx);
			assert(this->poThreadNames);
			this->poThreadNames->clear();
		}

		bool SetFinalized(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
			bool s = false;

			if (it == poThreadNames->end())
			{
				s = false;
			}
			else
			{
				it->second.finalized = true;
				s = true;
			}

			return s;
		}
		const Data *GetData(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
			Data *s = nullptr;

			if (it == poThreadNames->end())
			{
				s = nullptr;
			}
			else
			{
				s = &it->second;
			}

			return s;
		}
		bool SetData(ThreadID threadId, const Data *data)
		{
			std::lock_guard<std::mutex> lock(this->mtx);
			bool status = false;

			auto it = poThreadNames->find(threadId);

			if (it == poThreadNames->end())
			{
				status = false;
			}
			else
			{
				it->second = *data;
				status = true;
			}

			return status;
		}

		bool GetID(ThreadID &threadId, const char *const threadName)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			bool status = false;
			threadId = 0;
			for (auto it = poThreadNames->begin(); it != poThreadNames->end(); ++it)
			{
				if (strcmp(it->second.name, threadName) == 0)
				{
					// found it.
					status = true;
					threadId = it->first;
					break;
				}
			}

			return status;
		}

		char *GetName(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
			char *s = nullptr;

			if (it == poThreadNames->end())
			{
				s = nullptr;
			}
			else
			{
				s = it->second.name;
			}

			return s;
		}
		int GetNumTabs(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
			int numTabs = 0;

			if (it == poThreadNames->end())
			{
				numTabs = 0;
			}
			else
			{
				numTabs = it->second.numTabs;
			}

			return numTabs;
		}
		void SetNumTabs(ThreadID threadId, int count)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);

			if (it == poThreadNames->end())
			{
				assert(0);
			}
			else
			{
				it->second.numTabs = count;
			}
		}
		void ClearAsync(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);

			if (it == poThreadNames->end())
			{
				assert(0);
			}
			else
			{
				it->second.async = false;
			}
		}
		void SetAsync(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
	
			if (it == poThreadNames->end())
			{
				assert(0);
			}
			else
			{
				it->second.async = true;
			}
		}
		bool GetAsync(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			auto it = poThreadNames->find(threadId);
			bool flag = false;

			if (it == poThreadNames->end())
			{
				flag = false;
				//assert(0);
			}
			else
			{
				flag = it->second.async;
			}

			return flag;
		}

		bool AddToMap(const char *const threadName, ThreadID threadId, int _numTabs)
		{
			std::lock_guard<std::mutex> lock(this->mtx);
			
			bool status(true);

			// is the string name too big?
			const size_t N = strlen(threadName) + 1;
			assert(N <= THREAD_NAME_SIZE);

			// Fill the data struct
			Dictionary::Data data;

			memset(data.name, 0x0, THREAD_NAME_SIZE);
			strcpy_s(data.name, N, threadName);
			data.numTabs = _numTabs;
			data.async = false;
			data.finalized = false;

			// The name is now OWNED by the Map
			auto it = poThreadNames->emplace(threadId, data);

			// is that threadId already in the map?
			if (it.second == false)
			{				
				// duplicate entry... no transfer

				// hits here for deferred async
				//auto it2 = poThreadNames->find(threadId);
				//Trace::out("--> warning AddToMap(%d) repeated threadID using %s  \n", threadId, it2->second.name);
				
				status = false;
			}
			return status;
		}
		bool RemoveFromMap(ThreadID threadId)
		{
			std::lock_guard<std::mutex> lock(this->mtx);

			bool status = false;
			auto it = poThreadNames->find(threadId);
	
			if (it == poThreadNames->end())
			{
				Trace::out("ERROR:(%5d) <---- RemoveFromMap()\n", threadId);
				assert(false);
				status = false;
			}
			else
			{
				// it->second.name;
				// do something
				status = true;
				poThreadNames->erase(it);
			}

			return status;
		}

	private:
		std::map<ThreadID, Data> *poThreadNames;
		std::mutex mtx;
	};
}

// ---------------------------------------------------------------
// Locally static section
//----------------------------------------------------------------

namespace ThreadFramework
{

#ifdef THREAD_FRAMEWORK_ENABLED
	class Debug
	{
	public:
		static const unsigned int DebugBuffSize = 256;

	public:
		Debug(const Debug &) = delete;
		Debug &operator = (const Debug &) = delete;
		~Debug()
		{
			delete this->poDictionary;
		}

		static void Create()
		{
			Dictionary *pDict = new Dictionary();
			assert(pDict);

			privInstance(pDict, true);
		}
		static void Destroy()
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			delete pDebug->poDictionary;
			pDebug->poDictionary = nullptr;
			
		}

		// given threadName
		static bool GetID(ThreadID &outID, const char * const threadName)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			// Find entry in dictionary
			return pDebug->poDictionary->GetID(outID, threadName);
		}

		// given std::Thread
		static const char * const GetName(const std::thread &t0)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			const ThreadID threadId = pDebug->privGetID(t0);

			return pDebug->poDictionary->GetName(threadId);
		}
		static ThreadID GetID(const std::thread &t0)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			const ThreadID threadId = pDebug->privGetID(t0);

			return threadId;
		}
		static bool SetName(const std::thread &t0, const char *threadName, int numTabs = 1)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			const ThreadID threadId = pDebug->privGetID(t0);

			// register the name
			const bool status = pDebug->poDictionary->AddToMap(threadName, threadId, numTabs);
			if (status)
			{
				std::wstring wsTmp(&threadName[0], &threadName[strlen(threadName)]);

				HANDLE tHandle = static_cast<HANDLE>((const_cast<std::thread &>(t0)).native_handle());
				const HRESULT hr = SetThreadDescription(tHandle, wsTmp.c_str());
				AZUL_UNUSED_VAR(hr);
				assert(FAILED(hr) == S_OK);
			}

			return status;
		}

		static bool ChangeName(const std::thread &t0, const char *threadName)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			const ThreadID threadId = pDebug->privGetID(t0);
			bool status = false;
			
			const Dictionary::Data *pData = pDebug->poDictionary->GetData(threadId);
			if (pData != nullptr)
			{
				Dictionary::Data data = *pData;
				strcpy_s(&data.name[0], Dictionary::THREAD_NAME_SIZE, threadName);

				status = pDebug->poDictionary->SetData(threadId, &data);

				std::wstring wsTmp(&threadName[0], &threadName[strlen(threadName)]);

				HANDLE tHandle = static_cast<HANDLE>((const_cast<std::thread &>(t0)).native_handle());
				const HRESULT hr = SetThreadDescription(tHandle, wsTmp.c_str());
				AZUL_UNUSED_VAR(hr);
				assert(FAILED(hr) == S_OK);
			}
			else
			{
				status = false;
			}
			
			return status;

		}

		// given ThreadID
		static const char * const GetName(const DWORD threadId)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			return pDebug->poDictionary->GetName(threadId);
		}

		// inside Current thread...
		static const char * const GetCurrentName()
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			// Get id of the current thread
			const ThreadID threadId = ::GetThreadId(GetCurrentThread());

			return pDebug->poDictionary->GetName(threadId);
		}
		static ThreadID GetCurrentID()
		{
			// Get the string ID in an Integer form
			std::stringstream ss;

			// extracts the id
			ss << std::this_thread::get_id();

			// convert from string to number
			ThreadID id = std::stoul(ss.str());

			return id;
		}
		static int GetCurrentNumTabs()
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			// Get id of the current thread
			const ThreadID threadId = ::GetThreadId(GetCurrentThread());

			return pDebug->poDictionary->GetNumTabs(threadId);
		}
		static bool SetCurrentName(const char * const threadName, int numTabs = 1)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			// Get id of the current thread

			const ThreadID threadId = ::GetThreadId(GetCurrentThread());

			// register the name
			const bool status = pDebug->poDictionary->AddToMap(threadName, threadId, numTabs);

			if (status)
			{
				std::wstring wsTmp(&threadName[0], &threadName[strlen(threadName)]);

				const HRESULT hr = SetThreadDescription(GetCurrentThread(), wsTmp.c_str());
				AZUL_UNUSED_VAR(hr);
				assert(FAILED(hr) == S_OK);
			}

			return status;
		}
		static bool ChangeCurrentName(const char *const threadName)
		{
			bool status = false;

			Debug *pDebug = privInstance();
			assert(pDebug);

			// Get id of the current thread
			const ThreadID threadId = ::GetThreadId(GetCurrentThread());

			// Get the Name... change it.
			char *pOldName = pDebug->poDictionary->GetName(threadId);
			if (pOldName != nullptr)
			{
				strcpy_s(pOldName, Dictionary::THREAD_NAME_SIZE, threadName);

				std::wstring wsTmp(&threadName[0], &threadName[strlen(threadName)]);

				const HRESULT hr = SetThreadDescription(GetCurrentThread(), wsTmp.c_str());
				AZUL_UNUSED_VAR(hr);
				assert(FAILED(hr) == S_OK);

				status = true;
			}
			else
			{
				assert(pOldName);
				status = false;
			}
			return status;
		}


		// Same as Debug::privOut() but honor's tabs for specific threads
		static void out(const char *const fmt, ...)
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			std::lock_guard<std::mutex> lock(pDebug->debugMtx);

			// indent + 1 more 
			const size_t numTabs = (size_t)Debug::GetCurrentNumTabs() + 1;

			va_list args;

			#pragma warning( push )
				#pragma warning( disable : 26492 )
				#pragma warning( disable : 26481 )
				va_start(args, fmt);
			#pragma warning( pop )

			char s[Debug::DebugBuffSize];
			char *p = Debug::privAddTabs(s, numTabs);

			if (pDebug->poDictionary->GetAsync(Debug::GetCurrentID()))
			{
				const char *pThreadName = Debug::GetCurrentName();
				if (pThreadName)
				{
					// async deferred with name
					sprintf_s(p, DebugBuffSize - numTabs, "(%s): -async defer-: ", Debug::GetCurrentName());
				}
				else
				{
					// async deferred with no name... print threadID
					sprintf_s(p, DebugBuffSize - numTabs, "(%5d): -async defer-: ", Debug::GetCurrentID());
				}
			}
			else
			{
				const char *pThreadName = Debug::GetCurrentName();
				if (pThreadName)
				{
					// thread or async(launch async) with name
					sprintf_s(p, DebugBuffSize - numTabs, "(%s): ", Debug::GetCurrentName());
				}
				else
				{
					// if no name... print threadID
					sprintf_s(p, DebugBuffSize - numTabs, "(%5d): ", Debug::GetCurrentID());
				}
			}
			strcat_s(s, 256, fmt);

			char sB[Debug::DebugBuffSize] = { 0 };

			vsprintf_s(sB, 256, s, args);

			pDebug->privOut(sB);

			//va_end(args);
			args = static_cast<va_list> (nullptr);

		}
private:
		friend class Banner;		

public:  // HACK
		// displays a printf to the output window
		static void privOut(const char *const fmt, ...)
		{
			// Assumes its locked before this is called.
			Debug *pDebug = Debug::privInstance();
			assert(pDebug);

			std::lock_guard<std::mutex> lock(pDebug->printMtx);

			va_list args;

			#pragma warning( push )
			#pragma warning( disable : 26492 )
			#pragma warning( disable : 26481 )
				va_start(args, fmt);
			#pragma warning( pop )

			vsprintf_s(&pDebug->privBuff[0], DebugBuffSize, fmt, args);
			OutputDebugString(&pDebug->privBuff[0]);

			//va_end(args);
			args = static_cast<va_list> (nullptr);
		}

		static char * privAddTabs(char *s, size_t numTabs)
		{
			// Add the number of tabs into the banner
			//    A real PAIN we cannot tear Debug::privOut()
			//    Create a buffer... print into that, submit buff to Debug::privOut()

			char *p;

			// clear buffer;
			memset(s, 0x0, Debug::DebugBuffSize);
			p = s;
			
			// add number of tabs and move pointer
			size_t count = numTabs;
			while (count--)
			{
				*p++ = 0x9; // "\t"
			}

			return p;

		}
		const ThreadID privGetID(const std::thread &t0)
		{
			// OMG - a const cast?
			// reason... if you have a thread reference... I want no way of modifying the data
			// but to get the name for the debugger, I need to call native_handle which is mutable

			const ThreadID threadId = ::GetThreadId(
				static_cast<HANDLE>(
					(const_cast<std::thread &>(t0)).native_handle()
					)
			);

			return threadId;
		}

		static Dictionary *GetDictionary()
		{
			Debug *pDebug = privInstance();
			assert(pDebug);

			return pDebug->poDictionary;
		}

		Debug()
			: poDictionary(nullptr),
			debugMtx()
		{
		}
		static Debug *privInstance(Dictionary *pDict = nullptr, bool flag = false)
		{
			static Debug debug;

			if (flag)
			{
				debug.poDictionary = pDict;
			}

			return &debug;
		}

		Dictionary   *poDictionary;
		std::mutex   debugMtx;
		std::mutex   printMtx;

		char privBuff[DebugBuffSize];
	};

#else
	class Debug
	{
	public:
		static const unsigned int DebugBuffSize = 256;

	public:
		Debug() = default;
		Debug(const Debug &) = delete;
		Debug &operator = (const Debug &) = delete;
		~Debug() = default;

	// given threadName
		static bool GetID(ThreadID &, const char *const )
		{
			return false;
		}
	// given std::Thread
		static const char *const GetName(const std::thread &)
		{
			return nullptr;
		}
		static ThreadID GetID(const std::thread &)
		{
			return (ThreadID)-1;
		}
		static bool SetName(const std::thread &, const char *, int numTabs = 1)
		{
			THREAD_UNUSED_VAR(numTabs);
			return false;
		}
		static bool ChangeName(const std::thread &, const char *)
		{
			return false;
		}
	// given ThreadID
		static const char *const GetName(const DWORD )
		{
			return nullptr;
		}
	// inside Current thread
		static const char *const GetCurrentName()
		{
			return nullptr;
		}
		static ThreadID GetCurrentID()
		{
			return (ThreadID)-1;
		}
		static int GetCurrentNumTabs()
		{
			return 33333;
		}
		static bool SetCurrentName(const char *const , int numTabs = 1)
		{
			THREAD_UNUSED_VAR(numTabs);
			return false;
		}
		static bool ChangeCurrentName(const char *const )
		{
			return false;
		}
	// same as Trace::out() but honor's tabs for specific threads
		static void out(const char *const, ...)
		{

		}
	};

#endif

}

// --------------------------------------------------------------
// Banner
// --------------------------------------------------------------

namespace ThreadFramework
{
#ifdef THREAD_FRAMEWORK_ENABLED
	class BannerBase
	{
	public:
	public:
		BannerBase(const char *const _pName)
			: IsBannerBaseID{ 0x7890abcd },
			BannerName{ 0 },
			CallingThreadNumTabs{ Debug::GetCurrentNumTabs() }
		{
			strncpy_s(this->BannerName, Dictionary::THREAD_NAME_SIZE, _pName, strlen(_pName) + 1);
		}
		BannerBase() = delete;
		BannerBase(const BannerBase &) = default;
		BannerBase &operator=(const BannerBase &) = default;
		virtual ~BannerBase() = default;

		const char *const GetBannerName()
		{
			return this->BannerName;
		}
		uint32_t GetBannerBaseID() const
		{
			return this->IsBannerBaseID;
		}
		int GetCallingThreadNumTabs()
		{
			return this->CallingThreadNumTabs;
		}
	private:
		uint32_t IsBannerBaseID;  // poor man's rtti
		char BannerName[Dictionary::THREAD_NAME_SIZE];
		int CallingThreadNumTabs;
	};
#else
	class BannerBase
	{
	public:
		BannerBase(const char *const) {}

		BannerBase() = delete;
		BannerBase(const BannerBase &) = default;
		BannerBase &operator=(const BannerBase &) = default;
		virtual ~BannerBase() = default;
	};
#endif
}

namespace ThreadFramework
{
	class SimpleBanner
	{
	public:
		SimpleBanner()
		{
			this->pName = Debug::GetCurrentName();
			this->tabs = Debug::GetCurrentNumTabs();
			this->tID = Debug::GetCurrentID();

			privPrint();
		}

		~SimpleBanner()
		{
			privPrintEnd();
		}

	private:
		void privPrint()
		{
			const ThreadID _ThreadID = this->tID;

			const size_t numTabs = (size_t) this->tabs;

			char s[Debug::DebugBuffSize];
			char * p = this->privAddTabs(s, numTabs);

			sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: begin()\n",
				_ThreadID,
				this->pName);

			Debug::privOut(s);
		}

		void privPrintEnd()
		{
			const ThreadID _ThreadID = this->tID;

			const size_t numTabs = (size_t) this->tabs;

			char s[Debug::DebugBuffSize];
			char * p = this->privAddTabs(s, numTabs);

			sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: end()\n",
				_ThreadID,
				this->pName);

			Debug::privOut(s);
		}

		char * privAddTabs(char * s, size_t numTabs)
		{
			// Add the number of tabs into the banner
			//    A real PAIN we cannot tear Debug::privOut()
			//    Create a buffer... print into that, submit buff to Debug::privOut()

			char * p;

			// clear buffer;
			memset(s, 0x0, Debug::DebugBuffSize);
			p = s;

			// add number of tabs and move pointer
			size_t count = numTabs;
			while(count--)
			{
				*p++ = 0x9; // "\t"
			}

			return p;

		}

		ThreadID tID;
		const char * pName;
		int tabs;
	};
}

namespace ThreadFramework
{
#ifdef THREAD_FRAMEWORK_ENABLED
	class Banner
	{
	public:
		Banner(void *_pBannerBase)
			: bannerMtx(),
			isThread(true),
			isMainBanner(false),
			pDict(Debug::GetDictionary())
		{
			std::lock_guard<std::mutex> lock(this->bannerMtx);

			// poor man's rtti
			// I buried a magic number after the vptr
			// check the magic number to indicate if derived from base or not
			BannerBase *pBannerBase = (BannerBase *)_pBannerBase;
			if(0x7890abcd == pBannerBase->GetBannerBaseID())
			{
				// Derived from BannerBase
				const ThreadID _ThreadID = Debug::GetCurrentID();

				const Dictionary::Data *pData = pDict->GetData(_ThreadID);

				if (pData)
				{
					if (pData->finalized == false)
					{
						// previously defined
						char name[Dictionary::THREAD_NAME_SIZE];
						strncpy_s(name, Dictionary::THREAD_NAME_SIZE, pData->name, strlen(pData->name));

						const bool RemoveStatus = pDict->RemoveFromMap(_ThreadID);
						AZUL_UNUSED_VAR(RemoveStatus);
						assert(RemoveStatus);

						const bool status = Debug::SetCurrentName(name,pBannerBase->GetCallingThreadNumTabs()+1);
						this->isThread = status;

						const bool FinalizedStatus = pDict->SetFinalized(_ThreadID);
						AZUL_UNUSED_VAR(FinalizedStatus);
						assert(FinalizedStatus);

					}
					else
					{
						// Previous defined and finalized --> ASYNC
						const bool status = Debug::SetCurrentName(pBannerBase->GetBannerName(), pBannerBase->GetCallingThreadNumTabs()+1);
						this->isThread = status;
					}
				}
				else
				{
					// First time	
					const bool status = Debug::SetCurrentName(pBannerBase->GetBannerName(), pBannerBase->GetCallingThreadNumTabs()+1);
					this->isThread = status;

					const bool FinalizedStatus = pDict->SetFinalized(_ThreadID);
					AZUL_UNUSED_VAR(FinalizedStatus);
					assert(FinalizedStatus);
				}

				const size_t numTabs = (size_t)this->pDict->GetNumTabs(_ThreadID);

				char s[Debug::DebugBuffSize];
				char *p = Debug::privAddTabs(s, numTabs);

				if (this->isThread)
				{
					sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: begin()\n",
						_ThreadID,
						this->pDict->GetName(_ThreadID));

					Debug::privOut(s);
				}
				else
				{
					this->pDict->SetAsync(_ThreadID);

					const int _tabCount = this->pDict->GetNumTabs(_ThreadID) + 1;
					this->pDict->SetNumTabs(_ThreadID, _tabCount);

					Debug::out("start()\n");

				}
			}
			else
			{
				// _NOT_ Derived from BannerBase
				const ThreadID _ThreadID = Debug::GetCurrentID();

				const size_t numTabs = (size_t)this->pDict->GetNumTabs(_ThreadID);

				char s[Debug::DebugBuffSize];
				char *p = Debug::privAddTabs(s, numTabs);

				sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: begin()\n",
					_ThreadID,
					this->pDict->GetName(_ThreadID));

				Debug::privOut(s);
			}
		}

		Banner(const char *const pName)
			: bannerMtx(),
			isThread(true),
			isMainBanner(true),
			pDict(nullptr)
		{
			Debug::Create();
			Debug::SetCurrentName(pName, 0);

			pDict = Debug::GetDictionary();
			std::lock_guard<std::mutex> lock(this->bannerMtx);

			const ThreadID _ThreadID = Debug::GetCurrentID();

			const size_t numTabs = (size_t)this->pDict->GetNumTabs(_ThreadID);

			char s[Debug::DebugBuffSize];
			char *p = Debug::privAddTabs(s, numTabs);

			sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: begin()\n",
				_ThreadID,
				this->pDict->GetName(_ThreadID));

			Debug::privOut(s);
		}

		Banner() = delete;
		Banner(const Banner &) = default;
		Banner &operator = (const Banner &) = default;
		virtual ~Banner()
		{
			const std::lock_guard<std::mutex> lock(this->bannerMtx);

			const ThreadID _ThreadID = Debug::GetCurrentID();

			const size_t numTabs = (size_t)this->pDict->GetNumTabs(_ThreadID);

			char s[Debug::DebugBuffSize];
			char *p = Debug::privAddTabs(s, numTabs);

			if (this->isThread)
			{
				sprintf_s(p, Debug::DebugBuffSize - numTabs, "thread(%5d) %s: end()\n",
					_ThreadID,
					this->pDict->GetName(_ThreadID));
				Debug::privOut(s);

				const bool status = this->pDict->RemoveFromMap(_ThreadID);
				assert(status);
				AZUL_UNUSED_VAR(status);
			}
			else
			{
				Debug::out("end()\n"); 

				this->pDict->ClearAsync(_ThreadID);
				const int _tabCount = this->pDict->GetNumTabs(_ThreadID) - 1;
				this->pDict->SetNumTabs(_ThreadID, _tabCount);
			}

			if (this->isMainBanner)
			{
				Debug::Destroy();
			}
		};

		// Data ------------------
		std::mutex		 bannerMtx;
		bool             isThread;
		bool		     isMainBanner;
		Dictionary       *pDict;

	};

	class MainBanner : public Banner
	{
	public:
		MainBanner(const char * const pName)
		: Banner(pName)
		{
			std::lock_guard<std::mutex> lock(this->bannerMtx);

			const ThreadID _ThreadID = Debug::GetCurrentID();

			const bool FinalizedStatus = pDict->SetFinalized(_ThreadID);
			AZUL_UNUSED_VAR(FinalizedStatus);
			assert(FinalizedStatus);
		}

		MainBanner(const MainBanner &) = delete;
		MainBanner &operator = (const MainBanner &) = delete;
		~MainBanner()
		{
			//Debug::Destroy();
		}
	};
#else
class Banner
{
public:
	Banner(void *) {}
	Banner(const char *const) {}

	Banner() = default;
	Banner(const Banner &) = default;
	Banner &operator = (const Banner &) = default;
	virtual ~Banner() {}
};

class MainBanner : public Banner
{
public:
	MainBanner(const char *const) {}

	MainBanner(const MainBanner &) = delete;
	MainBanner &operator = (const MainBanner &) = delete;
	~MainBanner() = default;

};
#endif
}

#endif

// --- End of File ---
