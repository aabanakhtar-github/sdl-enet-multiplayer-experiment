#include "Util.h"
#include <vector> 

enum class AppState
{
	AS_DEFAULT,
	AS_FAIL, 
	AS_QUIT,
	AS_LOOP
};

class GlobalAppState
{
	MAKE_SINGLETON(GlobalAppState)
public:
	GlobalAppState() : m_AppState(AppState::AS_DEFAULT), m_Error() {}
	
	void SetAppState(AppState state, const std::string& error = "")
	{
		m_AppState = state;
		if (error.size() > 0)
		{
			m_Error.push_back(error);
		}
	}

	AppState GetAppState()
	{
		return m_AppState;
	}

	const std::vector<std::string>& GetError()
	{
		return m_Error;
	}

private:
	AppState m_AppState;
	std::vector<std::string> m_Error;
};
