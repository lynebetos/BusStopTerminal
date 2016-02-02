#pragma once


// CWeatherStatic

class CWeatherStatic : public CStatic
{
	DECLARE_DYNAMIC(CWeatherStatic)

public:
	CWeatherStatic();
	virtual ~CWeatherStatic();

	void DrawWeather(_st_weather& stWeather);

private:
	_st_weather m_stWeather;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


