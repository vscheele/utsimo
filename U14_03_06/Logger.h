#ifndef LOGGER_H
#define LOGGER_H

#include "stdafx.h"

class Logger : public wxApp {
public:
	virtual bool OnInit ( ) ;

};


class LoggerFrame: public wxFrame
{
public:

   LoggerFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

};


DECLARE_APP(Logger)

#endif 
