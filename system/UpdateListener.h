#ifndef INFOUPDATELISTENER_H_INCLUDED
#define INFOUPDATELISTENER_H_INCLUDED

class IUpdateListener
{
public:
	virtual void UpdateListener(wxCommandEvent& event) = 0;
};

#endif // INFOUPDATELISTENER_H_INCLUDED
