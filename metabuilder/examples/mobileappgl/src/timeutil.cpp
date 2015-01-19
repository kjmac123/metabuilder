#include "core.h"

#include "timeutil.h"

Timer::Timer()
{
	m_tickToSecondScale = (F64)Platform::GetSystemTicksToSecondsScale();
	Reset();
}

void Timer::Reset()
{
	m_startTick = Platform::GetSystemTicks();
}

F64 Timer::GetTimeSeconds() const
{
	U64 elapsedTicks = Platform::GetSystemTicks() - m_startTick;
	F64 t = (F64)elapsedTicks * m_tickToSecondScale;
	return t;
}

F64 Timer::GetTimeMilliseconds() const
{
	return GetTimeSeconds() * F64(MB_SECONDS_TO_MILLISECONDS);
}

