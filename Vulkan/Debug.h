#pragma once
class Debug
{
public:
#ifdef _DEBUG
	static constexpr bool enableDebug = true;
#else 
	static constexpr bool enebleDebug = false;
#endif
};

