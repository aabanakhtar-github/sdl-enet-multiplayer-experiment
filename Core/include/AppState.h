#include "Util.h"
#include <vector> 

enum class AppState
{
	AS_FAIL, 
	AS_QUIT,
	AS_LOOP
};

class GlobalAppState
{
	MAKE_SINGLETON(GlobalAppState)

	GlobalAppState() : m_AppState(AppState::AS_LOOP), m_Error() {}
public:
	void SetAppState(AppState state, const std::string& error = "")
	{
		m_AppState = state;
		if (error.size() > 0)
		{
			m_Error.push_back(error);
		}
	}

	AppState GetAppState() const
	{
		return m_AppState;
	}

	const std::vector<std::string>& GetError() const 
	{
		return m_Error;
	}

private:
	AppState m_AppState;
	std::vector<std::string> m_Error;
};
