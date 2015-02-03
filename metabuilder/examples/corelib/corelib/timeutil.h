#ifndef TIMEUTIL_H
#define TIMEUTIL_H

class Timer
{
public:
	Timer();

	void Reset();

	F64 GetTimeSeconds() const;
	F64 GetTimeMilliseconds() const;

private:
	F64 m_tickToSecondScale;
	U64 m_startTick;
};

#endif
