#include <windows.h>
class Mutex {
private:
	HANDLE handle_;
public:
	Mutex(const char* name = "__my_mutex__")
	{
		
		handle_ = CreateMutex(NULL, FALSE, (LPCWSTR)(name));
		//handle_ = OpenMutex(MUTEX_ALL_ACCESS, FALSE, TEXT("__info_Mutex__"));
		if (handle_ == NULL)
			int tja = 3;
	}

	/*Mutex(const char* name){
	handle_ = CreateMutex(nullptr, false, (LPWSTR)(name));
	}
	*/
	~Mutex()
	{
		ReleaseMutex(handle_);
	}
	bool TryLock() 
	{
		DWORD tRet =  WaitForSingleObject(handle_, 0);
		if (tRet == WAIT_FAILED)
		{
			return true;
		}
		else if (tRet == WAIT_OBJECT_0)
		{
			return true;
		}
		else if (tRet == WAIT_TIMEOUT)
		{
			return true;
		}
		else if (tRet == WAIT_ABANDONED)
		{
			return true;
		}
		
	}

	bool Lock(DWORD milliseconds = 10000000000000) //inte bra att låsa forever ifall programmet dör, då unlockas den aldrig
	{
		DWORD check;
		check = WaitForSingleObject(handle_, milliseconds);
		//Sleep(milliseconds); //check returneras direkt så vänta ut skiten oxå??
		if (check == WAIT_ABANDONED) {
			return false; //didnt get mutex
		}

		return true; //got mutex
		
	}
	void Unlock()
	{
		ReleaseMutex(handle_);
	}
};
